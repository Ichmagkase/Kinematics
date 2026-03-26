#include <iostream>
#include <Kinect.h>

class Sensor {
public:
	Sensor() {
		std::cout << "Kinect Person Detection Program" << std::endl;

		// Get the default Kinect sensor
		IKinectSensor* pSensor = nullptr;
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
		IBodyFrameSource* pBodyFrameSource = nullptr;
		if (FAILED(pSensor->get_BodyFrameSource(&pBodyFrameSource))) {
			std::cerr << "Failed to get body frame source" << std::endl;
			pSensor->Release();
			exit(1);
		}

		// Create body frame reader
		IBodyFrameReader* pBodyFrameReader = nullptr;
		if (FAILED(pBodyFrameSource->OpenReader(&pBodyFrameReader))) {
			std::cerr << "Failed to open body frame reader" << std::endl;
			pBodyFrameSource->Release();
			pSensor->Release();
			exit(1);
		}

		std::cout << "Kinect sensor initialized successfully" << std::endl;
	};

	int listen() {
		std::cout << "Listening for persons..." << std::endl;

		// Detection loop
		bool bPersonDetected = false;
		int frameCount = 0;

		HandState leftHandState = HandState_Unknown, rightHandState = HandState_Unknown;
		HandState previousLeftHandState = HandState_Unknown, previousRightHandState = HandState_Unknown;

		TrackingConfidence leftHandConfidence, rightHandConfidence;

		while (true) {
			IBodyFrame* pBodyFrame = nullptr;

			// Get the latest body frame
			if (SUCCEEDED(pBodyFrameReader->AcquireLatestFrame(&pBodyFrame)) && pBodyFrame) {
				IBody* ppBodies[BODY_COUNT] = { nullptr };

				// Get the body data
				if (SUCCEEDED(pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBodies))) {
					bPersonDetected = false;

					// Check each body
					for (int i = 0; i < BODY_COUNT; ++i) {
						if (ppBodies[i]) {
							BOOLEAN bTracked = false;
							if (SUCCEEDED(ppBodies[i]->get_IsTracked(&bTracked)) && bTracked) {
								ppBodies[i]->get_HandRightState(&rightHandState);
								ppBodies[i]->get_HandRightConfidence(&rightHandConfidence);
								ppBodies[i]->get_HandLeftState(&leftHandState);
								ppBodies[i]->get_HandLeftConfidence(&leftHandConfidence);

								if (rightHandState != previousRightHandState || leftHandState != previousLeftHandState) {
									std::cout << "Right hand state: " << rightHandState << " (confidence: " << rightHandConfidence << ")" << std::endl;
									std::cout << "Left hand state: " << leftHandState << " (confidence: " << leftHandConfidence << ")" << std::endl;
								}

								previousRightHandState = rightHandState;
								previousLeftHandState = leftHandState;
								bPersonDetected = true;
							}
							ppBodies[i]->Release();
						}
					}

					// Print status every 30 frames
					if (frameCount % 30 == 0) {
						std::cout << "Frame " << frameCount << ": " << (bPersonDetected ? "PERSON PRESENT" : "No person") << std::endl;
					}
				}

				pBodyFrame->Release();
			}

			frameCount++;
			Sleep(33);  // ~30 FPS
		}

		// Cleanup
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

		std::cout << "Kinect sensor closed" << std::endl;
		return 0;
	}
	private:
		IBodyFrameReader* pBodyFrameReader;
		IBodyFrameSource* pBodyFrameSource;
		IKinectSensor* pSensor;
};