#include <iostream>
#include <Kinect.h>

int main() {
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

	IGesture** gestures = new IGesture * [gestureCount];
	hr = pGestureDatabase->get_AvailableGestures(gestureCount, gestures);
	CheckError(hr, "IVisualGestureBuilderDatabase::get_AvailableGestures");
}

