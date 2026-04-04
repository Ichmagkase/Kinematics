#pragma once

#include <Kinect.h>
#include <Kinect.VisualGestureBuilder.h>

typedef struct Players {
	UINT64 player1_id = 0;
	UINT64 player2_id = 0;
	IVisualGestureBuilderFrameSource* player1_GestureSource;
	IVisualGestureBuilderFrameReader* player1_GestureReader;
	IVisualGestureBuilderFrameSource* player2_GestureSource;
	IVisualGestureBuilderFrameReader* player2_GestureReader;
} Players;

class Sensor {
public:
	Sensor();
	~Sensor();
	void listen();
private:
	void initialize();
	void initializeSensor();
	void initializeBody();
	void initializeGesture();
	void update();
	void updatePlayerBodies();
	void updatePlayerGestures();
	void awaitPlayers(Players *players);
private:
	IKinectSensor* kinect;
	IBodyFrameSource* pBodyFrameSource;
	IBodyFrameReader* pBodyFrameReader;
	IVisualGestureBuilderDatabase* pGestureDatabase;
	
	UINT gestureCount;
	IGesture** pGestures;
	Players players;
	IBody* ppBodies[BODY_COUNT] = { nullptr };
};