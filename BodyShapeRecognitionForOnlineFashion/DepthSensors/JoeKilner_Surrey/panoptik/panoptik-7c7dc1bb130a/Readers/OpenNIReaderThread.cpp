/*
 *   OpenNIReaderThread.cpp
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

#include "OpenNIReaderThread.h"
#include "OpenNIReader.h"
#include <algorithm>

using namespace std;

OpenNIReaderThread *OpenNIReaderThread::get() {
	// CAN BE CALLED ON ANY THREAD

	// Thread that gets the data
	static OpenNIReaderThread *pThread(0);
	if (!pThread) {
		pThread = new OpenNIReaderThread();
	}

	return(pThread);
}

void OpenNIReaderThread::shutdown() {
	// SHOULD BE CALLED ONLY FROM UI THREAD
	checkthreadOther();
	get()->cancel();
	delete get();
}

// Start off running and paused
OpenNIReaderThread::OpenNIReaderThread() :
		_running(true),
		_paused(true),
		_frameWaiter(2)
{
	// CAN BE CALLED FROM ANY THREAD
	// (private constructor as this is a singleton)
	setSchedulePriority(THREAD_PRIORITY_MAX);
}

void OpenNIReaderThread::run()
{
	checkthreadOpenNI();
	// ONLY CALLED FROM THIS THREAD

	// OpenNI Initialisation
	if (OpenNIReader::initOpenNI() ) {
		// Main Loop
		while(_running)
		{
			if (!_paused)
			{
				_readersLock.lock();
				for(vector<OpenNIReader *>::iterator i = _readers.begin();
						i != _readers.end();
						++i)
				{
					(*i)->fetch();
				}
				_readersLock.unlock();
				OpenNIReader::waitOpenNI();
			}
			else
			{
				microSleep(500000);
			}
			// Release any threads that are waiting for a frame to end
			_frameWaiter.completed();
		}
	}
	// Release any threads that are waiting for a frame to end
	_frameWaiter.release();
	OpenNIReader::shutdownOpenNI();
}

// Ensure one read cycle has happened
void OpenNIReaderThread::wait() {
	checkthreadOther();
	// SHOULD BE CALLED ONLY FROM UI THREAD
	// Resets the frame waiter count to 2
	_frameWaiter.reset();
	// Waits until the count has been cleared
	_frameWaiter.block();
}

int OpenNIReaderThread::cancel()
{
	checkthreadOther();
	// SHOULD BE CALLED ONLY FROM UI THREAD
	_running.exchange(false);
	microSleep(4000000);
	if (isRunning()) {
		return(Thread::cancel());
	}
	return(0);
}

// Add a reader to read from
void OpenNIReaderThread::addReader(OpenNIReader *pReader)
{
	checkthreadOther();
	// SHOULD BE CALLED ONLY FROM UI THREAD
	_readersLock.lock();
	if (find(_readers.begin(), _readers.end(),pReader) == _readers.end()) {
		_readers.push_back(pReader);
	}
	_readersLock.unlock();
}

void OpenNIReaderThread::removeReader(OpenNIReader *pReader)
{
	checkthreadOther();
	// SHOULD BE CALLED ONLY FROM UI THREAD
	_readersLock.lock();
	std::vector<OpenNIReader *>::iterator loc = find(_readers.begin(), _readers.end(),pReader);
	if (loc != _readers.end()) {
	    _readers.erase(loc);
	}
	_readersLock.unlock();
}

// Start all threads actually running
void OpenNIReaderThread::resumeReading()
{
	checkthreadOther();
	// SHOULD BE CALLED ONLY FROM UI THREAD
	_readersLock.lock();
	_paused = false;
	_readersLock.unlock();
}
// Pause all threads actually running
void OpenNIReaderThread::pauseReading()
{
	checkthreadOther();
	// SHOULD BE CALLED ONLY FROM UI THREAD
	_readersLock.lock();
	_paused = true;
	_readersLock.unlock();
}


/////////////////////////////////////////////////////////////////////////////
//Debug helpers
void checkthread(bool bOpenNI, const char* fname) {
	bool test(OpenThreads::Thread::CurrentThread() == OpenNIReaderThread::get());
	if (test != bOpenNI){
		printf("*** Threading ERROR!! at %s ***\n", fname);
		exit(1);
	}
}
