#pragma once
#include <string>
#include <functional>
#include <Kinect.h>
#include <Kinect.VisualGestureBuilder.h>
#include "data.h"

class Sensor {
public:
	Sensor();
	~Sensor();
	void listen(void(*GestureCallback)(struct Data));
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