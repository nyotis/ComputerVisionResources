/*
 *   ImageWindow.h
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

#ifndef IMAGEWINDOW_H_
#define IMAGEWINDOW_H_

#include "wx/wx.h"
#include "opencv2/core/core.hpp"
#include "OpenNIReader.h"

class ImageWindow : public wxWindow{
public:
	ImageWindow( wxWindow *frame, const wxPoint& pos, const wxSize& size );
	virtual ~ImageWindow();

	void checkUpdate( );

	// Draw method
	void drawOpenNIImage( BufferData *pBuffer );
	void drawOpenCVImage( cv::Mat img );
	void draw( wxDC& dc );

	wxBitmap getImage(){return (_bitmap);};

// Protected data
private:
	wxBitmap	_bitmap;

	bool	_bDrawing;
	bool	_bNewImage;

	int _width;
	int _height;

	void	onPaint(wxPaintEvent& event);
	void	onSize( wxSizeEvent& even );

	DECLARE_EVENT_TABLE()
};

#endif /* IMAGEWINDOW_H_ */
