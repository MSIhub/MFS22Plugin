#ifndef MFS22PLUGIN_H_
#define MFS22PLUGIN_H_

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996) 

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

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
	float dt; //ANIMATION DELTA TIME [Seconds]
	float ax; //ACCELERATION BODY X [Feet (ft) per second squared]
	float ay; //ACCELERATION BODY Y
	float az; //ACCELERATION BODY Z
	float roll;//PLANE BANK DEGREES [Radians]
	float pitch;//PLANE PITCH DEGREES [Radians]
	float yaw;//PLANE HEADING DEGREES TRUE	 [Radians]
	float vroll;//ROTATION VELOCITY BODY X
	float vpitch;//ROTATION VELOCITY BODY Y
	float vyaw;//ROTATION VELOCITY BODY Z
	float GS;//GROUND VELOCITY
	float mass;// TOTAL WEIGHT[Pounds]
};

void CALLBACK MyDispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void* pContext);



//------ Socket setup
#define SERVER "127.0.0.1"  // or "localhost" - ip address of UDP server
#define UDP_PORT 22608
#define NUM_DATA 19 // nc1


#define CTRL_UDP_PORT 22609 // This is an abstract layer to connect to sp7 and disconnect
char dataControl[5]{}; //"QUIT" "CONN" "STRT" "STOP" "STAT"

// Used to store calculated motion data
float MPD_MotionData[NUM_DATA];

static float current_time = 0.0f;

//SOCKET sock;
//struct sockaddr_in server;

// create socket : controller
sockaddr_in server_ctrl;
int client_socket_ctrl;

sockaddr_in server;
int client_socket;
bool firstRun;

//------


void initSocketSetup();

void cleanUpSocket();

void startSocketSession();

void MPD_CalculateMotionData(SimResponse* pS);
#endif // !MFS22PLUGIN_H_