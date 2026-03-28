#include <iostream>
#include <string>
#include <functional>
#include <map>
#include <Kinect.h>
#include <Kinect.VisualGestureBuilder.h>
#include "sensor.h"

#pragma comment(lib, "Kinect20.lib")
#pragma comment(lib, "Kinect20.VisualGestureBuilder.lib")

#define MAX_PLAYERS 2

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

	// Load gesture database
	//IVisualGestureBuilderDatabase* pGestureDatabase = nullptr;

	//if (FAILED(CreateVisualGestureBuilderDatabaseInstanceFromFile(L"HandRaised2.gbd", &pGestureDatabase))) {
	//	std::cerr << "Failed to load gesture database" << std::endl;
	//	pBodyFrameSource->Release();
	//	pSensor->Release();
	//	exit(1);
	//}

	//gestureCount = 0;
	//if (FAILED(pGestureDatabase->get_AvailableGesturesCount(&gestureCount))) {
	//	std::cerr << "Failed to get gesture count" << std::endl;
	//	pGestureDatabase->Release();
	//	pBodyFrameSource->Release();
	//	pSensor->Release();
	//	exit(1);
	//}

	//IGesture **gestures = new IGesture * [gestureCount];
	//pGestureDatabase->get_AvailableGestures(gestureCount, gestures);

	//// Create gesture frame source
	//if (FAILED(CreateVisualGestureBuilderFrameSource(pSensor, 1, &pGestureFrameSource))) {
	//	std::cerr << "Failed to create gesture frame source" << std::endl;
	//	delete[] gestures;
	//	pGestureDatabase->Release();
	//	pBodyFrameSource->Release();
	//	pSensor->Release();
	//	exit(1);
	//}

	//// Add gestures to track
	//if (FAILED(pGestureFrameSource->AddGestures(gestureCount, gestures))) {
	//	std::cerr << "Failed to add gestures" << std::endl;
	//	delete[] gestures;
	//	pGestureFrameSource->Release();
	//	pGestureDatabase->Release();
	//	pBodyFrameSource->Release();
	//	pSensor->Release();
	//	exit(1);
	//}

	//// Create gesture frame reader
	//if (FAILED(pGestureFrameSource->OpenReader(&pGestureFrameReader))) {
	//	std::cerr << "Failed to open gesture frame reader" << std::endl;
	//	delete[] gestures;
	//	pGestureFrameSource->Release();
	//	pGestureDatabase->Release();
	//	pBodyFrameSource->Release();
	//	pSensor->Release();
	//	exit(1);
	//}

	//// Store database and clean up gestures array (but not individual gestures - they're referenced by the source)
	//this->pGestureDatabase = pGestureDatabase;
	//this->pGestures = gestures;
	//this->gestureCount = gestureCount;
}

void Sensor::listen(void(*GestureCallback)(struct Data)) {
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
		if (SUCCEEDED(pBodyFrameReader->AcquireLatestFrame(&pBodyFrame)) && pBodyFrame) {

			// Get the body data
			if (SUCCEEDED(pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBodies))) {
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
				IBody *most_left = ppBodies[0];
				IBody *most_right = ppBodies[0];
				for (auto const& [BodyId, Body] : currentTrackedBodies) {
					Joint* joints;
					Joint joints[JointType_Count];
					if (SUCCEEDED(Body->GetJoints(JointType_Count, joints))) {
						float x = joints[JointType_Head].Position.X;

						Joint leftJoints[JointType_Count];
						most_left->GetJoints(JointType_Count, leftJoints);

						Joint rightJoints[JointType_Count];
						most_right->GetJoints(JointType_Count, rightJoints);

						if (x < leftJoints[JointType_Head].Position.X) {
							most_left = Body;
						}
						if (x > rightJoints[JointType_Head].Position.X) {
							most_right = Body;
						}
					}
				}

				// Update tracked bodies for next frame
				// Print status every 30 frames
				if (frameCount % 30 == 0) {
					std::cout << "Frame " << frameCount << ": " << ((currentTrackedBodies.size() > 0) ? "PERSON PRESENT" : "No person")
						<< " (Tracking " << currentTrackedBodies.size() << " bodies)" << std::endl;
				}
			}

			pBodyFrame->Release();
		}

		frameCount++;
		Sleep(33);  // ~30 FPS
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
