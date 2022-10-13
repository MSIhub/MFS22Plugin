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
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ACCELERATION BODY X", "Feet (ft) per second squared"); //Default is float64
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ACCELERATION BODY Y", "Feet (ft) per second squared");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ACCELERATION BODY Z", "Feet (ft) per second squared");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "PLANE BANK DEGREES", "Radians");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "PLANE PITCH DEGREES", "Radians");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "PLANE HEADING DEGREES TRUE", "Radians");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ROTATION VELOCITY BODY X", "Feet (ft) per second");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ROTATION VELOCITY BODY Y", "Feet (ft) per second");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "ROTATION VELOCITY BODY Z", "Feet (ft) per second");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "GROUND VELOCITY", "Knots");
	hr = SimConnect_AddToDataDefinition(hSimConnect, DEFINITION_1, "TOTAL WEIGHT", "Pounds");

	hr = SimConnect_RequestDataOnSimObject(hSimConnect, REQUEST_1, DEFINITION_1, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_SIM_FRAME);

	//SET UP SOCKETS


	//Process incoming SimConnect server messages while the app is running
	while (!quit)
	{
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
			//SEND SOCKET
			// Now you have the data
			std::cout  << "\ax: " << pS->ax
				<< " - ay: " << pS->ay
				<< " - az: " << pS->az << std::setprecision(4)  << std::endl << std::flush;
			
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