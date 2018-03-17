/*
 *   IRCalibrator.cpp
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

#include "Calibrator.h"

#include <iostream>
#include <sstream>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"

using namespace std;
using namespace cv;
using namespace xn;

Calibrator::Calibrator(OpenNIReader *pReader) {
	_pReader = pReader;
	_hasCalibration = false;
	_pThread = 0;
	_numSamples = 40;
	_numCols = 10;
	_numRows = 7;
	_updateIntrinsics = false;
	_updateExtrinsics = false;
	_skipFrame = 0;

	for (uint j = 0; j < _numRows; j++) {
		for (uint i = 0; i < _numCols; i++) {
			float x = float(i * 35) / 1000.0;
			float y = float(j * 35) / 1000.0;
			_board_points.push_back(Point3f(x, y, 0));
		}
	}

	getFrame();

	_pThread = new CalibrationThread(this);
	_pThread->start();
}

Calibrator::~Calibrator() {
	_pThread->cancel();
	delete _pThread;
}

void Calibrator::getFrame() {
	// Get Data
	BufferData *pData = _pReader->read();
	Size newSize = Size(pData->getWidth(), pData->getHeight());
	_imageLock.lock();
	if (newSize.width != _image.size().width
			|| newSize.height != _image.size().height) {
		_image = Mat(newSize, CV_8UC1);
	}

	pData->putData(_image.data, false);
	_imageLock.unlock();
}

void Calibrator::stopCalibrating() {
	_pThread->cancel();
}

void Calibrator::calibrationLoop() {
	bool sampled = false;
	getFrame();
	_skipFrame = (_skipFrame + 1) % 10;

	// TODO: tempImage and outImage are only for displaying. Should
	// switch off if no image subscriber is attached
	stringstream message;
	message << _object_points.size() << " / " << _numSamples;
	cvtColor(_image, _tempImage, CV_GRAY2BGR);
	putText(_tempImage, message.str().c_str(), Point(20, 40),
			CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, 1.0,
			cvScalar(255, 255, 0));
	if (_updateIntrinsics || _updateExtrinsics) {
		sampled = sample();
	}
	if (sampled && _updateIntrinsics) {
		doUpdateIntrinsics();
	}
	if (sampled && _updateExtrinsics) {
		doUpdateExtrinsics();
	}

	if (hasCalibration()) {
		rectangle(_tempImage, Point(10, 10),
				Point(_tempImage.cols - 10, _tempImage.rows - 10),
				CV_RGB( 0,255,0 ), 1);

		rectangle(_tempImage, Point(30, 30),
				Point(_tempImage.cols - 30, _tempImage.rows - 30),
				CV_RGB( 0,255,0 ), 1);

		rectangle(_tempImage, Point(50, 50),
				Point(_tempImage.cols - 50, _tempImage.rows - 50),
				CV_RGB( 0,255,0 ), 1);

		rectangle(_tempImage, Point(70, 70),
				Point(_tempImage.cols - 70, _tempImage.rows - 70),
				CV_RGB( 0,255,0 ), 1);

		if (sampled) {
			vector<Point2f> points;
			projectPoints(Mat(_board_points), _cal.extrinsicsR,
					_cal.extrinsicsT, _cal.intrinsics, _cal.distortion, points);
			circle(_tempImage, points[0], 4, CV_RGB( 0, 255, 255), 2);
			for (uint i = 1; i < points.size(); ++i) {
				circle(_tempImage, points[i], 4, CV_RGB( 255, 0, 0), 2);
			}
		}

		_imageLock.lock();
		remap(_tempImage, _outImage, getMapX(), getMapY(), INTER_LINEAR,
				BORDER_CONSTANT, 0);

		rectangle(_outImage, Point(10, 10),
				Point(_outImage.cols - 10, _outImage.rows - 10),
				CV_RGB( 255,255,0 ), 1);

		rectangle(_outImage, Point(30, 30),
				Point(_outImage.cols - 30, _outImage.rows - 30),
				CV_RGB( 255,255,0 ), 1);

		rectangle(_outImage, Point(50, 50),
				Point(_outImage.cols - 50, _outImage.rows - 50),
				CV_RGB( 255,255,0 ), 1);

		rectangle(_outImage, Point(70, 70),
				Point(_outImage.cols - 70, _outImage.rows - 70),
				CV_RGB( 255,255,0 ), 1);

		_imageLock.unlock();
	} else {
		_imageLock.lock();
		_outImage = _tempImage;
		_tempImage.release();
		_imageLock.unlock();
	}
}

bool Calibrator::sample() {
	uint board_total = _numCols * _numRows;

	vector<Point2f> corners;

	//Find chessboard corners:
	bool found = findChessboardCorners(
			_image,
			Size(_numCols, _numRows),
			corners,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS
			 | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

	if (found) {
		//Get Subpixel accuracy on those corners
		cornerSubPix(
				_image,
				corners,
				Size(5, 5) /*cvSize(11,11)*/,
				Size(-1, -1),
				TermCriteria(TermCriteria::EPS | TermCriteria::MAX_ITER, 30,
						0.1));
	} else {
		//findCirclesGrid();
	}

	if (corners.size() != 0) {
		drawChessboardCorners(_tempImage, Size(_numCols, _numRows),
				Mat(corners), found);
		circle(_tempImage, corners[0], 6, CV_RGB( 255, 0, 255), 2);
	}

	// If we got a good board, add it to our data
	if (corners.size() == board_total && !_skipFrame) {
		_image_points.push_back(corners);
		_object_points.push_back(_board_points);
		circle(_tempImage, Point(40, 40), 30, CV_RGB( 0, 255, 0), 2);
		return (true);
	}

	return (false);
}

