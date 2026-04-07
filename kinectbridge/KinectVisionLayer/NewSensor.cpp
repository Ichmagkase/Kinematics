#include "sensor.h"
#include "util.h"
#include "structures.h"
#include <vector>
#define GESTURE_NAME_SIZE 260
// ----- INIT -----

void Sensor::initialize() 
{
	initializeSensor();
	initializeBody();
	initializeGesture();
}

void Sensor::initializeSensor() 
{
	ERROR_CHECK(GetDefaultKinectSensor(&kinect));
	ERROR_CHECK(kinect->Open());
}

void Sensor::initializeBody() 
{
	ERROR_CHECK(kinect->get_BodyFrameSource(&pBodyFrameSource));
	ERROR_CHECK(pBodyFrameSource->OpenReader(&pBodyFrameReader));
}

void Sensor::initializeGesture() 
{
	ERROR_CHECK(CreateVisualGestureBuilderDatabaseInstanceFromFile(L"gestures_updated.gbd", &pGestureDatabase));

	ERROR_CHECK(pGestureDatabase->get_AvailableGesturesCount(&gestureCount));

	pGestures = new IGesture * [gestureCount];
	ERROR_CHECK(pGestureDatabase->get_AvailableGestures(gestureCount, pGestures));

	ERROR_CHECK(CreateVisualGestureBuilderFrameSource(kinect, players.player1_id, &players.player1_GestureSource));
	ERROR_CHECK(CreateVisualGestureBuilderFrameSource(kinect, players.player2_id, &players.player2_GestureSource));

	ERROR_CHECK(players.player1_GestureSource->AddGestures(gestureCount, pGestures));
	ERROR_CHECK(players.player2_GestureSource->AddGestures(gestureCount, pGestures));

	for (UINT i = 0; i < gestureCount; ++i) {
		players.player1_GestureSource->SetIsEnabled(pGestures[i], true);
		players.player2_GestureSource->SetIsEnabled(pGestures[i], true);
	}

	ERROR_CHECK(players.player1_GestureSource->OpenReader(&players.player1_GestureReader));
	ERROR_CHECK(players.player2_GestureSource->OpenReader(&players.player2_GestureReader));
}

// ----- UPDATE -----

void Sensor::update() 
{
	updatePlayerBodies();
	updatePlayerGestures();
}

void Sensor::updatePlayerBodies() {
	IBodyFrame* pBodyFrame = nullptr;
	HRESULT hr = pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
	while (hr == E_PENDING) {
		Sleep(60);
		hr = pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
	}

	if (!pBodyFrame) {
		std::cout << "No body frame available" << std::endl;
		updatePlayerBodies();
	}

	ERROR_CHECK(pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBodies));
	pBodyFrame->Release();

	// Dont zero these. We need to know if they are tracked from the last frame using these same IDs. 
	// Keep booleans instead. Update later as necessary
	IBody* player1 = nullptr;
	IBody* player2 = nullptr;
	bool isPlayer1Tracked = false;
	bool isPlayer2Tracked = false;
	std::vector<UINT64> tracked_nonplayers;
	for (int i = 0; i < BODY_COUNT; ++i) {
		BOOLEAN bTracked = false;
		if (ppBodies[i]) {
			ppBodies[i]->get_IsTracked(&bTracked);
			if (bTracked) {
				UINT64 trackingId = 0;
				ppBodies[i]->get_TrackingId(&trackingId);
				if (players.player1_id == trackingId) {
					// we have identified this as player 1
					isPlayer1Tracked = true;
					player1 = ppBodies[i];
				}
				else if (players.player2_id == trackingId) {
					isPlayer2Tracked = true;
					player2 = ppBodies[i];
				}
				else {
					tracked_nonplayers.push_back(trackingId);
				}
			}
		}
	} 

	// at this point we have 2 players tracked. 

	if (players.player1_id == 0 && players.player2_id == 0) {
	  Sensor::awaitPlayersReady();
	}
	else {
		if (players.player1_id == 0) {
			if (tracked_nonplayers.size() > 0) {
				players.player1_id = tracked_nonplayers.back();
				tracked_nonplayers.pop_back();
			}
		}
		if (players.player2_id == 0) {
			if (tracked_nonplayers.size() > 0) {
				players.player1_id = tracked_nonplayers.back();
				tracked_nonplayers.pop_back();
			}
		}
	}

	if (player1) {
		Joint player1_joints[JointType_Count] = {};
		if (FAILED(player1->GetJoints(JointType_Count, player1_joints))) {
			std::cerr << "Failed to get joints for player 1" << std::endl;
		}
		float player1_y = player1_joints[JointType_Head].Position.Y;
		if (player1_y > players.player1_height + 0.20f) {
			gestureListener({ Event::JUMP, "1" });
		}
	}
	
	if (player2) {
		Joint player2_joints[JointType_Count] = {};
		if (FAILED(player2->GetJoints(JointType_Count, player2_joints))) {
			std::cerr << "Failed to get joints for player 2" << std::endl;
		}
		float player2_y = player2_joints[JointType_Head].Position.Y;
		if (player2_y > players.player2_height + 0.20f) {
			gestureListener({ Event::JUMP, "1" });
		}
	}
	
	players.player1_GestureSource->put_TrackingId(players.player1_id);
	players.player2_GestureSource->put_TrackingId(players.player2_id);
}

