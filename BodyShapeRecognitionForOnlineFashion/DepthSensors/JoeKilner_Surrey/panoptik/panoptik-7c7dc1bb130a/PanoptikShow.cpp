/*
 *   PanoptikShow.cpp
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

#include "PanoptikShow.h"
#include "Motor.h"

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

PanoptikShow::PanoptikShow()
{}

// Standard includes
#include <stdio.h>
#include <time.h>


// `Main program' equivalent, creating windows and returning main app frame
bool PanoptikShow::OnInit()
{
	// The parameters for image size and what format to use for device
    int width = 640;
    int height = 480;
    int data_format = 2;

	// Initialise the wx Image handlers
	wxInitAllImageHandlers();

	// Initialise OpenNI
    int num_devices = OpenNIReader::init();
    if (num_devices == 0)
    {
    	wxMessageBox(wxT("Error - Can not connect to depth sensors."
    			         "Check sensors are powered on correctly and "
    					 "no other instances of the software are running, then "
    			         "wait a few moments then try again. You can also "
    			         "try reconnecting the devices."),
    			     wxT("Device initialisation failed"),
    			     wxICON_ERROR);
    	return(false);
    }

    // Allow all LEDs to stop blinking
	sleep(2);

	// Blink all LEDs yellow in order
	for(int i = 0 ; i < num_devices; ++i) {
		DeviceMotor::getMotor(i)->setLight(DeviceMotor::LED_RED);
		sleep(2);
		DeviceMotor::getMotor(i)->setLight(DeviceMotor::LED_GREEN);
	}

    // Create the readers for reading data from the image sensors
    for(int i = 0 ; i < num_devices; ++i) {
		_image[i]  = auto_ptr<ImageReader>(new ImageReader(i,data_format));
    }

    // Start the system and pause it. This finishes initialisation for all
    // devices but does not start data pumping through the system
	OpenNIReader::resumeAllReading();
	OpenNIReader::wait();

    // Create display and processing objects

    // Create the display and processing objects
    for(int i = 0 ; i < num_devices; ++i) {
        ImageDisplayFrame *pCapture = new ImageDisplayFrame(NULL,
				wxString(wxT("Image Stream ")) + char('0'+i),
				wxDefaultPosition, wxSize(width, height), false,
				string("_img_") + char('0'+i) + ".png");
        pCapture->setSource( _image[i].get());
        pCapture->Show(true);
    }

	return (true);
}

int PanoptikShow::OnExit()
{
	cout << "Shutting Down..." << endl;
	OpenNIReader::shutdown();
    return (wxApp::OnExit());
}

IMPLEMENT_APP(PanoptikShow)


