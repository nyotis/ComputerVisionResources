/*
 *   PanoptikShow.h
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

/**
 * TODO:Test Depth vs IR calibration offset (if any)
 * TODO:Use boost auto_ptr etc
 * TODO:Speed up normal calculations
 */

#ifndef PANOPTIKSHOW_H_
#define PANOPTIKSHOW_H_

#include <wx/wx.h>
#include <memory>

#include "ImageReader.h"
#include "ImageDisplayFrame.h"

class PanoptikShow  : public wxApp{
public:
	PanoptikShow();

	// Overloads
	bool OnInit();
    int OnExit();

private:
    static const int NUM_DEVICES = 4;
    std::auto_ptr<ImageReader>         _image[NUM_DEVICES];
};

#endif /* PANOPTIKSHOW_H_ */
