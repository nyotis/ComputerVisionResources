/*
 *   Calibrator.h
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

#ifndef IRCALIBRATOR_H_
#define IRCALIBRATOR_H_

#include "OpenNIReader.h"
#include "ImageReader.h"
#include "DepthReader.h"
#include <OpenThreads/Thread>

#include "opencv2/core/core.hpp"

struct CameraCalibration
{
	cv::Mat extrinsicsR;
	cv::Mat extrinsicsT;
	cv::Mat intrinsics;
	cv::Mat distortion;


	// Default camera calibration
	// Initialise with standard values
	// width = image width in pixels
	// height = image height in pixels
	// F = fixed focal length in mm
	// pixelSize = pixel size (in mm) (note this is double reported value as
	//             we are using half size image wrt sensor
	CameraCalibration(int width = 640, int height = 480,
			int F = 120, float pixelSize = 0.2084) {

		float h2 = (((float) height / 2.0) - 0.5);
		float w2 = (((float) width / 2.0) - 0.5);
		float fxy = (float) F / (float) pixelSize;

		intrinsics = cv::Mat::eye(3, 3, CV_64FC1);
		intrinsics.at<double>(0, 0) = fxy;
		intrinsics.at<double>(1, 1) = fxy;
		intrinsics.at<double>(0, 2) = w2;
		intrinsics.at<double>(1, 2) = h2;

		distortion = cv::Mat::zeros(4, 1, CV_64FC1);

		extrinsicsR = cv::Mat::eye(3, 3, CV_64FC1);
		extrinsicsT = cv::Mat::zeros(1, 3, CV_64FC1);
	}

	CameraCalibration(std::string filename) {
		load(filename);
	}

	void load(std::string filename) {
		cv::FileStorage fs(filename.c_str(), cv::FileStorage::READ);
		if (fs.isOpened()) {
			fs["intrinsics"] >> intrinsics;
			fs["distortion"] >> distortion;
			fs["extrinsicsR"] >> extrinsicsR;
			fs["extrinsicsT"] >> extrinsicsT;
		}
	}

	void save(std::string filename) {
		cv::FileStorage fs(filename.c_str(), cv::FileStorage::WRITE);
		if (fs.isOpened()) {
			fs << "intrinsics" << intrinsics;
			fs << "distortion" << distortion;
			fs << "extrinsicsR" << extrinsicsR;
			fs << "extrinsicsT" << extrinsicsT;
		}
	}
};

class Calibrator {
	// Thread to continuously get the data
	class CalibrationThread : public OpenThreads::Thread
	{
	public :
		// Initialise with a depth reader to write to
		CalibrationThread (Calibrator *pCalibrator);
		// Run the thread
		virtual void run();
		// Stop the thread
		virtual int cancel();
	private:
		bool _running;
		// The depth reader to write to
		Calibrator *_pCalibrator;
	};
public:
	Calibrator(OpenNIReader *);
	virtual ~Calibrator();

	void calibrationLoop();
	void stopCalibrating();
	CameraCalibration getCalibration();
	void setCalibration(CameraCalibration cal);
	bool hasCalibration();
	void updateIntrinsics(){_updateIntrinsics = true;}
	void updateExtrinsics(){_updateExtrinsics = true;}
	void clearCalibration() {
		_hasCalibration = false;
		_cal = CameraCalibration();
	}

	cv::Mat getImage(bool processed);

	const cv::Mat &getMapX();
	const cv::Mat &getMapY();

	OpenNIReader *getReader(){return(_pReader);}

private:
	bool sample();
	void doUpdateIntrinsics();
	void doUpdateExtrinsics();

	void getFrame();

	OpenNIReader *_pReader;

	bool _hasCalibration;
	CalibrationThread *_pThread;

	CameraCalibration _cal;

	// Mutex to lock access to data
	OpenThreads::Mutex _lock;
	OpenThreads::Mutex _imageLock;

	// num Samples before we calibrate
	uint _numSamples;
	uint _numRows;
	uint _numCols;
	bool _updateIntrinsics;
	bool _updateExtrinsics;

	cv::Mat _image;
	cv::Mat _tempImage;
	cv::Mat _outImage;

	int _skipFrame;

	std::vector< std::vector< cv::Point2f> > _image_points;
	std::vector< std::vector< cv::Point3f> > _object_points;
	std::vector< cv::Point3f> _board_points;

	cv::Mat _mapx;
	cv::Mat _mapy;
};

#endif /* IRCALIBRATOR_H_ */
