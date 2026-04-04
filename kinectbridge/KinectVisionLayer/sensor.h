#pragma once
#include <string>
#include <functional>
#include <Kinect.h>
#include <Kinect.VisualGestureBuilder.h>
#include "data.h"
#include <array>

class Sensor {
public:
	Sensor();
	~Sensor();
	void listen(void(*GestureCallback)(struct Data), std::array<UINT64, 2> players);
	std::array<UINT64, 2> awaitPlayersReady();
	std::array<IBody*, 2> refreshAndGetPlayers(std::array<UINT64, 2> &players);

private:
	BOOLEAN playersAreTracked(std::array<IBody*, 2> players);
	IBodyFrameReader* pBodyFrameReader;
	IBodyFrameSource* pBodyFrameSource;
	IKinectSensor* pSensor;
	IVisualGestureBuilderFrameSource* pGestureFrameSource;
	IVisualGestureBuilderFrameReader* pGestureFrameReader;
	IVisualGestureBuilderDatabase* pGestureDatabase;
	IGesture** pGestures;
	UINT gestureCount;
};