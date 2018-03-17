/*
 *   PanoptikCal.cpp
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

#include "PanoptikCal.h"
#include "ImageDisplayFrame.h"

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

PanoptikCal::PanoptikCal()
{}

// Standard includes
#include <stdio.h>
#include <time.h>


// `Main program' equivalent, creating windows and returning main app frame
bool PanoptikCal::OnInit()
{
	// The parameters for image size and what format to use for device
    int width = 640;
    int height = 480;
    int data_format = 2;

    if (argc < 4) {
    	wxMessageBox(wxT("Usage:\n PanoptikCal <device_id> <usb_motor_id> [i|r] [a]"),
    			     wxT("Incorrect command line"),
    			     wxICON_ERROR);
    	return(false);

    }


    // Which device to read
    int sensorID(wxAtoi(argv[1]));
    int motorID(wxAtoi(argv[2]));
    bool reading_ir = wxStrcmp(argv[3],wxT("i"))==0;
    bool show_all = false;
    if (argc > 4 && (wxStrcmp(argv[4],wxT("a"))==0)) {
    	show_all = true;
    }

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

	_depth  = auto_ptr<DepthReader>(new DepthReader(sensorID,data_format));

	OpenNIReader *pReader;
	if (reading_ir) {
		_ir     = auto_ptr<IRReader>(new IRReader(sensorID,data_format));
		pReader = _ir.get();
	}
	else {
		_image  = auto_ptr<ImageReader>(new ImageReader(sensorID,data_format));
		pReader = _image.get();
	}

	OpenNIReader::resumeAllReading();
	OpenNIReader::wait();

	_cal = auto_ptr<Calibrator>(new Calibrator( pReader ));

    // Create display and processing objects
	char num('0'+ sensorID);


	ImageDisplayFrame *pCalibration = new ImageDisplayFrame(NULL, wxString(wxT("Calibration Stream ")) + num,
		wxDefaultPosition, wxSize(width, height), false, string("_cal_") + num + ".cal");
	pCalibration->setSource(_cal.get());
	pCalibration->Show(true);
	pCalibration->setId(motorID);

	if (show_all) {
		ImageDisplayFrame *pCapture = new ImageDisplayFrame(pCalibration,
				wxString(wxT("Image Stream ")) + num,
				wxDefaultPosition, wxSize(width, height), false,
				string("_img_") + num + ".png");
		pCapture->setSource( pReader);
		pCapture->Show(true);
		pCapture->setId(motorID);

		ImageDisplayFrame *pDepthCapture = new ImageDisplayFrame(pCalibration, wxString(wxT("Depth Stream ")) + num,
			wxDefaultPosition, wxSize(width, height), false, string("_depth_") + num + ".png");
		pDepthCapture->setSource(_depth.get());
		pDepthCapture->Show(true);
		pDepthCapture->setId(motorID);
	}

	return (true);
}

int PanoptikCal::OnExit()
{
	cout << "Shutting Down..." << endl;
	OpenNIReader::shutdown();
    return (wxApp::OnExit());
}

IMPLEMENT_APP(PanoptikCal)


