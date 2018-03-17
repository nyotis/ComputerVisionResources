/*
 *   IRViewer.cpp
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

#include "IRViewer.h"
#include <osg/Geode>
#include <osg/PrimitiveSet>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include <iostream>

using namespace xn;
using namespace osg;
using namespace osgDB;

class IRUpdateCallback : public NodeCallback
{
public:
	IRUpdateCallback(IRViewer *pViewer) :
		_viewer(pViewer)
	{}

	virtual void operator()(osg::Node*, osg::NodeVisitor* nv)
	{
		_viewer->getFrame();
	}

private:
	IRViewer *_viewer;
};

IRViewer::IRViewer(IRReader *pIR):
	_width(0),
	_height(0) {

	_pIR = pIR;

	geode = new Geode();
	Geometry *mesh = new Geometry();

	Vec3Array *vertices = new Vec3Array(4);
	(*vertices)[0] = Vec3(0,0,1);
	(*vertices)[1] = Vec3(0,0,0);
	(*vertices)[2] = Vec3(1,0,0);
	(*vertices)[3] = Vec3(1,0,1);

	Vec2Array *uvs = new Vec2Array;
	uvs->push_back(Vec2(0,1));
	uvs->push_back(Vec2(0,0));
	uvs->push_back(Vec2(1,0));
	uvs->push_back(Vec2(1,1));

	DrawElementsUInt *faces = new DrawElementsUInt(PrimitiveSet::QUADS,0);
	faces->push_back(0);
	faces->push_back(1);
	faces->push_back(2);
	faces->push_back(3);

    mesh->setVertexArray(vertices);
    mesh->setTexCoordArray(0,uvs);
    mesh->addPrimitiveSet(faces);

	image = new Image();
	image->allocateImage(640, 480, 1, GL_RGB, GL_BYTE);

    texture = new Texture2D;
    texture->setDataVariance(osg::Object::DYNAMIC);
	texture->setImage(image);
    texture->setResizeNonPowerOfTwoHint(false);
    texture->setFilter(Texture2D::MIN_FILTER,Texture2D::LINEAR);
    texture->setFilter(Texture2D::MAG_FILTER,Texture2D::LINEAR);

    geode->addDrawable(mesh);
    mesh->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture,StateAttribute::ON);
    mesh->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::OFF );

    geode->setUpdateCallback(new IRUpdateCallback(this));

}

IRViewer::~IRViewer() {
	// TODO Auto-generated destructor stub
}

void IRViewer::getFrame()
{
	IRMetaData *pIR = dynamic_cast<IRMetaData *>(_pIR->read());
	int width = pIR->XRes();
	int height = pIR->YRes();

	if (width != _width || height != _height)
	{
		image->allocateImage(width, height, 1, GL_RGB, GL_BYTE);
		width = _width;
		height = _height;
	}

	unsigned char * pData = image->data();
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < width; y++)
		{
			if (x > 100 && y > 100)
			{
				*pData = 1;
				pData++;
				*pData = 1;
				pData++;
				*pData = 1;
				pData++;
			}
			else
			{
				*pData = 0;
				pData++;
				*pData = 0;
				pData++;
				*pData = 0;
				pData++;
			}
		}
	}

	/*const XnIRPixel *pData = pIR->Data();
	XnRGB24Pixel * pTexMap = (XnRGB24Pixel *)image->data();

	XnRGB24Pixel* pTexRow = pTexMap + pIR->YOffset() * _width;
	for (XnUInt y = 0; y < pIR->YRes(); ++y)
	{
		const XnIRPixel * pImage = pData;
		XnRGB24Pixel* pTex = pTexRow + pIR->XOffset();

		for (XnUInt x = 0; x < pIR->XRes(); ++x, ++pImage, ++pTex)
		{
			pTex->nBlue = *pImage >> 2;
			pTex->nGreen = *pImage >> 2;
			pTex->nRed = *pImage >> 2;
		}

		pData += pIR->XRes();
		pTexRow += _width;
	}*/

	image->dirty();

	return;
}

osg::ref_ptr<osg::Geode> IRViewer::getNode()
{
	return(geode);
}

