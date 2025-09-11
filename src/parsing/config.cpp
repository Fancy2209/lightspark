/**************************************************************************
    Lightspark, a free flash player implementation

    Copyright (C) 2010-2013  Alessandro Pignotti (a.pignotti@sssup.it)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include "parsing/config.h"
#include "compat.h"

using namespace lightspark;

ConfigParser::ConfigParser(const std::string& filename):
	valid(true),
	groups(NULL),
	keys(NULL),
	group(NULL),key(NULL)
{

}

ConfigParser::~ConfigParser()
{
}

bool ConfigParser::read()
{
	return false;
}

std::string ConfigParser::getValue() 
{
	return "";
}

std::string ConfigParser::getValueString()
{
	return "";

}

std::vector<std::string> ConfigParser::getValueStringList()
{
	std::vector<std::string> ret;
		ret.push_back(std::string(""));
	return ret;
}
