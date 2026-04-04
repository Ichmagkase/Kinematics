#include "NewSensor.h"
#include "util.h"
#include <vector>

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
	ERROR_CHECK(CreateVisualGestureBuilderDatabaseInstanceFromFile(L"gestures.gbd", &pGestureDatabase));

	UINT gestureCount = 0;
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
	ERROR_CHECK(pBodyFrameReader->AcquireLatestFrame(&pBodyFrame));

	if (!pBodyFrame) {
		// Handle null body frame
	}

	ERROR_CHECK(pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBodies));
	pBodyFrame->Release();

	// Dont zero these. We need to know if they are tracked from the last frame using these same IDs. 
	// Keep booleans instead. Update later as necessary
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
				}
				else if (players.player2_id == 0) {
					isPlayer2Tracked = true;
				}
				else {
					tracked_nonplayers.push_back(trackingId);
				}
			}
		}
	} 

	// at this point we have 2 players tracked. 

if (players.player1_id == 0 && players.player2_id == 0) {
  Sensor::awaitPlayers();
} else {
  if (players.player1_id == 0) {
    if (tracked_nonplayers.size() > 0) {
      players.player1_id = tracked_nonplayers.pop_off();
    }
    
  }
  if (players.player2_id == 0) {
    if (tracked_nonplayers.size() > 0) {
      players.player1_id = tracked_nonplayers.pop_off();
    }
  }
}

	players.player1_GestureSource->put_TrackingId(players.player1_id);
	players.player2_GestureSource->put_TrackingId(players.player2_id);
}

void Sensor::updatePlayerGestures() {

}

void Sensor::awaitPlayers(Players *players) {
	
}

// ----- RUN -----

Sensor::Sensor()
{
	initialize();
}

Sensor::~Sensor()
{
	// TODO
}

void Sensor::listen()
{
	awaitPlayers(&players);

	while (true) {
		update();
		
		// Implement exit logic here later
	}
}