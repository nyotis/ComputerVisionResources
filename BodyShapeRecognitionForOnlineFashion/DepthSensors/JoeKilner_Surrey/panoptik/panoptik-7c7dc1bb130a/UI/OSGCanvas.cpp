/*
 *   OSGCanvas.cpp
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

#include "OSGCanvas.h"

#include <wx/wx.h>
//#include <wx/image.h>
//#include <wx/menu.h>

BEGIN_EVENT_TABLE(OSGCanvas, wxGLCanvas)
    EVT_SIZE                (OSGCanvas::onSize)
    EVT_PAINT               (OSGCanvas::onPaint)
    EVT_ERASE_BACKGROUND    (OSGCanvas::onEraseBackground)

    EVT_CHAR                (OSGCanvas::onChar)
    EVT_KEY_UP              (OSGCanvas::onKeyUp)

    EVT_ENTER_WINDOW        (OSGCanvas::onMouseEnter)
    EVT_LEFT_DOWN           (OSGCanvas::onMouseDown)
    EVT_MIDDLE_DOWN         (OSGCanvas::onMouseDown)
    EVT_RIGHT_DOWN          (OSGCanvas::onMouseDown)
    EVT_LEFT_UP             (OSGCanvas::onMouseUp)
    EVT_MIDDLE_UP           (OSGCanvas::onMouseUp)
    EVT_RIGHT_UP            (OSGCanvas::onMouseUp)
    EVT_MOTION              (OSGCanvas::onMouseMotion)
    EVT_MOUSEWHEEL          (OSGCanvas::onMouseWheel)
END_EVENT_TABLE()

OSGCanvas::OSGCanvas(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name, int *attributes)
    : wxGLCanvas(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE, name, attributes)
{
    // default cursor to standard
    _oldCursor = *wxSTANDARD_CURSOR;
}

OSGCanvas::~OSGCanvas()
{
}

void OSGCanvas::onPaint( wxPaintEvent& WXUNUSED(event) )
{
    /* must always be here */
    wxPaintDC dc(this);
}

void OSGCanvas::onSize(wxSizeEvent& event)
{
    // this is also necessary to update the context on some platforms
    wxGLCanvas::OnSize(event);

    // set GL viewport (not called by wxGLCanvas::OnSize on all platforms...)
    int width, height;
    GetClientSize(&width, &height);

    if (_graphics_window.valid())
    {
        // update the window dimensions, in case the window has been resized.
        _graphics_window->getEventQueue()->windowResize(0, 0, width, height);
        _graphics_window->resized(0,0,width,height);
    }
}

void OSGCanvas::onEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    /* Do nothing, to avoid flashing on MSW */
}

void OSGCanvas::onChar(wxKeyEvent &event)
{
#if wxUSE_UNICODE
    int key = event.GetUnicodeKey();
#else
    int key = event.GetKeyCode();
#endif

    if (_graphics_window.valid())
        _graphics_window->getEventQueue()->keyPress(key);

    // If this key event is not processed here, we should call
    // event.Skip() to allow processing to continue.
}

void OSGCanvas::onKeyUp(wxKeyEvent &event)
{
#if wxUSE_UNICODE
    int key = event.GetUnicodeKey();
#else
    int key = event.GetKeyCode();
#endif

    if (_graphics_window.valid())
        _graphics_window->getEventQueue()->keyRelease(key);

    // If this key event is not processed here, we should call
    // event.Skip() to allow processing to continue.
}

void OSGCanvas::onMouseEnter(wxMouseEvent &event)
{
    // Set focus to ourselves, so keyboard events get directed to us
    SetFocus();
}

void OSGCanvas::onMouseDown(wxMouseEvent &event)
{
    if (_graphics_window.valid())
    {
        _graphics_window->getEventQueue()->mouseButtonPress(event.GetX(), event.GetY(),
            event.GetButton());
    }
}

void OSGCanvas::onMouseUp(wxMouseEvent &event)
{
    if (_graphics_window.valid())
    {
        _graphics_window->getEventQueue()->mouseButtonRelease(event.GetX(), event.GetY(),
            event.GetButton());
    }
}

void OSGCanvas::onMouseMotion(wxMouseEvent &event)
{
    if (_graphics_window.valid())
        _graphics_window->getEventQueue()->mouseMotion(event.GetX(), event.GetY());
}

void OSGCanvas::onMouseWheel(wxMouseEvent &event)
{
    int delta = event.GetWheelRotation() / event.GetWheelDelta() * event.GetLinesPerAction();

    if (_graphics_window.valid()) {
        _graphics_window->getEventQueue()->mouseScroll(
            delta>0 ?
            osgGA::GUIEventAdapter::SCROLL_UP :
            osgGA::GUIEventAdapter::SCROLL_DOWN);
    }
}

void OSGCanvas::useCursor(bool value)
{
    if (value)
    {
        // show the old cursor
        SetCursor(_oldCursor);
    }
    else
    {
        // remember the old cursor
        _oldCursor = GetCursor();

        // hide the cursor
        //    - can't find a way to do this neatly, so create a 1x1, transparent image
        wxImage image(1,1);
        image.SetMask(true);
        image.SetMaskColour(0, 0, 0);
        wxCursor cursor(image);
        SetCursor(cursor);

        // On wxGTK, only works as of version 2.7.0
        // (http://trac.wxwidgets.org/ticket/2946)
        // SetCursor( wxStockCursor( wxCURSOR_BLANK ) );
    }
}
