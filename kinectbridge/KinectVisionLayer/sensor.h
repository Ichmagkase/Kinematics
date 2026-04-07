#pragma once

#include <Kinect.h>
#include <Kinect.VisualGestureBuilder.h>
#include "structures.h"

typedef struct Players {
	UINT64 player1_id = 0;
	UINT64 player2_id = 0;
	float player1_height = 0;
	float player2_height = 0;
	IVisualGestureBuilderFrameSource* player1_GestureSource;
	IVisualGestureBuilderFrameReader* player1_GestureReader;
	IVisualGestureBuilderFrameSource* player2_GestureSource;
	IVisualGestureBuilderFrameReader* player2_GestureReader;
} Players;

class Sensor {
public:
	Sensor(void (*gestureListener)(struct GestureData));
	~Sensor();
	void awaitPlayersReady();
	void listen();
	void (*gestureListener)(struct GestureData);
private:
	void initialize();
	void initializeSensor();
	void initializeBody();
	void initializeGesture();
	void update();
	void updatePlayerBodies();
	void updatePlayerGestures();
private:
	IKinectSensor* kinect;
	IBodyFrameSource* pBodyFrameSource;
	IBodyFrameReader* pBodyFrameReader;
	IVisualGestureBuilderDatabase* pGestureDatabase;
	
	UINT gestureCount = 0;
	IGesture** pGestures;
	Players players;
	IBody* ppBodies[BODY_COUNT] = { nullptr };
};