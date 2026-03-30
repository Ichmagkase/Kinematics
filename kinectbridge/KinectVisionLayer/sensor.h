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
	void listen(void(*GestureCallback)(struct Data), std::array<IBody*, 2> players);
	std::array<IBody*, 2> awaitPlayersReady();
private:
	IBodyFrameReader* pBodyFrameReader;
	IBodyFrameSource* pBodyFrameSource;
	IKinectSensor* pSensor;
	IVisualGestureBuilderFrameSource* pGestureFrameSource;
	IVisualGestureBuilderFrameReader* pGestureFrameReader;
	IVisualGestureBuilderDatabase* pGestureDatabase;
	IGesture** pGestures;
	UINT gestureCount;
};