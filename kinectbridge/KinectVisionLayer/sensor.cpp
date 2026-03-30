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

void CheckError(HRESULT hr, std::string message) {
	if (FAILED(hr)) {
		std::cerr << message << ": " << GetHRESULTErrorMessage(hr) << std::endl;
		exit(1);
	}
}

Sensor::Sensor() : pBodyFrameReader(nullptr), pBodyFrameSource(nullptr), pSensor(nullptr), 
	pGestureFrameSource(nullptr), pGestureFrameReader(nullptr), pGestureDatabase(nullptr),
	pGestures(nullptr), gestureCount(0) {
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

	// load gesture database
	IVisualGestureBuilderDatabase* pGestureDatabase = nullptr;
	hr = CreateVisualGestureBuilderDatabaseInstanceFromFile(L"C:\\Users\\micro\\source\\repos\\Ichmagkase\\Kinematics\\kinectbridge\\x64\\Debug\\gestures.gbd", &pGestureDatabase);
	CheckError(hr, "CreateVisualGestureBuilderDatabaseInstanceFromFile");

	gestureCount = 0;
	hr = pGestureDatabase->get_AvailableGesturesCount(&gestureCount);
	CheckError(hr, "IVisualGestureBuilderDatabase::get_AvailableGesturesCount");
	
	IGesture **gestures = new IGesture* [gestureCount];
	hr = pGestureDatabase->get_AvailableGestures(gestureCount, gestures);
	CheckError(hr, "IVisualGestureBuilderDatabase::get_AvailableGestures");

	// Store database and clean up gestures array (but not individual gestures - they're referenced by the source)
	this->pGestureDatabase = pGestureDatabase;
	this->pGestures = gestures;
	this->gestureCount = gestureCount;
}

/**
 * Wait for 2 tracked players and return them
 * Also handle relative position of players here
 */
std::array<UINT64, 2> Sensor::awaitPlayersReady() {

	HRESULT hr;
	IBody* ppBodies[BODY_COUNT] = { nullptr };
	int detectedCount = 0;
	std::array<UINT64, 2> players = {};
	IBody* playerBodies[2] = { nullptr };

	while (detectedCount < 2) {
		IBodyFrame* pBodyFrame = nullptr;
		hr = pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
		if (hr == E_PENDING) {
			Sleep(60);
			continue;
		}
		std::cout << "Waiting for 2 players..." << std::endl;

		if (!pBodyFrame) {
			std::cerr << "pBodyFrame is null" << std::endl;
			continue;
			// exit(1);
		}

		hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBodies);
		CheckError(hr, "IBodyFrame::GetAndRefreshBodyData");
		
		detectedCount = 0;
		playerBodies[0] = nullptr;
		playerBodies[1] = nullptr;
		players[0] = 0;
		players[1] = 0;

		for (int i = 0; i < BODY_COUNT; ++i) {
			if (detectedCount >= 2) break;

			BOOLEAN bTracked = false;
			if (ppBodies[i]) {
				hr = ppBodies[i]->get_IsTracked(&bTracked);
				CheckError(hr, "IBody::getIsTracked");
			}
			
			if (bTracked) {
				UINT64 trackingId;
				hr = ppBodies[i]->get_TrackingId(&trackingId);
				CheckError(hr, "IBody::get_TrackingId");
				playerBodies[detectedCount] = ppBodies[i];
				players[detectedCount] = trackingId;
				detectedCount++;
			}
		}
		pBodyFrame->Release();
	}

	// now sort the 2 players by position from left to right
	Joint player1_joints[JointType_Count];
	if (FAILED(playerBodies[0]->GetJoints(JointType_Count, player1_joints))) {}
	float player1_head_x = player1_joints[JointType_Head].Position.X;

	Joint player2_joints[JointType_Count];
	if (FAILED(playerBodies[1]->GetJoints(JointType_Count, player2_joints))) {}
	float player2_head_x = player2_joints[JointType_Head].Position.X;

	// Sort left most player (rightmost from camera perspective) to be first
	if (player1_head_x < player2_head_x) {
		UINT64 tmp = players[0];
		players[0] = players[1];
		players[1] = tmp;
	}

	return players;
}

/**
 * Refresh players array in place
 */