void Calibrator::doUpdateIntrinsics() {
	if (_object_points.size() == _numSamples) {
		putText(_outImage, "Thinking...", Point(40, 40),
				CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, 1.0,
				cvScalar(255, 255, 0));

		vector<Mat> rvecs;
		vector<Mat> tvecs;

		// Create and initialise calibration
		CameraCalibration cal(_cal);

		cout << "--------------------------" << endl;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				cout << cal.intrinsics.at<double>(i, j) << ", ";
			}
			cout << endl;
		}

		double error = calibrateCamera(
				_object_points,
				_image_points,
				_image.size(),
				cal.intrinsics,
				cal.distortion,
				rvecs,
				tvecs,
				CV_CALIB_USE_INTRINSIC_GUESS | CV_CALIB_ZERO_TANGENT_DIST );

		Rodrigues(rvecs.back(), cal.extrinsicsR);
		cal.extrinsicsT = tvecs.back();

		cout << "++++++++++++++++++++++++++" << endl;
		cout << "ERROR = " << error << endl;
		cout << "INTRINSICS" << endl;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				cout << cal.intrinsics.at<double>(i, j) << ", ";
			}
			cout << endl;
		}

		cout << "DISTORTION" << endl;
		for (int i = 0; i < 4; i++) {
			cout << cal.distortion.at<double>(i, 0) << ", ";
		}
		cout << endl;

		cout << "==========================" << endl;

		setCalibration(cal);

		_image_points.clear();
		_object_points.clear();
		_updateIntrinsics = false;
	}
}

void Calibrator::doUpdateExtrinsics() {
	cout << "In doUpdateExtrinsics" << endl;
	CameraCalibration cal(_cal);

	Mat rotVec = Mat(1, 3, CV_64FC1);
	Mat transVec = Mat(1, 3, CV_64FC1);

	CvMat objectPts = Mat(_board_points);
	CvMat imagePts = Mat(_image_points.back());
	CvMat intrinsics = cal.intrinsics;
	CvMat distortion = cal.distortion;
	CvMat rotation = rotVec;
	CvMat translation = transVec;

	//cv::stereoCalibrate()

	cvFindExtrinsicCameraParams2(&objectPts, &imagePts, &intrinsics,
			&distortion, &rotation, &translation, 0);

	Rodrigues(rotVec, cal.extrinsicsR);
	cal.extrinsicsT = transVec;

	cout << "ROTATION" << endl;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			cout << cal.extrinsicsR.at<double>(i, j) << ", ";
		}
		cout << endl;
	}

	cout << "TRANSLATION" << endl;
	for (int i = 0; i < 3; i++) {
		cout << cal.extrinsicsT.at<double>(0, i) << ", ";
	}
	cout << endl;

	cout << "==========================" << endl;

	setCalibration(cal);

	_image_points.clear();
	_object_points.clear();
	_updateExtrinsics = false;
}

CameraCalibration Calibrator::getCalibration() {
	_lock.lock();
	CameraCalibration cal = _cal;
	_lock.unlock();
	return (cal);
}

bool Calibrator::hasCalibration() {
	return (_hasCalibration);
}

void Calibrator::setCalibration(CameraCalibration cal) {
	_lock.lock();
	_hasCalibration = true;
	_cal = cal;
	_mapx = Mat();
	_mapy = Mat();
	_lock.unlock();
}

const Mat &Calibrator::getMapX() {
	if (hasCalibration() && _mapx.rows == 0) {
		initUndistortRectifyMap(_cal.intrinsics, _cal.distortion, Mat(), Mat(),
				_image.size(), CV_32FC1, _mapx, _mapy);
	}
	return (_mapx);
}

const Mat &Calibrator::getMapY() {
	if (hasCalibration() && _mapy.rows == 0) {
		initUndistortRectifyMap(_cal.intrinsics, _cal.distortion, Mat(), Mat(),
				_image.size(), CV_32FC1, _mapx, _mapy);
	}
	return (_mapy);
}

Mat Calibrator::getImage(bool processed) {
	// Force wait until current processing is finished

	_imageLock.lock();
	Mat retImage;
	if (processed) {
		retImage = _outImage.clone();
	} else {
		retImage = _image.clone();
	}
	_imageLock.unlock();
	return (retImage);
}

/*
 * Just get the guy to write to.
 */
Calibrator::CalibrationThread::CalibrationThread(Calibrator *pCalibrator) :
		_running(true), _pCalibrator(pCalibrator) {
}

void Calibrator::CalibrationThread::run() {
	while (_running) {
		microSleep(50000);
		_pCalibrator->calibrationLoop();
	}
}

int Calibrator::CalibrationThread::cancel() {
	_running = false;
	sleep(2);
	return (Thread::cancel());
}

