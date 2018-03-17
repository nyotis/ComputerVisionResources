/*
 *   Motor.h
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

// OpenNI includes
#include "OpenNI.h"
#include <XnUSB.h>
#include <memory>

#ifndef MOTOR_H_
#define MOTOR_H_

/**
 * Class to control Device's motor.
 */
class DeviceMotor
{
private:
    DeviceMotor();

    /**
     * Open device.
     * @return true if succeeded, false - otherwise
     */
    bool open(int deviceID);

    /**
     * Close device.
     */
    void close();

    /**
     * Move motor up or down to a specified angle.
     * @param angle angle value
     * @return true if succeeded, false - otherwise
     */
    bool move(int angle);
public:
    virtual ~DeviceMotor();

    /**
     * Gets the relevant motor object for an OpenNIReader
     * @return the DeviceMotor object that is required.
     */
    static DeviceMotor *getMotor(int id);

	/**
	 * Move motor up one stop from current angle
	 * @return true if succeeded, false - otherwise
	 */
	bool moveUp();

	/**
	 * Move motor down one stop from current angle
	 * @return true if succeeded, false - otherwise
	 */
	bool moveDown();

	/**
	 * Enumeration for setting LED colour on Device
	 */
	enum {
		LED_OFF = 0,
		LED_GREEN  = 1,
		LED_RED    = 2,
		LED_YELLOW = 3,
		LED_BLINK_YELLOW = 4,
		LED_BLINK_GREEN = 5,
		LED_BLINK_RED_YELLOW = 6
	} LightState;

	/**
	 * Set the colour of the LED on the Device
	 * @return true if succeeded, false - otherwise
	 */
	bool setLight(XnUInt32 val);


private:
        XnUSBDeviceHandle *_pDevice;
        bool _bIsOpen;
        int _angle;
        // FUTURE BUG ALERT
        // assume no more than 20 Devices will ever be used
        static std::auto_ptr<DeviceMotor> deviceList[20];
};
#endif /* MOTOR_H_ */
