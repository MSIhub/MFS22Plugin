#ifndef MFS22PLUGIN_H_
#define MFS22PLUGIN_H_

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996) 


#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#include <iostream>
#include <windows.h>
#include <iomanip>
#include <memory>

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
PACK(struct SimResponse
{
	double dt; //ANIMATION DELTA TIME [Seconds]
	double bank;//PLANE BANK DEGREES [Radians]
	double pitch;//PLANE PITCH DEGREES [Radians]
	double head;//PLANE HEADING DEGREES TRUE	 [Radians]
	double ax; //ACCELERATION BODY X [Feet (ft) per second squared]
	double ay; //ACCELERATION BODY Y
	double az; //ACCELERATION BODY Z
	double wx;//ROTATION VELOCITY BODY X
	double wy;//ROTATION VELOCITY BODY Y
	double wz;//ROTATION VELOCITY BODY Z
	double GS;//GROUND VELOCITY
	double mass;// TOTAL WEIGHT[Pounds]
};)



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