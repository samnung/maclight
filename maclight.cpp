/**
	Copyright (c) 2013, samnung
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	* Redistributions of source code must retain the above copyright notice, this
	  list of conditions and the following disclaimer.

	* Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
	OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>
#include <vector>
#include <sstream>

#include "lights_handle.h"


typedef enum {
	UNKNOWN,
	SET_KEYBOARD,
	GET_KEYBOARD,

	SET_DISPLAY,
	GET_DISPLAY,

	DISPLAY_SLEEP,
	DISPLAY_WAKE,

	GET_SENSOR
} ArgumentType;


typedef std::vector<std::string> string_list;



float floatFromString(const std::string & str)
{
	std::stringstream ss(str);
	float value;
	ss >> value;

	return value;
}

void printUsage(std::ostream & os = std::cout)
{
	os << "usage: maclight OPTION\n";
	os << "OPTION:\n";
	os << "--set-keyboard <number> | --sk <number>    set keyboard brightness\n";
	os << "--get-keyboard          | --gk             get keyboard brightness\n";
	os << "\n";
	os << "--set-display <number>  | --sd <number>    set display brightness\n";
	os << "--get-display           | --gd             get display brightness\n";
	os << "\n";
	os << "--display-sleep         | --ds             put display to sleep\n";
	os << "--display-wake          | --dw             wake up display\n";
	os << "\n";
	os << "--get-sensor            | --gs             get display sensor value\n";
	os << "\n\n";
	os << "<number> = 0.0 – 1.0\n";
}


int main(int argc, char const *argv[])
{
	string_list args(argv, argv + argc);

	std::string value;
	ArgumentType arg_type = UNKNOWN;


	try
	{
		for (size_t i = 1; i < args.size(); i++)
		{
			std::string & arg = args[i];

			if ( arg == "--get-keyboard" or arg == "--gk" )
			{
				arg_type = GET_KEYBOARD;
			}
			else if ( arg == "--set-keyboard" or arg == "--sk" )
			{
				arg_type = SET_KEYBOARD;
				value = args.at(++i);
			}
			else if ( arg == "--get-display" or arg == "--gd" )
			{
				arg_type = GET_DISPLAY;
			}
			else if ( arg == "--set-display" or arg == "--sd" )
			{
				arg_type = SET_DISPLAY;
				value = args.at(++i);
			}
			else if ( arg == "--display-sleep" or arg == "--ds" )
			{
				arg_type = DISPLAY_SLEEP;
			}
			else if ( arg == "--display-wake" or arg == "--dw" )
			{
				arg_type = DISPLAY_WAKE;
			}
			else if ( arg == "--get-sensor" or arg == "--gs" )
			{
				arg_type = GET_SENSOR;
			}
			else if ( arg == "--help" or arg == "-h" )
			{
				printUsage();
				return 0;
			}
			else
			{
				std::cerr << "error parsing arguments\n";
				return 1;
			}
		}
	}
	catch (...)
	{
		std::cerr << "error parsing arguments\n";
		return 1;
	}

	switch (arg_type)
	{
		case GET_KEYBOARD:
		{
			float brightness = getKeyboardBrightness();
			std::cout << brightness << std::endl;
			break;
		}
		case SET_KEYBOARD:
		{
			float brightness = floatFromString(value);
			setKeyboardBrightness(brightness);
			std::cout << getKeyboardBrightness() << std::endl;

			break;
		}

		case GET_DISPLAY:
		{
			float brightness = getDisplayBrightness();
			std::cout << brightness << std::endl;
			break;
		}
		case SET_DISPLAY:
		{
			float brightness = floatFromString(value);
			setDisplayBrightness(brightness);
			std::cout << getDisplayBrightness() << std::endl;

			break;
		}
		case GET_SENSOR:
		{
			float left, right;
			getLightSensors(&left, &right);
			std::cout << left << " " << right << std::endl;
			break;
		}
		case DISPLAY_SLEEP:
		{
			setDisplayWake(false);
			break;
		}
		case DISPLAY_WAKE:
		{
			setDisplayWake(true);
			break;
		}
		case UNKNOWN:
		{
			printUsage();
			return 1;
			break;
		}
	}

	return 0;
}