std::array<IBody*, 2> Sensor::refreshAndGetPlayers(std::array<UINT64, 2> & players) {
	IBody* ppBodies[BODY_COUNT] = {};
	HRESULT hr;
	IBodyFrame* pBodyFrame = nullptr;
	hr = pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
	while (hr == E_PENDING) {
		Sleep(60);
		hr = pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
		continue;
	}
	// CheckError(hr, "IBodyFrameReader::AcquireLatestFrame");
	hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBodies);
	CheckError(hr, "IBodyFrame::GetAndRefreshBodyData");

	std::array<IBody*, 2> playerBodies = { };

	BOOLEAN player1_IsTracked = false;
	BOOLEAN player2_IsTracked = false;
	int extraCount = 0;
	UINT64 extraBodyTrackingIds[] = { 0, 0 };
	// For every body this frame
	for (int i = 0; i < BODY_COUNT; ++i) {
		UINT64 thisTrackingId;
		// if body is not null
		if (ppBodies[i]) {
			BOOLEAN bTracked;
			hr = ppBodies[i]->get_IsTracked(&bTracked);
			CheckError(hr, "IBody::get_IsTracked");
			// ... and body is tracked
			if (bTracked) {
				// Get its trackingID
				hr = ppBodies[i]->get_TrackingId(&thisTrackingId);
				CheckError(hr, "IBody::get_TrackingId");
				// Check if its an existing tracked player
				if (thisTrackingId == players[0]) {
					playerBodies[0] = ppBodies[i];
					player1_IsTracked = true;
				}
				else if (thisTrackingId == players[1]) {
					playerBodies[1] = ppBodies[i];
					player2_IsTracked = true;
				}
				else {
					extraBodyTrackingIds[extraCount++] = thisTrackingId;
				}
			}
		}
	}

	// if neither players are present, get 2 new players
	if (!player1_IsTracked && !player2_IsTracked) {
		players = awaitPlayersReady();
	}
	else {
		// Fill in missing player gaps
		if (!player1_IsTracked) {
			if (extraCount == 0) {
				players[0] = 0;
			}
			players[0] = extraBodyTrackingIds[--extraCount];
		}
		if (!player2_IsTracked) {
			if (extraCount == 0) {
				players[1] = 0;
			}
			players[1] = extraBodyTrackingIds[extraCount];
		}
	}

	pBodyFrame->Release();

	return playerBodies;
}

/**
 * Ensure players do not go out of bounds and detect gestures
 */
void Sensor::listen(void(*GestureCallback)(struct Data), std::array<UINT64, 2> players) {
	std::cout << "Listening for persons..." << std::endl;

	// Detection loop
	int frameCount = 0;
	HRESULT hr;
	// Track which bodies we've already reported to avoid duplicate callbacks
	// std::map<UINT64, IBody*> currentTrackedBodies;
	IBody* ppBodies[BODY_COUNT] = { nullptr };

	IVisualGestureBuilderFrameSource* gestureSources[2] = { nullptr, nullptr };
	IVisualGestureBuilderFrameReader* gestureReaders[2] = { nullptr, nullptr };

	for (int i = 0; i < 2; i++) {
		hr = CreateVisualGestureBuilderFrameSource(pSensor, players[i], &gestureSources[i]);
		CheckError(hr, "CreateVisualGestureBuilderFrameSource");
		gestureSources[i]->AddGestures(gestureCount, pGestures);
		gestureSources[i]->OpenReader(&gestureReaders[i]);
	}

	while (true) {
		refreshAndGetPlayers(players);
		Sleep(33);

		// Keep gesture sources in sync with current tracking IDs
		for (int i = 0; i < 2; i++) {
			gestureSources[i]->put_TrackingId(players[i]);
		}
		std::cout << "Current tracked players: " << ((players[0] != 0) ? std::to_string(players[0]) : "None") 
			<< ", " << ((players[1] != 0) ? std::to_string(players[1]) : "None") << std::endl;

		// for (auto const& [BodyId, Body] : currentTrackedBodies) {
		for (int i = 0; i < 2; i++) {
			std::cout << "Checking gestures for player " << ((players[i] != 0) ? std::to_string(players[i]) : "None") << std::endl;
			IVisualGestureBuilderFrame* pGestureFrame = nullptr;
			hr = gestureReaders[i]->CalculateAndAcquireLatestFrame(&pGestureFrame);
			if (hr == E_PENDING || !pGestureFrame) continue; // not ready this tick, skip

			CheckError(hr, "IVisualGestureBuilderFrameReader::CalculateAndAcquireLatestFrame");

			if (!pGestureFrame) {
				std::cerr << "pGestureFrame is null" << std::endl;
				exit(1);
			}

			BOOLEAN gesturing = false;

			for (UINT j = 0; j < gestureCount; ++j) {
				std::cout << "Checking gesture " << j << " for player " << ((players[i] != 0) ? std::to_string(players[i]) : "None") << std::endl;
				if (gesturing) {
					break;
				}

				IDiscreteGestureResult* pGestureResult = nullptr;
				hr = pGestureFrame->get_DiscreteGestureResult(pGestures[j], &pGestureResult);
				CheckError(hr, "IVisualGestureBuilderFrame::get_DiscreteGestureResult");

				if (pGestureResult) {
					BOOLEAN isDetected = false;
					float confidence = 0.0f;

					pGestureResult->get_Detected(&isDetected);
					pGestureResult->get_Confidence(&confidence);

					if (isDetected && confidence > 0.5f) {
						UINT capacity = 256;
						wchar_t gestureName[256];
						pGestures[j]->get_Name(capacity, gestureName);

						std::wprintf(L"Detected gesture: %s (confidence: %.2f) from player %llu\n", gestureName, confidence, players[i]);
						gesturing = true;
					}

					pGestureResult->Release();
				}

				if (!gesturing) {
					std::cout << "No gesture detected for player " << ((players[i] != 0) ? std::to_string(players[i]) : "None") << std::endl;
					// TODO: player is idle
				}
			}

			pGestureFrame->Release();
		}
			
		// }

		// Update tracked bodies for next frame
		// Print status every 30 frames
		//if (frameCount % 30 == 0) {
		//	std::cout << "Frame " << frameCount << ": " << ((currentTrackedBodies.size() > 0) ? "PERSON PRESENT" : "No person")
		//		<< " (Tracking " << currentTrackedBodies.size() << " bodies)" << std::endl;
		//}

		// pBodyFrame->Release();

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
