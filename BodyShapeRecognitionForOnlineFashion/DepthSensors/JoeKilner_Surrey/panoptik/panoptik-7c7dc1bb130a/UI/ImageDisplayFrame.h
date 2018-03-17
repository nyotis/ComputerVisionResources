/*
 *   ImageDisplayWindow.h
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

#ifndef IMAGEDISPLAYWINDOW_H_
#define IMAGEDISPLAYWINDOW_H_

#include "wx/wx.h"

#include "ImageReader.h"
#include "DepthReader.h"
#include "IRReader.h"
#include "Calibrator.h"
#include "ImageWindow.h"
#include "CaptureUI.h"

class DeviceMotor;

class ImageDisplayFrame  : public wxFrame, public CaptureUI
{
public:
	ImageDisplayFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
	        const wxSize& size, bool master, std::string postFix, long style = wxDEFAULT_FRAME_STYLE);
	virtual ~ImageDisplayFrame();

    void setSource(OpenNIReader *reader);
    void setSource(Calibrator *calibrator);
    void onIdle(wxIdleEvent& event);

    void setId(int val){_id = val;}

protected:
    void saveImpl(std::string name);

private:
    ImageWindow *_pWindow;
    OpenNIReader *_pReader;
    Calibrator *_pCalibrator;
    int _id;

    void onMenuSave(wxCommandEvent &event);
    void onMenuLoad(wxCommandEvent &event);
    void onKeyPress(wxKeyEvent &event);

    DeviceMotor *getMotor();

    DECLARE_EVENT_TABLE()

};

#endif /* IMAGEDISPLAYWINDOW_H_ */
