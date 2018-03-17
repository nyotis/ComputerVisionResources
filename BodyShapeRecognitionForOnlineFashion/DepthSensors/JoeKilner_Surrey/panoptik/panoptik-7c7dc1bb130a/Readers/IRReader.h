/*
 *   IRReader.h
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

#ifndef IRREADER_H_
#define IRREADER_H_

#include "OpenNIReader.h"

/*
 * The primesense sensor seems to use a 7x7 (or 9x9 0r 9x7 according to the web) pixel
 * window for the depth from structured light. As such a 3x3 offset applied to
 * the IR camera calibration will bring it inline with the depth camera image
 */


class IRBufferData : public BufferData {
public:
	IRBufferData(xn::IRMetaData *);
	void setData(xn::MapMetaData *);
	void putData(unsigned short *pDest);
	void putData(unsigned char *pDest, bool rgb);
	~IRBufferData();
private:
	unsigned short *_data;
	int _size;
};

typedef TypedOpenNIReader<xn::IRGenerator, xn::IRMetaData, IRBufferData , XN_NODE_TYPE_IR> IRReader;

#endif /* IRREADER_H_ */
