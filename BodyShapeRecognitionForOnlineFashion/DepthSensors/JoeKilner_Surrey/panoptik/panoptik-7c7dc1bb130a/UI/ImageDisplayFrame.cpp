/*
 *   ImageDisplayWindow.cpp
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

#include "ImageDisplayFrame.h"
#include "Motor.h"

#include <iostream>

using namespace xn;
using namespace std;

BEGIN_EVENT_TABLE(ImageDisplayFrame, wxFrame)
    EVT_IDLE(ImageDisplayFrame::onIdle)
    EVT_MENU(wxID_SAVE,  ImageDisplayFrame::onMenuSave)
    EVT_MENU(wxID_OPEN,  ImageDisplayFrame::onMenuLoad)
    EVT_KEY_DOWN(ImageDisplayFrame::onKeyPress)
END_EVENT_TABLE()

ImageDisplayFrame::ImageDisplayFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
	    const wxSize& size, bool master, std::string postFix, long style)
	    : wxFrame(frame, wxID_ANY, title, pos, size, style) ,
	      CaptureUI(master, postFix),
	      _pReader(0),
	      _pCalibrator(0),
	      _id(0){

	_pWindow = new ImageWindow(this,wxDefaultPosition,size);

	_pWindow->Connect(wxID_ANY,
            wxEVT_KEY_DOWN,
            wxKeyEventHandler(ImageDisplayFrame::onKeyPress),
            (wxObject*) NULL,
            this);

    wxMenuBar *pMenuBar = new wxMenuBar;
    wxMenu *pFileMenu = new wxMenu;
    pFileMenu->Append(wxID_OPEN, wxT("&Load"));
    pFileMenu->Append(wxID_SAVE, wxT("&Save"));
    pMenuBar->Append(pFileMenu, wxT("&File"));
	SetMenuBar(pMenuBar);
}

ImageDisplayFrame::~ImageDisplayFrame() {
	// TODO Auto-generated destructor stub
}

void ImageDisplayFrame::setSource(OpenNIReader *pReader)
{
	_pReader = pReader;
	_pCalibrator = NULL;
}

void ImageDisplayFrame::setSource(Calibrator *calibrator)
{
	_pReader = NULL;
	_pCalibrator = calibrator;
}

void ImageDisplayFrame::onIdle(wxIdleEvent &event)
{
	if (_pReader){
		_pWindow->drawOpenNIImage(static_cast<ImageBufferData *>(_pReader->read()));
	}
	if (_pCalibrator){
		_pWindow->drawOpenCVImage(_pCalibrator->getImage(true));
	}
	Refresh(FALSE);
	Update();
    event.RequestMore();
}

void ImageDisplayFrame::onMenuSave(wxCommandEvent &event)
{
	if (_pReader)
	{
		wxString filename(wxSaveFileSelector(wxT("Image"), wxT(".png"), wxT("capture.png"), this));
		save(string(filename.char_str()));

	}
	if (_pCalibrator)
	{
		string filename(wxSaveFileSelector(wxT("Calibration"), wxT(".cal"), wxT("cal_.cal"), this).char_str());
		save(filename);
	}
}

void ImageDisplayFrame::saveImpl(string name)
{
	if (_pReader)
	{
		wxBitmap image = _pWindow->getImage();
		image.SaveFile(wxString(name.c_str(),wxConvUTF8), wxBITMAP_TYPE_PNG);
	}
	if (_pCalibrator)
	{
		_pCalibrator->getCalibration().save(name);
	}
}

void ImageDisplayFrame::onMenuLoad(wxCommandEvent &event)
{
	if (_pCalibrator)
	{
		string filename(wxLoadFileSelector(wxT("Calibration"), wxT(".cal"), wxT("cal_.cal"), this).char_str());
		_pCalibrator->setCalibration(CameraCalibration(filename));
	}
}

void ImageDisplayFrame::onKeyPress(wxKeyEvent &event)
{
	if (_pCalibrator)
	{
		if (event.GetKeyCode() == 'I') {
			_pCalibrator->updateIntrinsics();
		}
		if (event.GetKeyCode() == 'E') {
			_pCalibrator->updateExtrinsics();
		}
		if (event.GetKeyCode() == 'C') {
			_pCalibrator->clearCalibration();
		}
	}
	if (_id != -1) {
		if (event.GetKeyCode() == 'O') {
			getMotor()->moveUp();
		}
		if (event.GetKeyCode() == 'L') {
			getMotor()->moveDown();
		}
	}
}


DeviceMotor *ImageDisplayFrame::getMotor(){
	DeviceMotor *device(0);
	if(_pCalibrator) {
		device = DeviceMotor::getMotor(_id);
	}
	if(_pReader) {
		device = DeviceMotor::getMotor(_id);
	}
	return(device);
}
