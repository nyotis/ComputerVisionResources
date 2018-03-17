/*
 *   MeshBuilder.h
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

#ifndef MESHBUILDER_H_
#define MESHBUILDER_H_

#include <osg/Geometry>
#include <OpenThreads/Thread>
#include "DepthReader.h"
#include "Calibrator.h"

class MeshBuilder {

	// Thread to continuously build the data
	class MeshBuilderThread: public OpenThreads::Thread {
	public:
		// Initialise
		MeshBuilderThread(MeshBuilder *pBuilder) :
				_builder(pBuilder), _running(true) {
		}
		// Override. Run the thread
		virtual void run() {
			while (_running) {
				_builder->buildMesh();
			}
		}
		// Override. Stop the thread
		virtual int cancel() {
			_running = false;
			microSleep(1000000);
			if (isRunning()) {
				return(Thread::cancel());
			}
			return (0);
		}
	private:
		MeshBuilder *_builder;
		bool _running;
	};

public:
	MeshBuilder(DepthReader *pDepth, std::string name, ImageReader *pImage = NULL);
	virtual ~MeshBuilder();

	void shutdown();

	void buildMesh();
	void updateMesh();
	void wait() {
		_dataWaiter.reset();
		_dataWaiter.block();
	}

	osg::ref_ptr<osg::Node> getMesh();

	void reset() {
		_reset = true;
	}

	void setIRCalibration(CameraCalibration IRCalibration){
		_IRCal = IRCalibration;
	}

	void setImageCalibration(CameraCalibration imageCalibration){
		_imgCal = imageCalibration;
	}

	void setCalibration(CameraCalibration calibration){
		_cal = calibration;
	}

	void setImage(ImageReader *pImage) {
		_pImage = pImage;
	}

	void pause();
	void resume();

private:

	std::string _name;

	DepthReader *_pDepth;
	ImageReader *_pImage;
	CameraCalibration _cal;
	CameraCalibration _IRCal;
	CameraCalibration _imgCal;

	unsigned int _width;
	unsigned int _height;
	XnUInt64 _F;
	XnDouble _pixelSize;
	bool _updated;
	bool _reset;
	bool _paused;

	unsigned short *_pSmoothData;

	float _h2;
	float _w2;
	float _pf;
	osg::ref_ptr<osg::MatrixTransform> _transform;
	osg::ref_ptr<osg::Geode> _geode;
	osg::ref_ptr<osg::Geometry> _geometry;
	osg::ref_ptr<osg::Vec3Array> _vertices;
	osg::ref_ptr<osg::Vec3Array> _normals;
	osg::ref_ptr<osg::Vec3Array> _colours;
	osg::ref_ptr<osg::DrawElementsUInt> _faces;

	MeshBuilderThread _thread;

	// Mutex to lock access to data
	OpenThreads::Mutex _dataLock;
	OpenThreads::BlockCount _dataWaiter;
};

#endif /* MESHBUILDER_H_ */
