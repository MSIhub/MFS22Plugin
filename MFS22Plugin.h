#ifndef MFS22PLUGIN_H_
#define MFS22PLUGIN_H_

#pragma once
#include <iostream>
#include <windows.h>
#include <iomanip>

#include "SimConnect.h"

bool quit = false; // 0 = Keep Running, 1 =Exit app
HANDLE hSimConnect = NULL;


static enum DATA_DEFINE_ID
{
	DEFINITION_1,
};

static enum DATA_REQUEST_ID
{
	REQUEST_1, 
};

//static const char* datatitles[] = { "T", "ax", "ay", "az", "roll", "pitch", "yaw", "vroll", "vpitch", "vyaw", "GS", "mass",
										//"ph_x", "ph_y", "ph_z", "ph_roll", "ph_pitch", "ph_yaw", "zulu_time" }; //nc4 (add header)
struct SimResponse
{
	double ax; //ACCELERATION BODY X
	double ay; //ACCELERATION BODY Y
	double az; //ACCELERATION BODY Z
	double roll;//PLANE BANK DEGREES [Radians]
	double pitch;//PLANE PITCH DEGREES [Radians]
	double yaw;//PLANE HEADING DEGREES TRUE	 [Radians]
	double vroll;//ROTATION VELOCITY BODY X
	double vpitch;//ROTATION VELOCITY BODY Y
	double vyaw;//ROTATION VELOCITY BODY Z
	double GS;//GROUND VELOCITY
	double mass;// TOTAL WEIGHT[Pounds]
};

void CALLBACK MyDispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext);

#endif // !MFS22PLUGIN_H_