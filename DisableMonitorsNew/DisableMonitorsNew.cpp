#include <iostream>
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <iterator>

using namespace std;

const string DISPLAY_NAME_PREFIX = "\\\\.\\DISPLAY";

string convertDisplayName(WCHAR* displayName)
{
	wstring deviceNameWstring = wstring(displayName);
	return string(deviceNameWstring.begin(), deviceNameWstring.end());
}

void SetPrimaryDisplay(string displayToMakePrimary)
{
	DWORD           DispNum = 0;
	DISPLAY_DEVICE  DisplayDevice;
	int i = 0;
	DEVMODE    DevMode;
	ZeroMemory(&DevMode, sizeof(DevMode));
	DevMode.dmSize = sizeof(DevMode);

	// Initialize DisplayDevice
	ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
	DisplayDevice.cb = sizeof(DisplayDevice);

	// get all display devices
	while (EnumDisplayDevices(NULL, DispNum, &DisplayDevice, 0))
	{
		if (convertDisplayName(DisplayDevice.DeviceName) == displayToMakePrimary)
		{
			DevMode.dmFields = DM_POSITION;
			DevMode.dmPosition.x = 0;
			DevMode.dmPosition.y = 0;
			auto flags = CDS_UPDATEREGISTRY | CDS_NORESET | CDS_SET_PRIMARY;
			ChangeDisplaySettingsEx((LPCWSTR)DisplayDevice.DeviceName, &DevMode, NULL, flags, NULL);
			ChangeDisplaySettingsEx(NULL, NULL, NULL, 0, NULL);
		}

		// Reinit DisplayDevice just to be extra clean
		ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
		DisplayDevice.cb = sizeof(DisplayDevice);
		DispNum++;
	}
}

void EnableProperDisplays(vector<string> &displaysToEnable)
{
	DWORD           DispNum = 0;
	DISPLAY_DEVICE  DisplayDevice;
	int i = 0;

	// Initialize DisplayDevice
	ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
	DisplayDevice.cb = sizeof(DisplayDevice);

	// get all display devices
	while (EnumDisplayDevices(NULL, DispNum, &DisplayDevice, 0))
	{
		// Initialize: 
		DEVMODE    DevMode;
		ZeroMemory(&DevMode, sizeof(DevMode));
		DevMode.dmSize = sizeof(DevMode);

		// Check to see if display should be enabled, and whether it should be primary: 
		bool shouldBeEnabled = find(displaysToEnable.begin(), displaysToEnable.end(), convertDisplayName(DisplayDevice.DeviceName)) != displaysToEnable.end();

		// If screen should be disabled, turn it off: 
		if (!shouldBeEnabled)
		{
			DevMode.dmFields = DM_POSITION | DM_PELSHEIGHT | DM_PELSWIDTH;
			DevMode.dmPelsHeight = 0;
			DevMode.dmPelsWidth = 0;
			ChangeDisplaySettingsEx((LPCWSTR)DisplayDevice.DeviceName, &DevMode, NULL, CDS_UPDATEREGISTRY | CDS_RESET, NULL);
		} else {
			DevMode.dmFields = DM_POSITION;
			DevMode.dmPosition.x = 0;
			DevMode.dmPosition.y = 0;
			auto flags = CDS_UPDATEREGISTRY | CDS_NORESET;
			ChangeDisplaySettingsEx((LPCWSTR)DisplayDevice.DeviceName, &DevMode, NULL, flags, NULL);
		}
		ChangeDisplaySettingsEx(NULL, NULL, NULL, 0, NULL);

		// Reinit DisplayDevice just to be extra clean
		ZeroMemory(&DisplayDevice, sizeof(DisplayDevice));
		DisplayDevice.cb = sizeof(DisplayDevice);
		DispNum++;
	}
}

int main()
{
	// TO BE LOADED VIA CMD LINE / CONFIG: 
	vector<string> displaysToEnable;


	displaysToEnable.push_back(DISPLAY_NAME_PREFIX + "1");

	displaysToEnable.push_back(DISPLAY_NAME_PREFIX + "2");
	// displaysToEnable.push_back(DISPLAY_NAME_PREFIX + "3");

	// Execute: 
	EnableProperDisplays(displaysToEnable);
	SetPrimaryDisplay(displaysToEnable.front());

	// Success!
	return 0;
}