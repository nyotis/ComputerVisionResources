/*
 *   OpenNIReaderThread.h
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

#ifndef OPENNIREADERTHREAD_H_
#define OPENNIREADERTHREAD_H_

#include <OpenThreads/Thread>
#include <OpenThreads/Atomic>
#include <OpenThreads/Block>
#include <vector>

class OpenNIReader;

class OpenNIReaderThread  : public OpenThreads::Thread
	{
	public :
		// Static access function for singleton thread
		static OpenNIReaderThread *get();
		// Static access function for singleton thread
		static void shutdown();

		// Override. Run the thread
		virtual void run();
		// Override. Stop the thread
		virtual int cancel();
		// Add a reader to read from
		void addReader(OpenNIReader *);
		// Remove a reader from the list
		void removeReader(OpenNIReader *);
		// Start all threads actually running
		void resumeReading();
		// Pause all threads actually running
		void pauseReading();
		// Ensure one read cycle has happened
		void wait();
	private:
		// Initialise with a depth reader to write to
		OpenNIReaderThread();
		// Are we running
		OpenThreads::Atomic _running;
		//bool _running;
		// Are we running
		bool _paused;
		// Mutex to lock access to readersdata
		OpenThreads::Mutex _readersLock;
		// Mutex to lock waiting
		OpenThreads::BlockCount _frameWaiter;
		// The readers to read from
		std::vector<OpenNIReader *> _readers;
	};

#define checkthreadOpenNI() checkthread(true, __func__)
#define checkthreadOther() checkthread(false, __func__)
void checkthread(bool bOpenNI, const char* fname);

#endif /* OPENNIREADERTHREAD_H_ */
