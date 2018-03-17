/*
 *   CaptureUI.h
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
 
#ifndef CAPTUREUI_H
#define CAPTUREUI_H

#include <list>
#include <string>

class CaptureUI {
public:
	CaptureUI(bool isMaster, std::string postFix) {
		_isMaster = isMaster;
		_postFix = postFix;
	}

	virtual ~CaptureUI(){};

	void addListener(CaptureUI* pListener) {
		if (_isMaster) {
			_listeners.push_back(pListener);
		}
		// Throw exception here
	}

	void save(std::string name) {
		if (_isMaster) {
			for (std::list<CaptureUI *>::iterator i = _listeners.begin();
					i != _listeners.end();
					++i) {
				(*i)->saveImpl(name + (*i)->_postFix);
			}
		}
		saveImpl(name);
	}

	void setIsMaster(bool isMaster) {
		_isMaster = isMaster;
	}

protected:
	virtual void saveImpl(std::string name) = 0;

private:
	bool _isMaster;
	std::list<CaptureUI *> _listeners;
	std::string _postFix;
};

#endif //CAPTUREUI_H
