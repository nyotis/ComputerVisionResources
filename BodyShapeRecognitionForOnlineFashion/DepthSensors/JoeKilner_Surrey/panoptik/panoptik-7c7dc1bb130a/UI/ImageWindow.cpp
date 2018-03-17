/*
 *   ImageWindow.cpp
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

#include "ImageWindow.h"
#include "opencv2/imgproc/imgproc.hpp"

BEGIN_EVENT_TABLE(ImageWindow, wxWindow)
	EVT_PAINT( ImageWindow::onPaint )
	EVT_SIZE( ImageWindow::onSize )
END_EVENT_TABLE()

ImageWindow::ImageWindow(wxWindow *frame, const wxPoint& pos, const wxSize& size ):
     wxWindow(frame, -1, pos, size, wxSIMPLE_BORDER ),
 	_bDrawing(false),
 	_bNewImage(0),
 	_width(size.GetWidth( )),
 	_height(size.GetHeight( ))
{
}

ImageWindow::~ImageWindow() {
}
void ImageWindow::onPaint( wxPaintEvent& event )
{
	wxPaintDC dc(this);
	draw( dc );
}
void ImageWindow::draw( wxDC& dc )
{
	// check if dc available
	if( !dc.Ok( ) || _bDrawing == true ){ return; }

	_bDrawing = true;


	int x,y,w,h;
	dc.GetClippingBox( &x, &y, &w, &h );
	// if there is a new image to draw
	if( _bNewImage )
	{
		dc.DrawBitmap( _bitmap, x, y );
		_bNewImage = false;
	} else
	{
		// draw inter frame ?
	}

	_bDrawing = false;

	return;
}

void ImageWindow::drawOpenNIImage( BufferData *pBuffer )
{
	// Change this crappy synchronisation stuff...
	if( _bDrawing ) return;
	_bDrawing = true;

	// if there was an image then we need to update view
    if( pBuffer )
    {
		// copy data to wxImg
		wxImage pWxImg = wxImage( pBuffer->getWidth(), pBuffer->getHeight());
		pBuffer->putData(pWxImg.GetData(),true);

		// convert to bitmap to be used by the window to draw
		_bitmap = wxBitmap( pWxImg.Scale(_width, _height) );

		_bNewImage = true;
		_bDrawing = false;
    }
}

void ImageWindow::drawOpenCVImage( cv::Mat img )
{
//	return;
	if( _bDrawing ) return;
	_bDrawing = true;

	// if there was an image then we need to update view
    if( img.cols != 0 )
    {
    	cv::Mat dstImg;

        if (img.channels() == 1)
        {
        	cv::cvtColor(img,dstImg, CV_GRAY2RGB);
        }
        else
        {
        	dstImg = img;
        }

		// convert data from raw image to wxImg
		wxImage pWxImg = wxImage( dstImg.cols, dstImg.rows, dstImg.data, TRUE );
		// convert to bitmap to be used by the window to draw
		_bitmap = wxBitmap( pWxImg.Scale(_width, _height) );

		_bNewImage = true;
		_bDrawing = false;
    }

}

void ImageWindow::checkUpdate()
{
	Update( );
}

void ImageWindow::onSize( wxSizeEvent& even )
{
	int nWidth = even.GetSize().GetWidth();
	int nHeight = even.GetSize().GetHeight();

	_width = nWidth;
	_height = nHeight;

}
