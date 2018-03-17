/*
 *   ReconstructionFrame.h
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

#ifndef RECONSTRUCTIONFRAME_H_
#define RECONSTRUCTIONFRAME_H_

#include "wx/wx.h"
#include <osgViewer/Viewer>

#include "CaptureUI.h"

class ReconstructionFrame : public wxFrame, public CaptureUI
{
public:
	ReconstructionFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
        const wxSize& size,  bool master, std::string postFix, long style = wxDEFAULT_FRAME_STYLE);

    void onIdle(wxIdleEvent& event);

    osgViewer::Viewer *getViewer(){return (_viewer);}

protected:
    void saveImpl(std::string name);

private:
    osg::ref_ptr<osgViewer::Viewer> _viewer;

    void onMenuSave(wxCommandEvent &event);

    DECLARE_EVENT_TABLE()
};

#endif /* RECONSTRUCTIONFRAME_H_ */
