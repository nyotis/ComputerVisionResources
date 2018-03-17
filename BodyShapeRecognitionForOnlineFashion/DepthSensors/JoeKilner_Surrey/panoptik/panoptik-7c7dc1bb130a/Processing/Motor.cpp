/*
 *   Motor.cpp
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
#include "Motor.h"

#include <iostream>

using namespace std;

auto_ptr<DeviceMotor> DeviceMotor::deviceList[20];

DeviceMotor::DeviceMotor() :
	_pDevice(0),
	_bIsOpen(false),
	_angle(0)
{
}

DeviceMotor::~DeviceMotor()
{
        close();
}

bool DeviceMotor::open(int deviceID)
{
        const XnUSBConnectionString *paths;
        XnUInt32 count;
        XnStatus res;

        // Open "Device motor" USB device
        res = xnUSBEnumerateDevices(0x045E /* VendorID */, 0x02B0 /*ProductID*/, &paths, &count);
        if (res != XN_STATUS_OK) {
                xnPrintError(res, "xnUSBEnumerateDevices failed");
                return(false);
        }

        // Open correct device
        res = xnUSBOpenDeviceByPath(paths[deviceID], &_pDevice);
        if (res != XN_STATUS_OK) {
                xnPrintError(res, "xnUSBOpenDeviceByPath failed");
                return(false);
        }

        res = xnUSBSendControl(_pDevice, XN_USB_CONTROL_TYPE_VENDOR, 0x06, 0x01, 0x00, NULL, 0, 0);
        if (res != XN_STATUS_OK) {
                xnPrintError(res, "xnUSBSendControl failed");
                close();
                return(false);
        }
        _bIsOpen = true;
        return(true);
}

void DeviceMotor::close()
{
	if (_bIsOpen) {
		xnUSBCloseDevice(_pDevice);
		_bIsOpen = false;
	}
}

bool DeviceMotor::move(int angle)
{
    if (_bIsOpen) {
        XnStatus res;
        if (angle > 31) angle = 31;
        if (angle < -31) angle = -31;

        // Send move control request
        res = xnUSBSendControl(_pDevice, XN_USB_CONTROL_TYPE_VENDOR, 0x31, angle*2, 0x00, NULL, 0, 0);
        if (res != XN_STATUS_OK) {
                xnPrintError(res, "xnUSBSendControl failed");
                return(false);
        }
        _angle = angle;
        return(true);
    }
    return(false);
}

bool DeviceMotor::moveUp(){
	return(move(_angle+1));
}

bool DeviceMotor::moveDown(){
	return(move(_angle-1));
}


bool DeviceMotor::setLight(XnUInt32 val)
{
    if (_bIsOpen) {
		XnStatus res;

		// Send move control request
		res = xnUSBSendControl(_pDevice, XN_USB_CONTROL_TYPE_VENDOR, 0x06, val, 0x00, NULL, 0, 0);
		if (res != XN_STATUS_OK) {
				xnPrintError(res, "xnUSBSendControl failed");
				return(false);
		}
		return(true);
    }
    return(false);
}


DeviceMotor *DeviceMotor::getMotor(int id) {
	if (deviceList[id].get() == 0) {
		auto_ptr<DeviceMotor> pNewMotor(new DeviceMotor());
		// If opening fails assignment will not take place and the object will
		// be deleted as the auto_ptr goes out of scope.
		if (pNewMotor->open(id)) {
			deviceList[id] = pNewMotor;
		}
	}
	return(deviceList[id].get());
}
