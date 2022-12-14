#include "mfs22plugin.h"


int main()
{
	HRESULT hr = SimConnect_Open(&hSimConnect, "Client: Data to Cueing", NULL, 0, 0, 0);
	if (hr != S_OK)
	{
		std::cout << "Failed to connect!\n";
		return(-1);
	}
	std::cout << "Connected to MFS22: Data Extraction Started! \n";

	

	//Requesting Data
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ANIMATION DELTA TIME", "Seconds"); //Default is float64
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "PLANE BANK DEGREES", "Radians");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "PLANE PITCH DEGREES", "Radians");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "PLANE HEADING DEGREES TRUE", "Radians");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ACCELERATION BODY X", "Feet");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ACCELERATION BODY Y", "Feet");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ACCELERATION BODY Z", "Feet");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ROTATION VELOCITY BODY X", "Radians");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ROTATION VELOCITY BODY Y", "Radians");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ROTATION VELOCITY BODY Z", "Radians");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "GROUND VELOCITY", "Knots");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "TOTAL WEIGHT", "Pounds");

	hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_1, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);

	//SET UP SOCKETS
	initSocketSetup();

	//Process incoming SimConnect server messages while the app is running
	while (!quit)
	{
		startSocketSession();
		SimConnect_CallDispatch(hSimConnect, MyDispatchProc, NULL);
		Sleep(1);
	}
	// Close our SimConnect Handle
	hr = SimConnect_Close(hSimConnect);
	if (hr != S_OK)
	{
		return -2;
	}
	hSimConnect = nullptr;

	cleanUpSocket();
	return 0;
}


void CALLBACK MyDispatchProc(SIMCONNECT_RECV* pData, DWORD cbData, void * pContext)
{
	switch (pData->dwID)
	{
	case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
	{

		SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

		switch (pObjData->dwRequestID)
		{
		case REQUEST_1:
		{
			
			SimResponse* pS = (SimResponse*)&pObjData->dwData;

			//SEND DATA THROUGH SOCKET
			MPD_CalculateMotionData(pS);
			float t = MPD_MotionData[0];
			float hz = 1.0f / (0.04);

			if (int res = sendto(client_socket, (char*)MPD_MotionData, sizeof(MPD_MotionData), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
				printf("sendto() failed with error code: %d", WSAGetLastError());
			else
			{ 
				printf("send ok [%d] [%dHz]\n", res, (int)hz);
				//memset(MPD_MotionData, 0, sizeof(MPD_MotionData));
			}
			
			/*	std::cout << pS->dt << "\t" << pS->roll * (57.2958) << "\t" << pS->pitch * (57.2958) << "\t" << pS->yaw * (57.2958) << "\t" << pS->ax * 0.3048 << "\t" << pS->ay * 0.3048 << "\t" << pS->az * 0.3048 << "\t" << pS->vroll << "\t" << pS->vpitch  << "\t" << pS->vyaw << "\t" << pS->GS << "\t" << pS->mass << "\n" << std::flush;*/
			//std::cout << pS->vroll * (57.2958) << "\t" << pS->vpitch * (57.2958) << "\t" << pS->vyaw * (57.2958) << "\n" << std::flush;
			//std::cout << pS->ax * 0.3048 << "\t" << pS->ay * 0.3048 << "\t" << pS->az * 0.3048 << "\n" << std::flush;

			break;
		}
		}

		break;
	}
	case SIMCONNECT_RECV_ID_QUIT:
	{
		quit = true;
		break;
	}
	default:
		break;

	}
}


//-- 
void initSocketSetup()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return;
	}


	// nc3 (asss new with var)
	memset(MPD_MotionData, 0, sizeof(MPD_MotionData));

	// create socket : cueing
	if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) // <<< UDP socket
	{
		printf("socket() failed with error code: %d", WSAGetLastError());
		//return 2;
	}
	// setup address structure : cueing
	// server comm start -- 
	memset((char*)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(UDP_PORT);
	server.sin_addr.S_un.S_addr = inet_addr(SERVER);
	

	//Rasberry Pi "172.16.1.1" 

	// create socket : controller
	if ((client_socket_ctrl = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) // <<< UDP socket
	{
		printf("socket() failed with error code: %d", WSAGetLastError());
		//return 2;
	}

	//setup address structure : controller
	memset((char*)&server_ctrl, 0, sizeof(server_ctrl));
	server_ctrl.sin_family = AF_INET;
	server_ctrl.sin_port = htons(CTRL_UDP_PORT);
	server_ctrl.sin_addr.S_un.S_addr = inet_addr(SERVER);

	firstRun = true;
}

void cleanUpSocket()
{
	strcpy(dataControl, "STOP");
	if (sendto(client_socket_ctrl, (char*)dataControl, sizeof(dataControl), 0, (sockaddr*)&server_ctrl, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code: %d", WSAGetLastError());
	}
	
	closesocket(client_socket);

	//send control signal to disconnect
	strcpy(dataControl, "DISC");
	if (sendto(client_socket_ctrl, (char*)dataControl, sizeof(dataControl), 0, (sockaddr*)&server_ctrl, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code: %d", WSAGetLastError());
		//return 3;
	}

	//send control signal to quit
	strcpy(dataControl, "QUIT");
	if (sendto(client_socket_ctrl, (char*)dataControl, sizeof(dataControl), 0, (sockaddr*)&server_ctrl, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code: %d", WSAGetLastError());
		//return 3;
	}


	closesocket(client_socket_ctrl);
	WSACleanup();
}


void startSocketSession()
{
	if (firstRun)
	{
		// send control signal to connect 
		strcpy(dataControl, "CONN");
		if (sendto(client_socket_ctrl, (char*)dataControl, sizeof(dataControl), 0, (sockaddr*)&server_ctrl, sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code: %d", WSAGetLastError());
			//return 3;
		}
		firstRun = false;
	}

	strcpy(dataControl, "STRT");
	if (sendto(client_socket_ctrl, (char*)dataControl, sizeof(dataControl), 0, (sockaddr*)&server_ctrl, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("sendto() failed with error code: %d", WSAGetLastError());
		//return 3;
	}
}

void MPD_CalculateMotionData(SimResponse* pS)
{
	// Store the results in an array so that we can easily send it.
	//CONVERT UNITS HERE
	current_time += (float)pS->dt;
	MPD_MotionData[0] = current_time;
	MPD_MotionData[1] = (float)pS->ax * -0.3048f;
	MPD_MotionData[2] = (float)pS->ay * 0.3048f;
	MPD_MotionData[3] = (float)pS->az * -0.3048f;
	MPD_MotionData[4] = (float)pS->bank;
	MPD_MotionData[5] = (float)pS->pitch;
	MPD_MotionData[6] = (float)pS->head;
	MPD_MotionData[7] = (float)pS->wz;
	MPD_MotionData[8] = (float)pS->wx;
	MPD_MotionData[9] = (float)pS->wy*-1.0f;
	MPD_MotionData[10] = (float)pS->GS;
	MPD_MotionData[11] = (float)pS->mass;
	MPD_MotionData[12] = 0.0f;
	MPD_MotionData[13] = 0.0f;
	MPD_MotionData[14] = 0.0f;
	MPD_MotionData[15] = 0.0f;
	MPD_MotionData[16] = 0.0f;
	MPD_MotionData[17] = 0.0f;
	MPD_MotionData[18] = 0.0f;
}