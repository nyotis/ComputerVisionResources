/*
 *   OpenNIReader.h
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

#ifndef OPENNIREADER_H_
#define OPENNIREADER_H_

#include "OpenNI.h"
#include <OpenThreads/Thread>
#include <vector>
#include <string>

#include "OpenNIReaderThread.h"

class BufferData
{
public:
	BufferData(xn::MapMetaData *pData):
		_width(pData->XRes()),
		_height(pData->YRes())
	{};
	virtual ~BufferData(){};
	virtual void setData(xn::MapMetaData *) = 0;
	virtual void putData(unsigned char *pDest, bool rgb) = 0;
	int getWidth() {return (_width);}
	int getHeight() {return (_height);}
protected:
	int _width;
	int _height;
};

class NullBufferData: public BufferData
{
public:
	NullBufferData(): BufferData(new xn::DepthMetaData) {}
	virtual void setData(xn::MapMetaData *) {}
	virtual void putData(unsigned char *pDest, bool rgb) {}
};

class OpenNIReader {

public:
	// Initialise OpenNI and return number of devices
	static int init();
	static void wait();
	static void shutdown();

	// pause and start reading threads
	void static pauseAllReading();
	void static resumeAllReading();

	// Get the most recent data
	BufferData *read();

	// Get meta-data about the camera
	XnUInt64 getFocalLength();
	XnDouble getPixelSize();

	void stopReading();
	void startReading();

	friend class OpenNIReaderThread;

	std::string getName() {return(_name);}
	void setName(std::string name) {_name = name;}

	bool viewFrom(OpenNIReader *pReader);

	void moveUp() {tilt += 2;}
	void moveDown() {tilt -= 2;}

	void setTilt(int t) {tilt = t;}
	int tilt;

	int getDeviceID();
protected:

	// OpenNIReaderThread Interface:
	// Called by thread to initialise all OpenNI stuff
	static bool initOpenNI();
	static void waitOpenNI();
	static void shutdownOpenNI();
	// Get the current context
	static xn::Context *getContext();

	// Called by thread to fetch the next lot of data
	void fetch();


	// OpenNIReader Interface for derived classes
	// Construction / destruction
	OpenNIReader(int deviceID, int mode);
	virtual ~OpenNIReader();

	// Pure Virtual function that basically implements class
	virtual void createGenerator() = 0;

	// Functions for setting data
	void setBufferData(int buffer, BufferData *pData);
	void setMetaData(xn::MapMetaData *pData);

	void setGenerator(xn::MapGenerator *pData);
	xn::MapGenerator *getFirstGenerator();
	xn::MapGenerator *getGenerator();


	int getMode();

private:
	// Creation Data
	int _deviceID;
	int _mode;

	// Data about the camera
	XnUInt64	_focalLength;
	XnDouble	_pixelSize;
	bool		_hasAVCap;

	// Metadata
	std::string _name;

	// Flags for pausing or resuming a reader
	bool _stopping;
	bool _starting;

	// Map generators etc.
	xn::MapGenerator *_generator;
	OpenNIReader*_masterView;
	xn::MapMetaData *_metaData;

	// Double buffered data
	bool _swapNextWrite;
	int _bufferIndex;
	BufferData *_bufferData[2];

	// Mutex to lock access to data
	OpenThreads::Mutex _lock;

// STATIC DATA
	static int numDevices;
// END STATIC DATA
};

template <typename TGenerator, typename TMetaData, typename TBufferData, XnProductionNodeType xnType>
class TypedOpenNIReader : public OpenNIReader {

public:
	TypedOpenNIReader(int deviceID, int mode): OpenNIReader(deviceID, mode){}
	virtual ~TypedOpenNIReader(){}
	static bool init() {
		checkthreadOpenNI();
		XnStatus rc = getContext()->EnumerateProductionTrees(xnType,NULL,typed_list,NULL);
		if (rc != XN_STATUS_OK)
		{
			printf("Enumeration failed: %s\n", xnGetStatusString(rc));
			return false;
		}
		return true;
	}

protected:
	virtual void createGenerator() {
		checkthreadOpenNI();

		TGenerator *pGenerator = new TGenerator();

		//printf("Initialising Generator device=%d, mode=%d, type=%d\n", getDeviceID(), getMode(), xnType);
		int count = 0;
		for (xn::NodeInfoList::Iterator i = typed_list.Begin();
				i != typed_list.End();
				++i, count++)
		{
			if (count == getDeviceID())
			{
				xn::NodeInfo ni = *i;
				/*XnProductionNodeDescription desc = ni.GetDescription();
				printf("strVendor = %s, strName = %s, creationInfo = %s, name = %s\n",
						desc.strVendor, desc.strName, ni.GetCreationInfo(),
						ni.GetInstanceName());*/

				getContext()->CreateProductionTree(ni, *pGenerator);
				unsigned mode_count = pGenerator->GetSupportedMapOutputModesCount ();

				XnMapOutputMode* modes = new XnMapOutputMode[mode_count];
				pGenerator->GetSupportedMapOutputModes (modes, mode_count);
				/*for (unsigned x = 0 ; x < mode_count; ++x) {
					printf("mode %d, FPS = %d, XRes = %d, YRes = %d\n",
					x,modes[x].nFPS, modes[x].nXRes, modes[x].nYRes);
				}*/

				pGenerator->SetMapOutputMode (modes[getMode()]);
				if (getFirstGenerator() != 0) {
					pGenerator->GetFrameSyncCap().FrameSyncWith(*getFirstGenerator());
				}
			}
		}

		setGenerator(pGenerator);
		TMetaData *pMetaData = new TMetaData;
		pGenerator->GetMetaData(*pMetaData);
		setBufferData(0, new TBufferData(pMetaData));
		setBufferData(1, new TBufferData(pMetaData));
		setMetaData(pMetaData);
	}

private:
	// List of all attached nodes
	static xn::NodeInfoList typed_list;
};
template <typename TGenerator, typename TMetaData, typename TBufferData, XnProductionNodeType xnType>
xn::NodeInfoList TypedOpenNIReader<TGenerator, TMetaData, TBufferData, xnType>::typed_list;

#endif /* OPENNIREADER_H_ */
