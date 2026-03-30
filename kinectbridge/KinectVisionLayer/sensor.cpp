#include <iostream>
#include <string>
#include <functional>
#include <map>
#include <Kinect.h>
#include <Kinect.VisualGestureBuilder.h>
#include "sensor.h"
#include <windows.h>

#pragma comment(lib, "Kinect20.lib")
#pragma comment(lib, "Kinect20.VisualGestureBuilder.lib")

#define MAX_PLAYERS 2

std::string GetHRESULTErrorMessage(HRESULT hr)
{
	char* errorMsg = nullptr;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&errorMsg,
		0,
		nullptr
	);

	std::string message = errorMsg ? errorMsg : "Unknown error";
	LocalFree(errorMsg);
	return message;
}

Sensor::Sensor() : pBodyFrameReader(nullptr), pBodyFrameSource(nullptr), pSensor(nullptr), 
	pGestureFrameSource(nullptr), pGestureFrameReader(nullptr), pGestureDatabase(nullptr),
	pGestures(nullptr), gestureCount(0) {
	std::cout << "Kinect Person Detection Program" << std::endl;

	// Get the default Kinect sensor
	if (FAILED(GetDefaultKinectSensor(&pSensor))) {
		std::cerr << "Failed to get default Kinect sensor" << std::endl;
		exit(1);
	}

	if (!pSensor) {
		std::cerr << "No Kinect sensor found" << std::endl;
		exit(1);
	}

	// Open the sensor
	if (FAILED(pSensor->Open())) {
		std::cerr << "Failed to open Kinect sensor" << std::endl;
		pSensor->Release();
		exit(1);
	}

	// Get the body frame source
	if (FAILED(pSensor->get_BodyFrameSource(&pBodyFrameSource))) {
		std::cerr << "Failed to get body frame source" << std::endl;
		pSensor->Release();
		exit(1);
	}

	// Create body frame reader
	if (FAILED(pBodyFrameSource->OpenReader(&pBodyFrameReader))) {
		std::cerr << "Failed to open body frame reader" << std::endl;
		pBodyFrameSource->Release();
		pSensor->Release();
		exit(1);
	}

	std::cout << "Kinect sensor initialized successfully" << std::endl;

	// load gesture database
	IVisualGestureBuilderDatabase* pGestureDatabase = nullptr;

	if (FAILED(CreateVisualGestureBuilderDatabaseInstanceFromFile(L"C:\\Users\\micro\\source\\repos\\Ichmagkase\\Kinematics\\kinectbridge\\x64\\Debug\\gestures.gbd", &pGestureDatabase))) {
		std::cerr << GetHRESULTErrorMessage(CreateVisualGestureBuilderDatabaseInstanceFromFile(L"gestures.gbd", &pGestureDatabase));
		exit(1);
	}

	gestureCount = 0;
	if (FAILED(pGestureDatabase->get_AvailableGesturesCount(&gestureCount))) {
		std::cerr << "Failed to get gesture count" << std::endl;
		pGestureDatabase->Release();
		pBodyFrameSource->Release();
		pSensor->Release();
		exit(1);
	}
	
	IGesture **gestures = new IGesture* [gestureCount];
	pGestureDatabase->get_AvailableGestures(gestureCount, gestures);

	// Store database and clean up gestures array (but not individual gestures - they're referenced by the source)
	this->pGestureDatabase = pGestureDatabase;
	this->pGestures = gestures;
	this->gestureCount = gestureCount;
}

/**
 * Wait for 2 tracked players and return them
 * Also handle relative position of players here
 */
std::array<IBody*, 2> Sensor::awaitPlayersReady() {
	std::cout << "Waiting for 2 players..." << std::endl;

	IBody* ppBodies[BODY_COUNT] = { nullptr };
	int detectedCount = 0;
	std::array<IBody*, 2> players = {};

	while (detectedCount < 2) {
		IBodyFrame* pBodyFrame = nullptr;
		if (FAILED(pBodyFrameReader->AcquireLatestFrame(&pBodyFrame)) && pBodyFrame) {}
		if (FAILED(pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBodies))) {}

		detectedCount = 0;
		players[0] = nullptr;
		players[1] = nullptr;

		for (int i = 0; i < BODY_COUNT; ++i) {
			BOOLEAN bTracked = false;
			if (ppBodies[i])
				if (FAILED(ppBodies[i]->get_IsTracked(&bTracked))) {}
			
			if (bTracked) {
				players[detectedCount] = ppBodies[i];
				detectedCount++;
			}
		}
		pBodyFrame->Release();
	}

	// now sort the 2 players by position from left to right
	Joint player1_joints[JointType_Count];
	if (FAILED(players[0]->GetJoints(JointType_Count, player1_joints))) {}
	float player1_head_x = player1_joints[JointType_Head].Position.X;

	Joint player2_joints[JointType_Count];
	if (FAILED(players[1]->GetJoints(JointType_Count, player2_joints))) {}
	float player2_head_x = player2_joints[JointType_Head].Position.X;

	// Sort left most player (rightmost from camera perspective) to be first
	if (player1_head_x < player2_head_x) {
		IBody* tmp = players[0];
		players[0] = players[1];
		players[1] = tmp;
	}

	return players;
}

