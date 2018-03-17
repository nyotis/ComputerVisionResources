/*
 *   Panoptik.cpp
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

#include "Panoptik.h"
#include "ReconstructionFrame.h"
#include "ControlUI.h"
#include "Calibrator.h"

#include <iostream>
#include <string>
#include <sstream>
#include <zmq.h>

#include <osgDB/WriteFile>

using namespace osg;
using namespace std;

Panoptik::Panoptik():
	_num_devices(0),
	_context(1)
{}

// `Main program' equivalent, creating windows and returning main app frame
bool Panoptik::OnInit()
{
	// Create the Control UI
    ControlUI* control_frame = new ControlUI(NULL, wxID_ANY, wxEmptyString);
    SetTopWindow(control_frame);

	// The parameters for image size and what format to use for sensor
    int width = 640;
    int height = 480;
    int data_format = 2;

    // Which Device to read "-1" means all of them
    int val = -1;
    if (argc > 1) {
    	val = wxAtoi(argv[1]);
    }

	// Initialise the wx Image handlers
	wxInitAllImageHandlers();


	// Get the number of devices on the system
    _num_devices = OpenNIReader::init();
    if (_num_devices == 0)
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

    // Create the readers for reading data from the image and depth sensors
    for(int i = 0 ; i < _num_devices; ++i) {
    	// If we are only looking at one device, skip the others
    	if(val != -1 && val != i) {
    		continue;
    	}

		_image[i]  = auto_ptr<ImageReader>(new ImageReader(i,data_format));
        _depth[i]  = auto_ptr<DepthReader>(new DepthReader(i,data_format));
    }

    // Start the system and pause it. This finishes initialisation for all
    // devices but does not start data pumping through the system
	OpenNIReader::resumeAllReading();
	OpenNIReader::wait();

	// Create the openscenegraph group we are going to use for displaying the
	// scene
    _group = new osg::Group();

    // Create the display and processing objects
    for(int i = 0 ; i < _num_devices; ++i) {
    	// If we are only looking at one device, skip the others
    	if(val != -1 && val != i) {
    		continue;
    	}

    	// Create a mesh builder from the depth map
        _builder[i] = auto_ptr<MeshBuilder>(new MeshBuilder(_depth[i].get(),
        		string("Captured_Mesh_")+char('0'+i)));
        _group->addChild(_builder[i]->getMesh());
    }

    ReconstructionFrame *pReconstruction = new ReconstructionFrame(control_frame, wxT("Reconstruction"),
            wxDefaultPosition, wxSize(width, height), true, "");
    pReconstruction->getViewer()->setSceneData(_group);
    pReconstruction->Show(true);

    control_frame->setApp(this);
    control_frame->Show();

	return (wxApp::OnInit());
}

int Panoptik::OnExit()
{
	cout << "Shutting Down..." << endl;
    for(int i = 0 ; i < _num_devices; ++i) {
    	_builder[i]->shutdown();
    }
	// Shutdown capture system
	OpenNIReader::shutdown();
    return (wxApp::OnExit());
}

void Panoptik::stop() {
	// Signal all running devices to stop data capture
	for (int i = 0; i < _num_devices; ++i ) {
		if (_depth[i].get() != 0) {
			_depth[i]->stopReading();
		}
	}
	OpenNIReader::wait();
}

void Panoptik::start() {
	// Signal all running devices to start data capture
	for (int i = 0; i < _num_devices; ++i ) {
		if (_depth[i].get() != 0) {
			_depth[i]->startReading();
		}
	}
	OpenNIReader::wait();
}

void Panoptik::save(std::string filename, bool saveSequentially) {
	// Simultaneous save operation
	if (!saveSequentially) {
		for (int i = 0; i < _num_devices; ++i ) {
			_builder[i]->pause();
		}
		osg::Node *data(dynamic_cast<Node*>(_group->clone(CopyOp::DEEP_COPY_ALL)));
		for (int i = 0; i < _num_devices; ++i ) {
			_builder[i]->resume();
		}
		osgDB::writeNodeFile(*data, filename);
		data->unref();
	}

	// Sequential save operations meshes and images for sequential save
	// images only for simultaneous save
	for (int i = 0; i < _num_devices; ++i ) {
		// If we are saving meshes sequentially do that.
		if (saveSequentially && _builder[i].get() != 0) {
			// Disable unused devices
			for (int j = 0; j < _num_devices; ++j ) {
				if (_depth[j].get() != 0) {
					if (j != i) {
						_depth[j]->stopReading();
					}
					else {
						_depth[j]->startReading();
					}
				}
			}
			// Wait to allow devices to stop capturing
			OpenNIReader::wait();

			// Reset builder to remove old data so desired mesh is only one
			// there, then force update to get new data
			_builder[i]->reset();
			_builder[i]->wait();
			_builder[i]->updateMesh();

			// Write data out to file
			stringstream save_name;
			save_name << filename << _depth[i]->getName() << ".obj";
			_builder[i]->pause();
			osg::Node *data(dynamic_cast<Node*>(_builder[i]->getMesh()->clone(CopyOp::DEEP_COPY_ALL)));
			_builder[i]->resume();
			osgDB::writeNodeFile(*data, save_name.str());
			data->unref();
		}

		if (_image[i].get() != 0) {
			stringstream save_name;
			if (saveSequentially) {
				save_name << filename << _depth[i]->getName() << ".png";
			}
			else {
				save_name << filename << "_img_" << _depth[i]->getName() << ".png";
			}
			ImageBufferData *pBuffer = dynamic_cast<ImageBufferData *>(_image[i]->read());
			wxImage pWxImg = wxImage( pBuffer->getWidth(), pBuffer->getHeight());
			pBuffer->putData(pWxImg.GetData(),true);
			pWxImg.SaveFile(wxString(save_name.str().c_str(),wxConvUTF8), wxBITMAP_TYPE_PNG);
		}
	}
}


void Panoptik::setName(int id, std::string name)
{
	if (_depth[id].get() != 0) {
		_depth[id]->setName(name);
	}
	if (_image[id].get() != 0) {
		_image[id]->setName(name);
	}
}

void Panoptik::enableDevice(int id, bool enabled)
{
	if (enabled) {
		if (_depth[id].get() != 0) {
			_depth[id]->startReading();
		}
		if (_image[id].get() != 0) {
			_image[id]->startReading();
		}
		_group->addChild(_builder[id]->getMesh());
	}
	else {
		if (_depth[id].get() != 0) {
			_depth[id]->stopReading();
		}
		if (_image[id].get() != 0) {
			_image[id]->stopReading();
		}
		_group->removeChild(_builder[id]->getMesh());
	}
}

void Panoptik::loadDepthCalibration(std::string filename) {

	for (int i = 0; i < _num_devices; ++i ) {
		stringstream load_name;
		load_name << filename << _depth[i]->getName() << ".cal";
		if (wxFileExists(wxString(load_name.str().c_str(),wxConvUTF8))) {
			_builder[i]->setCalibration(CameraCalibration(load_name.str()));
		}
		else{
			_builder[i]->setCalibration(CameraCalibration());
		}

		load_name.str("");
		load_name << filename << _depth[i]->getName() << "_rgb.cal";
		if (wxFileExists(wxString(load_name.str().c_str(),wxConvUTF8))) {
			_builder[i]->setImageCalibration(CameraCalibration(load_name.str()));
			_builder[i]->setImage(_image[i].get());
		}
		else{
			_builder[i]->setImageCalibration(CameraCalibration());
			_builder[i]->setImage(0);
		}

		load_name.str("");
		load_name << filename << _depth[i]->getName() << "_ir.cal";
		if (wxFileExists(wxString(load_name.str().c_str(),wxConvUTF8))) {
			_builder[i]->setIRCalibration(CameraCalibration(load_name.str()));
		}
		else{
			_builder[i]->setIRCalibration(CameraCalibration());
		}
	}
}

IMPLEMENT_APP(Panoptik)


