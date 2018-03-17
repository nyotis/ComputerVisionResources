/*
 *   IRViewer.h
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

// Unused
// This file was an attempt to stream the IR image as a texture onto a plane
// I never got the video texture updating, and this is now dead code no longer
// up-to-date with other changes in the source base

#ifndef IRVIEWER_H_
#define IRVIEWER_H_

#include <osg/Geometry>
#include <osg/Texture2D>
#include "IRReader.h"

class IRViewer {
public:
	IRViewer(IRReader *);
	virtual ~IRViewer();

	void getFrame ();

	void init(int width, int height);

	osg::ref_ptr<osg::Geode> getNode();

	int _width;
	int _height;
	IRReader *_pIR;
	osg::ref_ptr<osg::Geode> geode;
	osg::ref_ptr<osg::Image> image;
	osg::ref_ptr<osg::Texture2D> texture;
};

#endif /* IRVIEWER_H_ */
