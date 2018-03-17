/*
 *   ImageReader.h
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

#ifndef IMAGEREADER_H_
#define IMAGEREADER_H_

#include "OpenNIReader.h"

class ImageBufferData : public BufferData {
public:
	ImageBufferData(xn::ImageMetaData *);
	void setData(xn::MapMetaData *);
	void putData(unsigned char *pDest, bool rgb);
	~ImageBufferData();
private:
	unsigned char *_data;
	int _size;
};

typedef TypedOpenNIReader<xn::ImageGenerator, xn::ImageMetaData, ImageBufferData, XN_NODE_TYPE_IMAGE> ImageReader;

#endif /* IMAGEREADER_H_ */
