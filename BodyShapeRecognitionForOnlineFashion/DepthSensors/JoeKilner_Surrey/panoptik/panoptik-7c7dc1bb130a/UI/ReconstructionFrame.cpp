/*
 *   ReconstructionFrame.cpp
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

#include "ReconstructionFrame.h"

#include "OSGCanvas.h"
#include "WXGraphicsWindow.h"

#include <osgViewer/ViewerEventHandlers>
#include <osgGA/TrackballManipulator>
#include <osgDB/WriteFile>

using namespace osg;
using namespace std;

BEGIN_EVENT_TABLE(ReconstructionFrame, wxFrame)
    EVT_IDLE(ReconstructionFrame::onIdle)
    EVT_MENU(wxID_SAVE,  ReconstructionFrame::onMenuSave)
END_EVENT_TABLE()

/* My frame constructor */
ReconstructionFrame::ReconstructionFrame(wxFrame *frame, const wxString& title, const wxPoint& pos,
    const wxSize& size, bool master, string postFix, long style)
    : wxFrame(frame, wxID_ANY, title, pos, size, style),
      CaptureUI(master, postFix)
{
    // create osg canvas
    //    - initialize

    int *attributes = new int[7];
    attributes[0] = int(WX_GL_DOUBLEBUFFER);
    attributes[1] = WX_GL_RGBA;
    attributes[2] = WX_GL_DEPTH_SIZE;
    attributes[3] = 8;
    attributes[4] = WX_GL_STENCIL_SIZE;
    attributes[5] = 8;
    attributes[6] = 0;

    OSGCanvas *canvas = new OSGCanvas(this, wxID_ANY, wxDefaultPosition,
    		size, wxSUNKEN_BORDER, wxT("osgviewerWX"), attributes);

    WXGraphicsWindow* gw = new WXGraphicsWindow(canvas);
    canvas->setGraphicsWindow(gw);

    _viewer = new osgViewer::Viewer;
    _viewer->getCamera()->setGraphicsContext(gw);
    _viewer->getCamera()->setViewport(0,0,size.x,size.y);
    _viewer->addEventHandler(new osgViewer::StatsHandler);
    _viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
    _viewer->setCameraManipulator(new osgGA::TrackballManipulator);

    wxMenuBar *pMenuBar = new wxMenuBar;
    wxMenu *pFileMenu = new wxMenu;
    pFileMenu->Append(wxID_SAVE, wxT("&Save"));
    pMenuBar->Append(pFileMenu, wxT("&File"));
	SetMenuBar(pMenuBar);
}

void ReconstructionFrame::onIdle(wxIdleEvent &event)
{
    _viewer->frame();
    event.RequestMore();
}

void ReconstructionFrame::saveImpl(std::string name)
{
	osg::Node *data(dynamic_cast<Node*>(_viewer->getSceneData()->clone(CopyOp::DEEP_COPY_ALL)));
	osgDB::writeNodeFile(*data, name);
	data->unref();
}

void ReconstructionFrame::onMenuSave(wxCommandEvent &event)
{
	string filename(wxSaveFileSelector(wxT("Save File"), wxT(".obj"), wxT("panoptik_data.obj"), this).char_str());
	save(filename);
}
