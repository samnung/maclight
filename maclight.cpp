

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

			if ( arg == "--get-keyboard" )
				arg_type = GET_KEYBOARD;
			else if ( arg == "--set-keyboard" )
			{
				arg_type = SET_KEYBOARD;
				value = args.at(++i);
			}
			else if ( arg == "--get-display" )
				arg_type = GET_DISPLAY;
			else if ( arg == "--set-display" )
			{
				arg_type = SET_DISPLAY;
				value = args.at(++i);
			}
			else if ( arg == "--display-sleep" )
				arg_type = DISPLAY_SLEEP;
			else if ( arg == "--display-wake" )
				arg_type = DISPLAY_WAKE;

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
			std::stringstream ss(value);
			float brightness;
			ss >> brightness;

			std::cout << brightness << std::endl;
			setKeyboardBrightness(brightness);

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
			std::stringstream ss(value);
			float brightness;
			ss >> brightness;

			std::cout << brightness << std::endl;
			setDisplayBrightness(brightness);

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
			std::cerr << "usage:\n";
			return 1;
			break;
		}
	}

	return 0;
}