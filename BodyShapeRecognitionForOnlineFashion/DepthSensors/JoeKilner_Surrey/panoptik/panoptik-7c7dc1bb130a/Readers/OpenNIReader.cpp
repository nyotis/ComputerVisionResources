/*
 *   OpenNIReader.cpp
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

#include "OpenNIReader.h"
#include "OpenNIReaderThread.h"
#include "DepthReader.h"
#include "ImageReader.h"
#include "IRReader.h"
#include "Motor.h"

#include <iostream>

using namespace xn;
using namespace std;

int OpenNIReader::numDevices(0);

///////////////////////////////////////////////////////////////
// Static functions - public may be called from any thread.
int OpenNIReader::init() {
	checkthreadOther();
	OpenNIReaderThread::get()->start();
	wait();
	return(numDevices);
}

void OpenNIReader::wait() {
	checkthreadOther();
	OpenNIReaderThread::get()->wait();
}

void OpenNIReader::shutdown() {
	checkthreadOther();
	OpenNIReaderThread::shutdown();
}

void OpenNIReader::pauseAllReading()
{
	checkthreadOther();
	OpenNIReaderThread::get()->pauseReading();
};

void OpenNIReader::resumeAllReading()
{
	checkthreadOther();
	OpenNIReaderThread::get()->resumeReading();
	OpenNIReaderThread::get()->wait();
};

///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
// Member functions - public may be called from any thread.
BufferData *OpenNIReader::read()
{
	checkthreadOther();
	_lock.lock();
	BufferData *buffer = _bufferData[_bufferIndex];
	_swapNextWrite = true;
	_lock.unlock();
	return(buffer);
}

// Get meta-data about the camera
XnUInt64 OpenNIReader::getFocalLength(){
	checkthreadOther();
	_lock.lock();
	XnUInt64 focalLength = _focalLength;
	_lock.unlock();
	return(focalLength);
}
XnDouble OpenNIReader::getPixelSize(){
	checkthreadOther();
	_lock.lock();
	XnDouble pixelSize = _pixelSize;
	_lock.unlock();
	return(pixelSize);
}

void OpenNIReader::stopReading()
{
	checkthreadOther();
	_lock.lock();
	_stopping = true;
	_lock.unlock();
}

void OpenNIReader::startReading()
{
	checkthreadOther();
	_lock.lock();
	_starting = true;
	_lock.unlock();
}

bool OpenNIReader::viewFrom(OpenNIReader *pReader)
{
	if (_hasAVCap && (pReader->getDeviceID() == getDeviceID())) {
		_lock.lock();
		_masterView = pReader;
		_lock.unlock();
		return(true);
	}
	return(false);
}

// Can be called from any thread - only copies value set in constructor so
// no need for any concurrency checks.
int OpenNIReader::getDeviceID() {
	return(_deviceID);
}

///////////////////////////////////////////////////////////////
// Member functions - protected may be called from any thread.
OpenNIReader::OpenNIReader(int deviceID, int mode) :
		tilt(0),
		_deviceID(deviceID),
		_mode(mode),
		_focalLength(0),
		_pixelSize(0),
		_hasAVCap(false),
		_stopping(false),
		_starting(true),
		_generator(0),
		_masterView(0),
		_metaData(0),
		_swapNextWrite(false),
		_bufferIndex(0){
	checkthreadOther();
	_bufferData[0] = 0;
	_bufferData[1] = 0;
	OpenNIReaderThread::get()->addReader(this);
	_name = '0' + deviceID;
}

OpenNIReader::~OpenNIReader() {
	checkthreadOther();
	stopReading();
	OpenNIReaderThread::get()->removeReader(this);
	delete _generator;
	_generator = 0;
	delete _bufferData[0];
	_bufferData[0] = 0;
	delete _bufferData[1];
	_bufferData[1] = 0;
}
///////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////
// Static functions - protected may only be called from
// OpenNIReaderThread thread

/*
 * Initialise the OpenNI context
 */
