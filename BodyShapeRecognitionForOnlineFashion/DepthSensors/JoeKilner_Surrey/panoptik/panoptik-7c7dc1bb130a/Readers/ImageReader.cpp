/*
 *   ImageReader.cpp
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

#include "ImageReader.h"
using namespace xn;

/******************************* Buffer Data ******************************************/
ImageBufferData::ImageBufferData(ImageMetaData *pData) :
		BufferData(pData),
		_data(0),
		_size(pData->XRes() * pData->YRes() * 3) {
	_data = new unsigned char [_size];
}

ImageBufferData::~ImageBufferData(){
	delete []_data;
	_data = 0;
	_size = 0;
}

void ImageBufferData::putData(unsigned char *pDest, bool rgb) {
	if (rgb) {
		memcpy(pDest, _data, _size * sizeof(unsigned char));
	}
	else
	{
		unsigned char *inData = _data;
		for (int i = 0; i < _size; i+=3, inData+=3, ++pDest)
		{
			*pDest = (int(*inData) + int(*(inData+1)) + int(*(inData+2))) / 3;
		}
	}
}

void ImageBufferData::setData(MapMetaData *pData) {
	ImageMetaData *pImageData = dynamic_cast<ImageMetaData *>(pData);
	if (pImageData) {
		int nCamWidth = pImageData->XRes();
		int nCamHeight = pImageData->YRes();

		if (nCamWidth * nCamHeight * 3 != _size) {
			printf("Error - buffer size and data size do not match!\n");
			return;
		}

		const XnRGB24Pixel *inData = pImageData->RGB24Data();
		unsigned char *outData = _data;
		for (int y = 0; y < nCamWidth; ++y)
		{
			for (int x = 0; x < nCamHeight; ++x, ++inData, ++outData)
			{
					*outData = inData->nRed;
					++outData;
					*outData = inData->nGreen;
					++outData;
					*outData = inData->nBlue;
			}
		}
	}
}

