/*
 *   DepthReader.cpp
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

#include "DepthReader.h"
using namespace xn;

/******************************* Buffer Data ******************************************/
DepthBufferData::DepthBufferData(xn::DepthMetaData *pData) :
	BufferData(pData),
	_data(0),
	_size(pData->XRes() * pData->YRes()) {
	_data = new unsigned short [_size];
}

DepthBufferData::~DepthBufferData(){
	delete [] _data;
	_data = 0;
	_size = 0;
}

void DepthBufferData::putData(unsigned short *pDest) {
	memcpy(pDest, _data, _size * sizeof(unsigned short));
}
void DepthBufferData::putData(unsigned char *pDest, bool rgb) {
	unsigned short *inData = _data;
	if (rgb) {
		for (int i = 0; i < _size; ++i, ++inData, pDest+=3)
		{
			if(*inData)
			{
				for (int i = 0 ; i < 16; )
				{
					i+=3;
					*pDest     |= (((*inData) & (1<<(i  ))) >> ((i/3)  ));
					*(pDest+1) |= (((*inData) & (1<<(i+1))) >> ((i/3)+1));
					*(pDest+2) |= (((*inData) & (1<<(i+2))) >> ((i/3)+2));
				}
				*(pDest  ) = (*(pDest  ) << 2) | 3;
				*(pDest+1) = (*(pDest+1) << 2) | 3;
				*(pDest+2) = (*(pDest+2) << 2) | 3;
			}
			else {
				*pDest     = 0;
				*(pDest+1) = 0;
				*(pDest+2) = 0;
			}
		}
	}
	else {
		for (int i = 0; i < _size; ++i, ++inData, ++pDest)
		{
			if (*inData)
			{
				*pDest = 0;
			}
			else
			{
				*pDest = 255;
			}
		}
	}
}

void DepthBufferData::setData(MapMetaData *pData) {
	DepthMetaData *pDepthData = dynamic_cast<DepthMetaData *>(pData);
	if (pDepthData) {
		int nCamWidth = pDepthData->XRes();
		int nCamHeight = pDepthData->YRes();

		if (nCamWidth * nCamHeight != _size) {
			printf("Error - buffer size and data size do not match!\n");
			return;
		}

		const XnDepthPixel *inData = pDepthData->Data();
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