bool OpenNIReader::initOpenNI()
{
	checkthreadOpenNI();
	// ONLY TO BE CALLED ON OpenNIReaderThread thread
	XnStatus rc;

	EnumerationErrors errors;
	rc = getContext()->Init();
	if (rc == XN_STATUS_NO_NODE_PRESENT)
	{
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		printf("%s\n", strError);
		return(false);
	}
	else if (rc != XN_STATUS_OK)
	{
		printf("Open failed: %s\n", xnGetStatusString(rc));
		return(false);
	}

	NodeInfoList device_list;
	rc = getContext()->EnumerateProductionTrees(XN_NODE_TYPE_DEVICE,NULL,device_list,NULL);
	numDevices = 0;
	for (NodeInfoList::Iterator i = device_list.Begin();
			i != device_list.End();
			++i, numDevices++)
	{}

	if (numDevices) {
		// Could do this a bit neater rather than hard-coding for all derived types
		if (!DepthReader::init()) {
			numDevices = 0;
		}
		if (!ImageReader::init()) {
			numDevices = 0;
		}
		if (!IRReader::init()) {
			numDevices = 0;
		}
	}

	// If we knew more about which device was whitch we could do some
	// some interesting status display here. For now we can just update all
	// connected devices
	for (int i = 0 ; i < numDevices; ++i) {
		DeviceMotor::getMotor(i)->setLight(DeviceMotor::LED_GREEN);
	}

	return(numDevices != 0);
}

void OpenNIReader::waitOpenNI() {
	checkthreadOpenNI();
	XnStatus rc = getContext()->WaitAndUpdateAll();
	if (rc != XN_STATUS_OK)
	{
		printf("Waiting interrupted or failed: %s\n", xnGetStatusString(rc));
	}
}
void OpenNIReader::shutdownOpenNI() {
	checkthreadOpenNI();
	for (int i = 0 ; i < numDevices; ++i) {
	    DeviceMotor::getMotor(i)->setLight(DeviceMotor::LED_BLINK_GREEN);
	}
	getContext()->StopGeneratingAll();
	delete getContext();
}

Context *OpenNIReader::getContext() {
	checkthreadOpenNI();
	static Context *context(0);
	if (!context) {
		context = new Context;
	}
	return(context);
}

///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// Member functions - protected may only be called from
// OpenNIReaderThread thread

// Fetch data - API call
void OpenNIReader::fetch()
{
	checkthreadOpenNI();
	// Fetch data into the other buffer
	_lock.lock();
	if (_masterView) {
		_generator->GetAlternativeViewPointCap().SetViewPoint(*(_masterView->getGenerator()));
		_masterView = 0;
	}
	if (_swapNextWrite) {
		_bufferIndex = 1-_bufferIndex;
		_swapNextWrite = false;
	}
	if (_stopping) {
		if (_generator) {
			_generator->StopGenerating();
		}
		_stopping = false;
	}
	if ((_starting)) {
		if (!_generator) {
			createGenerator();
		}
	    _generator->StartGenerating();
		_starting = false;
	}
	if (_generator) {
		_bufferData[_bufferIndex]->setData(_metaData);
		// Can this viewpoint be mapped to another view location - can only
		// get a valid answer to this after we start reading so this code has
		// to go here...
		_hasAVCap = _generator->IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT);
	}
	_lock.unlock();

}

//------------------ Buffer Data ------------------------------
void OpenNIReader::setBufferData(int buffer, BufferData *pData){
	checkthreadOpenNI();
	delete _bufferData[buffer];
	_bufferData[buffer] = pData;
}

//------------------ Meta Data ------------------------------
void OpenNIReader::setMetaData(xn::MapMetaData *pData){
	checkthreadOpenNI();
	_metaData = pData;
}

//------------------ Generators ------------------------------
xn::MapGenerator *OpenNIReader::getGenerator(){
	checkthreadOpenNI();
	return(_generator);
}

xn::MapGenerator *OpenNIReader::getFirstGenerator() {
	checkthreadOpenNI();
	static xn::MapGenerator *firstGenerator(0);
	if (firstGenerator == 0) {
		firstGenerator = _generator;
	}
	return(firstGenerator);
}

void OpenNIReader::setGenerator(xn::MapGenerator *pData) {
	checkthreadOpenNI();
	_generator = pData;
	// Note the following work for depth cameras only:
	// get the focal length in mm (ZPD = zero plane distance)
	_generator->GetIntProperty("ZPD", _focalLength);
	// get the pixel size in mm ("ZPPS" = pixel size at zero plane)
	_generator->GetRealProperty ("ZPPS", _pixelSize);
}

int OpenNIReader::getMode(){
	checkthreadOpenNI();
	return(_mode);
}