/**
 * Ensure players do not go out of bounds and detect gestures
 */
void Sensor::listen(void(*GestureCallback)(struct Data), std::array<IBody*, 2> players) {
	std::cout << "Listening for persons..." << std::endl;

	// Detection loop
	bool bPersonDetected = false;
	int frameCount = 0;

	// Track which bodies we've already reported to avoid duplicate callbacks
	std::map<UINT64, IBody*> currentTrackedBodies;

	IBody* ppBodies[BODY_COUNT] = { nullptr };

	while (true) {
		IBodyFrame* pBodyFrame = nullptr;

		// Get the latest body frame
		if (FAILED(pBodyFrameReader->AcquireLatestFrame(&pBodyFrame)) && pBodyFrame) {}

		// Get the body data
		if (FAILED(pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBodies))) {}

		bPersonDetected = false;

		// TODO 1: Populate currentTrackedBodies with tracked IDs:Bodies of players
		for (int i = 0; i < BODY_COUNT; ++i) {
			if (ppBodies[i]) {
				BOOLEAN bTracked = false;
				if (SUCCEEDED(ppBodies[i]->get_IsTracked(&bTracked)) && bTracked) {
					UINT64 trackingId = 0;
					if (SUCCEEDED(ppBodies[i]->get_TrackingId(&trackingId)) && trackingId) {
						currentTrackedBodies[trackingId] = ppBodies[i];
					}
				}

			}
		}

		// TODO 2: Iterate over currentTrackedBodies, calculate gesture, determine position with body data
		// TODO: You can now use this known tracking data to find the gestures of each 
		// TODO: Pull assignments out of conditions to pull HRESULT and print better error messages

		for (auto const& [BodyId, Body] : currentTrackedBodies) {
			if (FAILED(CreateVisualGestureBuilderFrameSource(pSensor, BodyId, &pGestureFrameSource))) {}

			if (FAILED(pGestureFrameSource->AddGestures(gestureCount, pGestures))) {}

			if (FAILED(pGestureFrameSource->OpenReader(&pGestureFrameReader))) {}

			IVisualGestureBuilderFrame* pGestureFrame = nullptr;
			if (FAILED(pGestureFrameReader->CalculateAndAcquireLatestFrame(&pGestureFrame)) && pGestureFrame) {}

			BOOLEAN gesturing = false; 

			for (int i = 0; i < gestureCount; ++i) {
				if (gesturing) {
					break;
				}

				IDiscreteGestureResult* pGestureResult = nullptr;
				if (FAILED(pGestureFrame->get_DiscreteGestureResult(pGestures[i], &pGestureResult))) {}

				if (pGestureResult) {
					BOOLEAN isDetected = false;
					float confidence = 0.0f;

					pGestureResult->get_Detected(&isDetected);
					pGestureResult->get_Confidence(&confidence);

					if (isDetected && confidence > 0.5f) {
						UINT capacity = 256;
						wchar_t gestureName[256];
						pGestures[i]->get_Name(capacity, gestureName);

						std::wprintf(L"Detected gesture: %s (confidence: %.2f) from player %llu\n", gestureName, confidence, BodyId);
						gesturing = true;
					}

					pGestureResult->Release();
				}

				if (!gesturing) {
					// TODO: player is idle
				}
			}

			pGestureFrame->Release();
		}

		// Update tracked bodies for next frame
		// Print status every 30 frames
		if (frameCount % 30 == 0) {
			std::cout << "Frame " << frameCount << ": " << ((currentTrackedBodies.size() > 0) ? "PERSON PRESENT" : "No person")
				<< " (Tracking " << currentTrackedBodies.size() << " bodies)" << std::endl;
		}

		pBodyFrame->Release();

		frameCount++;
		Sleep(33);  // ~30 FPS FOR DEBUG PURPOSES ONLY
	}
}

Sensor::~Sensor() {
	// Cleanup
	if (pGestureFrameReader) {
		pGestureFrameReader->Release();
	}
	if (pGestureFrameSource) {
		pGestureFrameSource->Release();
	}
	if (pGestureDatabase) {
		pGestureDatabase->Release();
	}
	if (pBodyFrameReader) {
		pBodyFrameReader->Release();
	}
	if (pBodyFrameSource) {
		pBodyFrameSource->Release();
	}
	if (pSensor) {
		pSensor->Close();
		pSensor->Release();
	}

	if (pGestures) {
		for (UINT i = 0; i < gestureCount; i++) {
			if (pGestures[i]) pGestures[i]->Release();
		}
		delete[] pGestures;
	}


	std::cout << "Kinect sensor closed" << std::endl;
}
