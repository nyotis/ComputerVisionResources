/*
 *   WXGraphicsWindow.h
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

#ifndef WXGRAPHICSWINDOW_H_
#define WXGRAPHICSWINDOW_H_

#include "OSGCanvas.h"
#include <osgViewer/GraphicsWindow>

class WXGraphicsWindow : public osgViewer::GraphicsWindow{
public:
	WXGraphicsWindow(OSGCanvas *canvas);
    ~WXGraphicsWindow();

    void init();

    //
    // GraphicsWindow interface
    //
    void grabFocus();
    void grabFocusIfPointerInWindow();
    void useCursor(bool cursorOn);

    bool makeCurrentImplementation();
    void swapBuffersImplementation();

    // not implemented yet...just use dummy implementation to get working.
    virtual bool valid() const { return (true); }
    virtual bool realizeImplementation() { return (true); }
    virtual bool isRealizedImplementation() const  { return (true); }
    virtual void closeImplementation() {}
    virtual bool releaseContextImplementation() { return (true); }

private:
    // XXX need to set _canvas to NULL when the canvas is deleted by
    // its parent. for this, need to add event handler in OSGCanvas
    OSGCanvas*  _canvas;
};

#endif /* WXGRAPHICSWINDOW_H_ */
