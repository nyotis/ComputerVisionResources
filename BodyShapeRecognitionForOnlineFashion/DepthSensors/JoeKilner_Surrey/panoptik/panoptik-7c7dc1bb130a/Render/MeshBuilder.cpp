/*
 *   MeshBuilder.cpp
 *
 *	 Copyright(c) 2012 by Joe Kilner
 *   This file is part of Panoptik.
 *
 *   Panoptik is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Panoptik is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along with Panoptik.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "OpenNI.h"

#include "MeshBuilder.h"
#include <osg/PrimitiveSet>
#include <osg/Geode>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>

#include <iostream>
#include <algorithm>

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace xn;
using namespace osg;
using namespace osgDB;
using namespace std;
using namespace cv;

class MeshUpdateCallback: public NodeCallback {
public:
	MeshUpdateCallback(MeshBuilder *pBuilder) :
			_builder(pBuilder) {
	}

	virtual void operator()(osg::Node*, osg::NodeVisitor* nv) {
		_builder->updateMesh();
	}

private:
	MeshBuilder *_builder;
};

MeshBuilder::MeshBuilder(DepthReader *pDepth, string name, ImageReader *pImage) :
		_name(name),
		_pDepth(pDepth),
		_pImage(pImage),
		_updated(false),
		_reset(false),
		_paused(false),
		_thread(this),
		_dataWaiter(5) {
	_transform = new MatrixTransform();
	_geode = new Geode();

	_F = pDepth->getFocalLength();
	_pixelSize = pDepth->getPixelSize();

	_width = pDepth->read()->getWidth();
	_height = pDepth->read()->getHeight();

	_h2 = _height / 2;
	_w2 = _width / 2;
	_pf = 2.0 * _pixelSize / (float) _F;

	_geometry = new Geometry();

	_geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	_geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	_geometry->setUseDisplayList(false);
	//_geometry->setUseVertexBufferObjects(true);

	_geode->addDrawable(_geometry);
	_geode->setUpdateCallback(new MeshUpdateCallback(this));
	_transform->addChild(_geode);
	_transform->setName(name);

	_pSmoothData = new unsigned short[_height * _width];
	for (unsigned int i = 0; i < _height * _width; ++i) {
		_pSmoothData[i] = 0;
	}

	/*Group *pGroup = dynamic_cast<Group *>(readNodeFile("axes.osg"));
	 _transform->addChild(pGroup);*/

	// Finally kick off the building
	_thread.start();
}

MeshBuilder::~MeshBuilder() {
	shutdown();
}

void MeshBuilder::shutdown() {
	if (_thread.isRunning()) {
		_thread.cancel();
	}
}

