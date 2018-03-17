
#	 Copyright(c) 2012 by Joe Kilner
#    This file is part of Panoptik.
#
#    Panoptik is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Panoptik is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with Panoptik.  If not, see <http://www.gnu.org/licenses/>.

# Minimum requirements
cmake_minimum_required(VERSION 2.6.3)

# Project name
project ( Panoptik )

# Source Structure
include_directories(Readers)
include_directories(Render)
include_directories(Processing)
include_directories(UI)

set(SRC Processing/Calibrator.cpp
        Processing/Motor.cpp
        Readers/DepthReader.cpp
        Readers/ImageReader.cpp
        Readers/IRReader.cpp
        Readers/OpenNIReader.cpp
        Readers/OpenNIReaderThread.cpp
        Render/ImageViewer.cpp
        Render/IRViewer.cpp
        Render/MeshBuilder.cpp
        UI/ImageDisplayFrame.cpp
        UI/ImageWindow.cpp
        UI/OSGCanvas.cpp
        UI/ReconstructionFrame.cpp
        UI/WXGraphicsWindow.cpp)

# Find packages
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Modules/")
find_package ( wxWidgets REQUIRED core base gl  )
find_package ( OpenSceneGraph REQUIRED osgViewer osgGA osgDB osgUtil)
find_package ( OpenNI REQUIRED )
find_package ( OpenCV REQUIRED )

# Add include dirs etc.
include( ${wxWidgets_USE_FILE} )
include_directories(${OPENSCENEGRAPH_INCLUDE_DIRS})
include_directories(${OpenNI_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

# Add definitions
add_definitions(-Wall -Wno-unknown-pragmas -fmessage-length=0)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")

# Add outputs
add_executable ( Panoptik UI/ControlUI.cpp Panoptik.cpp ${SRC})
add_executable ( PanoptikCal PanoptikCal.cpp ${SRC})
add_executable ( PanoptikShow PanoptikShow.cpp ${SRC})

# Add libraries
target_link_libraries(Panoptik ${wxWidgets_LIBRARIES} 
                               ${OPENSCENEGRAPH_LIBRARIES}
                               ${OpenNI_LIBRARIES} 
                               ${OpenCV_LIBS}
                               zmq)
                               
target_link_libraries(PanoptikCal  ${wxWidgets_LIBRARIES} 
                               ${OPENSCENEGRAPH_LIBRARIES}
                               ${OpenNI_LIBRARIES} 
                               ${OpenCV_LIBS}
                               zmq)
                               
target_link_libraries(PanoptikShow ${wxWidgets_LIBRARIES} 
                               ${OPENSCENEGRAPH_LIBRARIES}
                               ${OpenNI_LIBRARIES} 
                               ${OpenCV_LIBS}
                               zmq)

#debug
