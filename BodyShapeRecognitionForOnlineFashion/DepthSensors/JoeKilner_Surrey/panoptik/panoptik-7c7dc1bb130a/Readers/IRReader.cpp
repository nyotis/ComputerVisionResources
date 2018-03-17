/*
 *   IRReader.cpp
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

#include "IRReader.h"
using namespace xn;

/******************************* Buffer Data ******************************************/
IRBufferData::IRBufferData(IRMetaData *pData) :
	BufferData(pData),
	_data(0),
	_size(pData->XRes() * pData->YRes()) {
	_data = new unsigned short [_size];
}

IRBufferData::~IRBufferData(){
	delete [] _data;
	_data = 0;
	_size = 0;
}

void IRBufferData::putData(unsigned short *pDest) {
	memcpy(pDest, _data, _size * sizeof(unsigned short));
}
void IRBufferData::putData(unsigned char *pDest, bool rgb) {
	unsigned short *inData = _data;
	if (rgb) {
		for (int i = 0; i < _size; ++i, ++inData, ++pDest)
		{
			unsigned char bottomBits = (*inData) & 0xff;
			unsigned char topBits = ((*inData) >> 8) & 0xff;
			if (topBits == 0)
			{
				*pDest = bottomBits;
				 ++pDest;
				 ++pDest;
			}
			if (topBits == 1)
			{
				*pDest = 0xff;
				 ++pDest;
				*pDest = bottomBits;
				 ++pDest;
			}
			if (topBits == 2)
			{
				*pDest = 0xff - bottomBits;
				 ++pDest;
				*pDest = 0xff;
				 ++pDest;
			}
			if (topBits == 3)
			{
				 ++pDest;
				*pDest = 0xff;
				 ++pDest;
				*pDest = bottomBits;
			}
			if (topBits == 4)
			{
				 ++pDest;
				*pDest = 0xff - bottomBits;
				 ++pDest;
				*pDest = 0xff;
			}
			if (topBits == 5)
			{
				*pDest = bottomBits;
				 ++pDest;
				 ++pDest;
				*pDest = 0xff;
			}
			if (topBits == 6)
			{
				*pDest = 0xff;
				 ++pDest;
				*pDest = bottomBits;
				 ++pDest;
				*pDest = 0xff;
			}
		}
	}
	else {
		for (int i = 0; i < _size; ++i, ++inData, ++pDest)
		{
			short val = (*inData) >> 1;
			if (val > 0xff) {
				*pDest = 0xff;

			}
			else {
				*pDest = val;
			}
		}
	}
}

void IRBufferData::setData(MapMetaData *pData) {
	IRMetaData *pIRData = dynamic_cast<IRMetaData *>(pData);
	if (pIRData) {
		int nCamWidth = pIRData->XRes();
		int nCamHeight = pIRData->YRes();

		if (nCamWidth * nCamHeight != _size) {
			printf("Error - buffer size and data size do not match!\n");
			return;
		}

		const XnIRPixel *inData = pIRData->Data();
		unsigned short *outData = _data;
		for (int y = 0; y < nCamWidth; ++y)
		{
			for (int x = 0; x < nCamHeight; ++x, ++inData, ++outData)
			{
				*outData = *inData;
			}
		}
	}
}


