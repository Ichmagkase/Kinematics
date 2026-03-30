#include "ipc.h"
//#include "sensor.h"
//#include "test.h"
//#include <iostream>
//#include "data.h"

#include <iostream>
#include <string>
#include <functional>
#include <map>
#include <Kinect.h>
#include <Kinect.VisualGestureBuilder.h>
#include "sensor.h"
#include <windows.h>

static IPC ipc;

void GestureListener(struct Data data) {
	std::cout << "Player " << data.player << " performed gesture: " << data.event << std::endl;
	ipc.sendEventPayload(data);
}

// DEBUG
std::string GetHRESULTErrorMessage(HRESULT hr);
void CheckError(HRESULT hr, std::string message);

void debugGestures() {
	IBodyFrameReader* pBodyFrameReader = nullptr;
	IBodyFrameSource* pBodyFrameSource = nullptr;
	IKinectSensor* pSensor = nullptr;
	IVisualGestureBuilderDatabase* pGestureDatabase = nullptr;
	IVisualGestureBuilderFrameSource* gestureSource = nullptr;
	IVisualGestureBuilderFrameReader* gestureReader = nullptr;
	IGesture** pGestures = nullptr;
	UINT gestureCount = 0;

	std::cout << "Kinect Person Detection Program" << std::endl;

	HRESULT hr;

	// Get the default Kinect sensor
	hr = GetDefaultKinectSensor(&pSensor);
	CheckError(hr, "GetDefaultKinectSensor");

	if (!pSensor) {
		std::cerr << "No Kinect sensor found" << std::endl;
		exit(1);
	}

	// Open the sensor
	hr = pSensor->Open();
	CheckError(hr, "IKinectSensor::Open()");

	// Get the body frame source
	hr = pSensor->get_BodyFrameSource(&pBodyFrameSource);
	CheckError(hr, "IKinectSensor::get_BodyFrameSource");

	// Create body frame reader
	hr = pBodyFrameSource->OpenReader(&pBodyFrameReader);
	CheckError(hr, "IBodyFrameSource::OpenReader");

	std::cout << "Kinect sensor initialized successfully" << std::endl;

	// Load gesture database
	hr = CreateVisualGestureBuilderDatabaseInstanceFromFile(
		L"C:\\Users\\micro\\source\\repos\\Ichmagkase\\Kinematics\\kinectbridge\\x64\\Debug\\gestures.gbd",
		&pGestureDatabase
	);
	CheckError(hr, "CreateVisualGestureBuilderDatabaseInstanceFromFile");

	hr = pGestureDatabase->get_AvailableGesturesCount(&gestureCount);
	CheckError(hr, "IVisualGestureBuilderDatabase::get_AvailableGesturesCount");

	pGestures = new IGesture * [gestureCount];
	hr = pGestureDatabase->get_AvailableGestures(gestureCount, pGestures);
	CheckError(hr, "IVisualGestureBuilderDatabase::get_AvailableGestures");

	IBody* ppBodies[BODY_COUNT] = { nullptr };
	UINT64 player1_id = 0;

	// Set up gesture source and reader (tracking ID will be updated in the loop)
	hr = CreateVisualGestureBuilderFrameSource(pSensor, 0, &gestureSource);
	CheckError(hr, "CreateVisualGestureBuilderFrameSource");

	gestureSource->AddGestures(gestureCount, pGestures);
	gestureSource->OpenReader(&gestureReader);

	std::cout << "Entering main loop" << std::endl;

	// Main gesture detection loop
	while (true) {
		// Re-acquire body frame each tick to keep player1_id fresh
		IBodyFrame* pBodyFrame = nullptr;
		hr = pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
		if (hr == E_PENDING || !pBodyFrame) {
			Sleep(33);
			continue;
		}

		hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBodies);
		pBodyFrame->Release();
		if (FAILED(hr)) continue;

		// Find the first tracked body
		player1_id = 0;
		for (int i = 0; i < BODY_COUNT; ++i) {
			BOOLEAN bTracked = false;
			if (ppBodies[i]) {
				ppBodies[i]->get_IsTracked(&bTracked);
				if (bTracked) {
					ppBodies[i]->get_TrackingId(&player1_id);
					break;
				}
			}
		}

		if (player1_id == 0) {
			std::cout << "No player tracked, waiting..." << std::endl;
			Sleep(33);
			continue;
		}

		// Update gesture source with current tracking ID
		gestureSource->put_TrackingId(player1_id);

		IVisualGestureBuilderFrame* pGestureFrame = nullptr;
		hr = gestureReader->CalculateAndAcquireLatestFrame(&pGestureFrame);
		if (hr == E_PENDING || !pGestureFrame) {
			Sleep(33);
			continue;
		}

		CheckError(hr, "IVisualGestureBuilderFrameReader::CalculateAndAcquireLatestFrame");

		BOOLEAN isTrackingIdValid = false;
		pGestureFrame->get_IsTrackingIdValid(&isTrackingIdValid);
		std::cout << "Player (ID=" << player1_id << ") isTrackingIdValid=" << isTrackingIdValid << std::endl;

		if (!isTrackingIdValid) {
			pGestureFrame->Release();
			continue;
		}

		BOOLEAN gesturing = false;

		for (UINT j = 0; j < gestureCount; ++j) {
			std::cout << "help" << std::endl;
			std::cout << "Checking gesture " << j << " for player " << player1_id << std::endl;

			if (gesturing) break; // already detected one this frame, skip the rest

			GestureType gestureType;
			pGestures[j]->get_GestureType(&gestureType);

			wchar_t gestureName[256];
			pGestures[j]->get_Name(256, gestureName);
			std::wprintf(L"Gesture: %s, Type: %s\n", gestureName,
				gestureType == GestureType_Discrete ? L"Discrete" : L"Continuous");

			IDiscreteGestureResult* pGestureResult = nullptr;
			hr = pGestureFrame->get_DiscreteGestureResult(pGestures[j], &pGestureResult);
			CheckError(hr, "IVisualGestureBuilderFrame::get_DiscreteGestureResult");

			if (pGestureResult) {
				BOOLEAN isDetected = false;
				float confidence = 0.0f;

				pGestureResult->get_Detected(&isDetected);
				pGestureResult->get_Confidence(&confidence);

				std::wprintf(L"Gesture: %s\n Player: %llu\n Confidence: %f\n\n",
					gestureName, player1_id, confidence);

				if (isDetected && confidence > 0.01f) {
					std::wprintf(L"Detected gesture: %s (confidence: %.2f) from player %llu\n",
						gestureName, confidence, player1_id);
					gesturing = true;
				}

				pGestureResult->Release();
			}
		}

		if (!gesturing) {
			std::cout << "Player idle: " << player1_id << std::endl;
		}

		pGestureFrame->Release();
		Sleep(33);
	}
}


int main() {
	//Sensor sensor;

	//// AWAIT PLAYERS READY ... Do this once to determine relative position
	//std::array<UINT64, 2> players;
	//Sleep(3);
	//players = sensor.awaitPlayersReady();
	//sensor.listen(&GestureListener, players);
	debugGestures();
}