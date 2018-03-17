/*
 *   OSGCanvas.h
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

#ifndef OSGCANVAS_H_
#define OSGCANVAS_H_

#include <osgViewer/GraphicsWindow>
#include "wx/glcanvas.h"

class OSGCanvas : public wxGLCanvas {
public:
    OSGCanvas(wxWindow *parent, wxWindowID id = wxID_ANY,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxString& name = wxT("TestGLCanvas"),
        int *attributes = 0);

    virtual ~OSGCanvas();

    void setGraphicsWindow(osgViewer::GraphicsWindow *gw)   { _graphics_window = gw; }

    void onPaint(wxPaintEvent& event);
    void onSize(wxSizeEvent& event);
    void onEraseBackground(wxEraseEvent& event);

    void onChar(wxKeyEvent &event);
    void onKeyUp(wxKeyEvent &event);

    void onMouseEnter(wxMouseEvent &event);
    void onMouseDown(wxMouseEvent &event);
    void onMouseUp(wxMouseEvent &event);
    void onMouseMotion(wxMouseEvent &event);
    void onMouseWheel(wxMouseEvent &event);

    void useCursor(bool value);

private:
    DECLARE_EVENT_TABLE()

    osg::ref_ptr<osgViewer::GraphicsWindow> _graphics_window;

    wxCursor _oldCursor;
};

#endif /* OSGCANVAS_H_ */
