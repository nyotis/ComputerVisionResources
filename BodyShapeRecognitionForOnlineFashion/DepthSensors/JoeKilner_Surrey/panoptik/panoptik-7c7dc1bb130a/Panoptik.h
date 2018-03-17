/*
 *   Panoptik.h
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
#ifndef PANOPTIK_H_
#define PANOPTIK_H_


#include <memory>

#include <wx/wx.h>
#include <zmq.hpp>
#include <osg/Group>

#include "DepthReader.h"
#include "ImageReader.h"
#include "MeshBuilder.h"

class Panoptik  : public wxApp{
public:
	Panoptik();

	// Overloads
	virtual bool OnInit();
	virtual int OnExit();

    zmq::context_t & getZMQContext(){return (_context);}

    void save(std::string filename, bool saveSequentially);
    void loadDepthCalibration(std::string filename);
    void loadImageCalibration(std::string filename);

    void stop();
    void start();

    void setName(int id, std::string name);
    void enableDevice(int id, bool enabled);

    int getNumDevices(){return (_num_devices);}

private:

    static const int NUM_DEVICES = 4;
    int _num_devices;
    zmq::context_t _context;

    std::auto_ptr<DepthReader>         _depth[NUM_DEVICES];
    std::auto_ptr<ImageReader>         _image[NUM_DEVICES];
    std::auto_ptr<MeshBuilder>         _builder[NUM_DEVICES];

	osg::ref_ptr<osg::Group> _group;
};

#endif /* PANOPTIK_H_ */