void Sensor::updatePlayerGestures() {
	IVisualGestureBuilderFrameReader* activePlayers[2] = {players.player1_GestureReader, players.player2_GestureReader};
	for (int i = 0; i < 2; ++i) {
		IVisualGestureBuilderFrame* pGestureFrame = nullptr;
		activePlayers[i]->CalculateAndAcquireLatestFrame(&pGestureFrame);

		if (pGestureFrame) {
			BOOLEAN isPlayer1TrackingIdValid = false;
			pGestureFrame->get_IsTrackingIdValid(&isPlayer1TrackingIdValid);
			if (!isPlayer1TrackingIdValid) {
				pGestureFrame->Release();
				pGestureFrame = nullptr;
				continue;
			}
			bool gesturing = false;
			for (UINT j = 0; j < gestureCount; ++j) {
				if (gesturing) break;

				IDiscreteGestureResult* pGestureResult = nullptr;
				pGestureFrame->get_DiscreteGestureResult(pGestures[j], &pGestureResult);

				if (pGestureResult == nullptr) {
					std::cerr << "pGestureResult is null for player " << i << " gesture " << j << std::endl;
					continue;
				}

				BOOLEAN isDetected = false;
				float confidence = 0.0f;
				pGestureResult->get_Detected(&isDetected);
				pGestureResult->get_Confidence(&confidence);

				if (isDetected && confidence > 0.5f) {
					// 1. Safely allocate the wide character buffer
					wchar_t gestureNameW[GESTURE_NAME_SIZE] = { 0 };

					// 2. Fetch the name and capture the HRESULT to ensure it actually succeeds
					// REFACTOR: this call explicitly uses 260. use a variable or get the size of gestureNameW
					ERROR_CHECK( pGestures[j]->get_Name(GESTURE_NAME_SIZE, gestureNameW));

					// 3. Convert the wide string to a standard narrow string
					std::wstring ws(gestureNameW);
					std::string gestureNameStr(ws.begin(), ws.end());

					if (gestureNameStr == "punch_Left") {
						gestureListener({ Event::PUNCH_L, std::to_string(i + 1) });
					}
					else if (gestureNameStr == "punch_Right") {
						gestureListener({ Event::PUNCH_R, std::to_string(i + 1) });
					}
					else if (gestureNameStr == "move_Right") {
						gestureListener({ Event::MOVE_R, std::to_string(i + 1) });
					}
					else if (gestureNameStr == "move_Left") {
						gestureListener({ Event::MOVE_L, std::to_string(i + 1) });
					}
					else if (gestureNameStr == "block") {
						gestureListener({ Event::BLOCK, std::to_string(i + 1) });
					}

					// 4. Print using the exact same std::cout you use everywhere else
					gesturing = true;
				}
				pGestureResult->Release();
			}

			if (!gesturing) {
				gestureListener({ Event::IDLE, std::to_string(i + 1) });
			}
			pGestureFrame->Release();
		}
	}
}

void Sensor::awaitPlayersReady() {
	int detectedCount = 0;
	IBody* player1 = nullptr;
	IBody* player2 = nullptr;
	IBodyFrame* pBodyFrame = nullptr;
	while (detectedCount < 2) {
		HRESULT hr = pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
		if (hr == E_PENDING) {
			Sleep(30);
			continue;
		} else if (FAILED(hr)) {
			ERROR_CHECK(hr);
		}
		ERROR_CHECK(pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBodies));
		detectedCount = 0;
		player1 = nullptr; 
		player2 = nullptr;
		for (int i = 0; i < BODY_COUNT; ++i) {
			BOOLEAN bTracked;
			if (ppBodies[i]) {
				ERROR_CHECK(ppBodies[i]->get_IsTracked(&bTracked));
				if (bTracked) {

					if (player1 == nullptr) {
						player1 = ppBodies[i];
					} else if (player2 == nullptr) {
						player2 = ppBodies[i];
					}
					detectedCount++;
				}
			}
		}
		pBodyFrame->Release();
	}

	// now sort the 2 players by position from left to right
	Joint player1_joints[JointType_Count] = {};
	if (FAILED(player1->GetJoints(JointType_Count, player1_joints))) {
		std::cerr << "Failed to get joints for player 1" << std::endl;
	}
	float player1_head_x = player1_joints[JointType_Head].Position.X;
	players.player1_height = player1_joints[JointType_Head].Position.Y;

	Joint player2_joints[JointType_Count] = {};
	if (FAILED(player2->GetJoints(JointType_Count, player2_joints))) {
		std::cerr << "Failed to get joints for player 2" << std::endl;
	}
	float player2_head_x = player2_joints[JointType_Head].Position.X;
	players.player2_height = player2_joints[JointType_Head].Position.Y;

	// Sort left most player (rightmost from camera perspective) to be first
	if (player1_head_x < player2_head_x) {
		player1->get_TrackingId(&players.player1_id); 
		player2->get_TrackingId(&players.player2_id);
	}
	else {
		player1->get_TrackingId(&players.player2_id); 
		player2->get_TrackingId(&players.player1_id);
	}

	// gestureListener({ Event::P1_READY, "1" });
	// gestureListener({ Event::P2_READY, "2" });
}

// ----- RUN -----

Sensor::Sensor(void (*gestureListener)(struct GestureData))
{
	this->gestureListener = gestureListener;
	initialize();
}

Sensor::~Sensor()
{
	// Release all resources
	for (int i = 0; i < BODY_COUNT; ++i) {
		if (ppBodies[i]) {
			ppBodies[i]->Release();
			ppBodies[i] = nullptr;
		}
	}

	players.player1_GestureReader->Release();
	players.player2_GestureReader->Release();
	players.player1_GestureSource->Release();
	players.player2_GestureSource->Release();
	pBodyFrameSource->Release();
	pBodyFrameReader->Release();
	pGestureDatabase->Release();
	kinect->Close();
}

void Sensor::listen()
{
	while (true) {
		update();
		Sleep(30);
	}
}