void MeshBuilder::buildMesh() {
	DepthBufferData *pDepth = dynamic_cast<DepthBufferData *>(_pDepth->read());
	ImageBufferData *pImage(0);
	if (_pImage) {
		pImage = dynamic_cast<ImageBufferData *>(_pImage->read());
	}

	unsigned int numVertices = _height * _width;
	int numFaces = (numVertices + 1 - _width - _height) * 2;

	Vec3Array *pVertices = new Vec3Array;
	pVertices->reserve(numVertices);

	Vec3Array *pNormals = new Vec3Array;
	pNormals->reserve(numVertices);

	Vec3Array *pColours = new Vec3Array;
	pColours->reserve(numVertices);

	DrawElementsUInt *pFaces = new DrawElementsUInt(PrimitiveSet::TRIANGLES, 0);
	pFaces->reserve(numFaces);

	unsigned short *pNewData = new unsigned short[_height * _width];
	if (pNewData == 0) {
		return;
	}
	pDepth->putData(pNewData);

	unsigned char *pImageData(0);
	if (pImage) {
		pImageData = new unsigned char[_height * _width * 3];
		if(pImageData) {
			pImage->putData(pImageData, true);
		}
	}

	// This weird bit of code is to stop a partial resetting...
	bool reset = false;
	if (_reset) {
		_dataLock.lock();
		reset = true;
		_reset = false;
		_dataLock.unlock();
	}
	unsigned int i = 0;
	for (unsigned short *inData(pNewData), *outData(_pSmoothData);
			i < _height * _width; ++inData, ++outData, ++i) {
		if (*outData == 0 || *inData == 0 || reset) {
			*outData = *inData;
		} else {
			*outData = (*inData + (2 * (*outData))) / 3.0;
		}
	}
	delete [] pNewData;
	unsigned short *pData = _pSmoothData;

	vector<Point2f> inPts;
	vector<Point2f> outPts;
	for (unsigned int y = 0; y < _height; ++y) {
		for (unsigned int x = 0; x < _width; ++x) {
			// TODO - Apply IR / Depth calibration offset
			inPts.push_back(Point2f(float(x)+3.5, float(y)+3.5));
		}
	}

	undistortPoints(Mat(inPts), outPts, _IRCal.intrinsics, _IRCal.distortion);

	vector<int> id_map(numVertices, -1);
	vector<Point3f> points3d;

	int *indx_a = &(id_map[0]);
	int *indx_b = indx_a - 1;
	int *indx_c = indx_a - _width;
	int *indx_d = indx_a - _width - 1;

	for (unsigned int vert_index = 0; vert_index < inPts.size();
			++vert_index, ++indx_a, ++indx_b, ++indx_c, ++indx_d, ++pData) {
		float z(*pData * 0.001); // from mm to meters!
		if (z != 0 && z < 2.0) {
			*indx_a = pVertices->size();
			float x(outPts[vert_index].x * z);
			float y(outPts[vert_index].y * z);
			pVertices->push_back(Vec3(x,y,z));
			points3d.push_back(Point3f(x,y,z));
			pNormals->push_back(Vec3(0, 0, 0));

			if (vert_index > _width) {
				if (vert_index % _width != 0) {
					if (*indx_a != -1 && *indx_d != -1) {
						Vec3 vA = (*pVertices)[*indx_a];
						Vec3 a = (*pVertices)[*indx_d] - vA;
						float filt = z * 0.05;
						if (a.length() < filt) {
							if (*indx_b != -1) {
								Vec3 b = (*pVertices)[*indx_b] - vA;
								if (b.length() < filt) {
									Vec3 n = a ^ b;
									n.normalize();
									pFaces->push_back(*indx_a);
									pFaces->push_back(*indx_d);
									pFaces->push_back(*indx_b);

									(*pNormals)[*indx_a] += n;
									(*pNormals)[*indx_d] += n;
									(*pNormals)[*indx_b] += n;
								}
							}
							if (*indx_c != -1) {
								Vec3 c = (*pVertices)[*indx_c] - vA;
								if (c.length() < filt) {
									Vec3 n = c ^ a;
									n.normalize();
									pFaces->push_back(*indx_a);
									pFaces->push_back(*indx_c);
									pFaces->push_back(*indx_d);

									(*pNormals)[*indx_a] += n;
									(*pNormals)[*indx_c] += n;
									(*pNormals)[*indx_d] += n;
								}
							}
						}
					}
				}
			}
		}
	}

	// Normalise the normals
	for_each(pNormals->begin(), pNormals->end(), mem_fun_ref(&Vec3::normalize));

	// Set up colour array
	pColours->resize(pNormals->size(), Vec3(0,0,0));

	// Get colours from image if we have one
	if (pImageData && points3d.size()) {
		vector<Point2f> imgPts;
		Mat R,T;
		R = cv::Mat::eye(3, 3, CV_64FC1);
		T = cv::Mat::zeros(1, 3, CV_64FC1);

		for (int i = 0 ; i < 3; ++i) {
			for (int j = 0 ; j < 3; ++j) {
				R.at<double>(i,j) = _imgCal.extrinsicsR.row(i).dot(_IRCal.extrinsicsR.row(j));
			}
			T.at<double>(0,i) = _imgCal.extrinsicsT.at<double>(0,i) - R.row(i).dot(_IRCal.extrinsicsT);
		}

		// Project points from IR camera space into image space
		projectPoints(points3d, R, T, _imgCal.intrinsics, _imgCal.distortion, imgPts);
		for (uint i = 0; i < imgPts.size(); ++i) {
			unsigned int idx((((int)imgPts[i].y * _width) + imgPts[i].x));
			if (idx <  (_height * _width)) {
				// multiply by 3 as there are RGB per pixel
				idx *= 3;
				(*pColours)[i] = Vec3(float(pImageData[idx]) / 255.0,
									  float(pImageData[idx+1]) / 255.0,
									  float(pImageData[idx+2]) / 255.0);
			}
		}
	}
	else {
		// Copy normals in as colours
		for(Vec3Array::iterator normIt = pNormals->begin(), colIt = pColours->begin();
				normIt != pNormals->end();
				++normIt, ++colIt) {
			(*colIt) = Vec3(((*normIt)[0]+1)/2,
				(-(*normIt)[1]+1)/2,
				(-(*normIt)[2]+1)/2);
		}
	}

	// This syntax is OK as pImageData is either allocated memory or 0
	delete [] pImageData;

	_dataLock.lock();
	_updated = true;
	_vertices = pVertices;
	_normals = pNormals;
	_colours = pColours;
	_faces = pFaces;
	_dataLock.unlock();

	_dataWaiter.completed();
	return;
}

void MeshBuilder::updateMesh() {
	_dataLock.lock();
	if (!_paused && _updated && _vertices.valid()) {
		if (_pImage) {
			_geode->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::OFF | StateAttribute::OVERRIDE);
		}
		else {
			_geode->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::ON | StateAttribute::OVERRIDE);
		}
		_geometry->setVertexArray(_vertices);
		_geometry->setNormalArray(_normals);
		_geometry->setColorArray(_colours);
		if (_geometry->getNumPrimitiveSets() != 0) {
			_geometry->removePrimitiveSet(0, _geometry->getNumPrimitiveSets());
		}
		_geometry->addPrimitiveSet(_faces);
		_updated = false;
	}
	// Apply world-space transformation got via ICP and calibration tools
	cv::Mat T = _cal.extrinsicsT.clone();
	cv::Mat R = _cal.extrinsicsR.clone();
	Matrix mat(R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2), 0,
			   R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2), 0,
			   R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2), 0,
			   T.at<double>(0, 0), T.at<double>(0, 1), T.at<double>(0, 2), 1);
	_transform->setMatrix(mat);

	_dataLock.unlock();
}

void MeshBuilder::pause()
{
	_dataLock.lock();
	_paused = true;
	_dataLock.unlock();
}

void MeshBuilder::resume()
{
	_dataLock.lock();
	_paused = false;
	_dataLock.unlock();
}
osg::ref_ptr<osg::Node> MeshBuilder::getMesh() {
	Node *retVal = _transform;
	/*Group *pGroup = dynamic_cast<Group *>(readNodeFile("axes_2.osg"));
	 if (pGroup) {
	 pGroup->addChild(_transform);
	 retVal = pGroup;
	 }*/
	return (retVal);
}

