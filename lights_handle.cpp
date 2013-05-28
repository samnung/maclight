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

enum {
	kGetSensorReadingID = 0, // getSensorReading(int *, int *)
	kGetLEDBrightnessID = 1, // getLEDBrightness(int, int *)
	kSetLEDBrightnessID = 2, // setLEDBrightness(int, int, int *)
	kSetLEDFadeID = 3, // setLEDFade(int, int, int, int *)
	// other firmware-related functions
	// verifyFirmwareID = 4,
	// verifyFirmware(int *)
	// getFirmwareVersionID = 5,
	// getFirmwareVersion(int *)
	// other flashing-related functions
	// ...
};


#include <mach/mach.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>
#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>
#include <iostream>


static io_connect_t dataPort = 0; // shared?

io_connect_t getDataPort(void)
{
	kern_return_t     kr;
	io_service_t      serviceObject;

	if (dataPort) return dataPort;

	// Look up a registered IOService object whose class is AppleLMUController
	serviceObject = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleLMUController"));

	if (!serviceObject)
	{
		std::cerr << "getDataPort() error: failed to find ambient light sensor\n";
		return 0;
	}

	// Create a connection to the IOService object
	kr = IOServiceOpen(serviceObject, mach_task_self(), 0, &dataPort);
	IOObjectRelease(serviceObject);

	if (kr != KERN_SUCCESS)
	{
		std::cerr << "getDataPort() error: failed to open IoService object\n";
		return 0;
	}

	return dataPort;
}

void getLightSensors(float *left, float *right)
{
	assert(left || right);

    uint64_t inputValues[0];
	uint32_t inputCount = 0;

    uint64_t outputValues[2];
	uint32_t outputCount = 2;

	kern_return_t kr;

    kr = IOConnectCallScalarMethod(getDataPort(),
								   kGetSensorReadingID,
								   inputValues,
								   inputCount,
								   outputValues,
								   &outputCount);

	if (kr != KERN_SUCCESS)
	{
		std::cerr << "error getting light sensor values\n";
		return;
	}

	*left = outputValues[0];
	*right = outputValues[1];

	*left  /= 2000;
	*right /= 2000;
}



float getDisplayBrightness(void)
{
	CGDisplayErr      dErr;
	io_service_t      service;
	CGDirectDisplayID targetDisplay;

	CFStringRef key = CFSTR(kIODisplayBrightnessKey);
	float brightness = HUGE_VALF;

	targetDisplay = CGMainDisplayID();
	service = CGDisplayIOServicePort(targetDisplay);

	dErr = IODisplayGetFloatParameter(service, kNilOptions, key, &brightness);

	return brightness;
}

void setDisplayBrightness(float brightness)
{
	CGDisplayErr      dErr;
	io_service_t      service;
	CGDirectDisplayID targetDisplay;
	CFStringRef key = CFSTR(kIODisplayBrightnessKey);

	targetDisplay = CGMainDisplayID();
	service = CGDisplayIOServicePort(targetDisplay);

	dErr = IODisplaySetFloatParameter(service, kNilOptions, key, brightness);

}

// Keyboard Brightness

float getKeyboardBrightness(void)
{
	float f;
	kern_return_t kr;

	uint64_t inputCount = 1;
    uint64_t inputValues[1] = {0};

    uint32_t outputCount = 1;
    uint64_t outputValues[1];

    uint32_t out_brightness;

	kr = IOConnectCallScalarMethod(getDataPort(),
								   kGetLEDBrightnessID,
								   inputValues,
								   inputCount,
								   outputValues,
								   &outputCount);

    out_brightness = outputValues[0];

	if (kr != KERN_SUCCESS)
	{
		std::cerr << "getKeyboardBrightness() error\n";
		return -1.0;
	}

	f = out_brightness;
	f /= 0xfff;
	return f;
}

void setKeyboardBrightness(float in)
{
	kern_return_t kr;

	uint64_t inputCount  = 2;
    uint64_t inputValues[2];
    uint64_t in_unknown = 0;
    uint64_t in_brightness = in * 0xfff;

    inputValues[0] = in_unknown;
    inputValues[1] = in_brightness;

    uint32_t outputCount = 1;
    uint64_t outputValues[1];

    uint32_t out_brightness;

	//kr = IOConnectMethodScalarIScalarO(dp, kSetLEDBrightnessID,
	kr = IOConnectCallScalarMethod(getDataPort(),
								   kSetLEDBrightnessID,
								   inputValues,
								   inputCount,
								   outputValues,
								   &outputCount);

    out_brightness = outputValues[0];

	if (kr != KERN_SUCCESS)
	{
		std::cerr << "setKeyboardBrightness() error\n";
		return;
	}
}



void setDisplayWake(bool wake)
{
    io_registry_entry_t entry = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/IOResources/IODisplayWrangler");
    if(entry)
    {
        IORegistryEntrySetCFProperty(entry, CFSTR("IORequestIdle"), wake ? kCFBooleanFalse : kCFBooleanTrue);
        IOObjectRelease(entry);
    }
}