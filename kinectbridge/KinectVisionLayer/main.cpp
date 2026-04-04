#include "ipc.h"
//#include "sensor.h"
//#include "test.h"
//#include <iostream>
//#include "data.h"

#include <iostream>
#include <string>
#include <functional>
#include <map>
#include <Kinect.h>
#include <Kinect.VisualGestureBuilder.h>
#include "sensor.h"
#include <windows.h>

static IPC ipc;

void GestureListener(struct GestureData data) {
	std::cout << "Player " << data.player << " performed gesture: " << data.event << std::endl;
	ipc.sendEventPayload(data);
}

int main() {
	Sensor sensor;
	Sleep(30); //Allow extra time to initialize
	sensor.awaitPlayersReady();
	sensor.listen(&GestureListener);
	// debugGestures();
}

//std::string GetHRESULTErrorMessage(HRESULT hr);
//void CheckError(HRESULT hr, std::string message);
//
//void debugGestures() {
//	// REFACTOR: dont do this. its not 1989
//	IBodyFrameReader* pBodyFrameReader = nullptr;
//	IBodyFrameSource* pBodyFrameSource = nullptr;
//	IKinectSensor* pSensor = nullptr;
//	IVisualGestureBuilderDatabase* pGestureDatabase = nullptr;
//	IVisualGestureBuilderFrameSource* gestureSource = nullptr;
//	IVisualGestureBuilderFrameReader* gestureReader = nullptr;
//	IGesture** pGestures = nullptr;
//	UINT gestureCount = 0;
//
//	std::cout << "Kinect Person Detection Program" << std::endl;
//
//	HRESULT hr;
//
//	// Get the default Kinect sensor
//	hr = GetDefaultKinectSensor(&pSensor);
//	CheckError(hr, "GetDefaultKinectSensor");
//
//	if (!pSensor) {
//		std::cerr << "No Kinect sensor found" << std::endl;
//		exit(1);
//	}
//
//	// Open the sensor
//	hr = pSensor->Open();
//	CheckError(hr, "IKinectSensor::Open()");
//
//	// Get the body frame source
//	hr = pSensor->get_BodyFrameSource(&pBodyFrameSource);
//	CheckError(hr, "IKinectSensor::get_BodyFrameSource");
//
//	// Create body frame reader
//	hr = pBodyFrameSource->OpenReader(&pBodyFrameReader);
//	CheckError(hr, "IBodyFrameSource::OpenReader");
//
//	std::cout << "Kinect sensor initialized successfully" << std::endl;
//
//	// Load gesture database
//	hr = CreateVisualGestureBuilderDatabaseInstanceFromFile(
//		L"C:\\Users\\micro\\source\\repos\\Ichmagkase\\Kinematics\\kinectbridge\\x64\\Debug\\gestures.gbd",
//		&pGestureDatabase
//	);
//	CheckError(hr, "CreateVisualGestureBuilderDatabaseInstanceFromFile");
//
//	hr = pGestureDatabase->get_AvailableGesturesCount(&gestureCount);
//	CheckError(hr, "IVisualGestureBuilderDatabase::get_AvailableGesturesCount");
//
//	pGestures = new IGesture * [gestureCount];
//	hr = pGestureDatabase->get_AvailableGestures(gestureCount, pGestures);
//	CheckError(hr, "IVisualGestureBuilderDatabase::get_AvailableGestures");
//
//	// --- FIX 1: INITIALIZE VGB OUTSIDE THE LOOP ---
//	// Create the source with a tracking ID of 0 (dormant state)
//	hr = CreateVisualGestureBuilderFrameSource(pSensor, 0, &gestureSource);
//	CheckError(hr, "CreateVisualGestureBuilderFrameSource");
//
//	hr = gestureSource->AddGestures(gestureCount, pGestures);
//	CheckError(hr, "AddGestures");
//
//	for (UINT i = 0; i < gestureCount; ++i) {
//		gestureSource->SetIsEnabled(pGestures[i], true);
//	}
//
//	hr = gestureSource->OpenReader(&gestureReader);
//	CheckError(hr, "OpenReader");
//	// ----------------------------------------------
//
//	IBody* ppBodies[BODY_COUNT] = { nullptr };
//	UINT64 player1_id = 0;
//	UINT64 last_tracking_id = 0;
//
//	std::cout << "Entering main loop" << std::endl;
//
//	// Main gesture detection loop
//
//	// REFACTOR: In general make this entire thing less deep by inverting conditions and handling errors better
//	while (true) {
//		// 1. INDEPENDENTLY poll for Body Frames to keep Tracking ID updated
//
//		// ------------- REFACTOR: Could be wrapped in a function ----------------
//		IBodyFrame* pBodyFrame = nullptr;
//		hr = pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);
//
//		if (SUCCEEDED(hr) && pBodyFrame) {
//			hr = pBodyFrame->GetAndRefreshBodyData(BODY_COUNT, ppBodies);
//			pBodyFrame->Release();
//
//			if (SUCCEEDED(hr)) {
//				player1_id = 0; // Reset to see if player is still in THIS frame
//				for (int i = 0; i < BODY_COUNT; ++i) {
//					BOOLEAN bTracked = false;
//					if (ppBodies[i]) {
//						ppBodies[i]->get_IsTracked(&bTracked);
//						if (bTracked) {
//							ppBodies[i]->get_TrackingId(&player1_id);
//							break;
//						}
//					}
//				}
//				
//				// REFACTOR: This is part really necessary if GetAndRefreshBodyData handles this? This should be run only once on shutdown
//				for (int i = 0; i < BODY_COUNT; ++i) {
//					if (ppBodies[i]) {
//						ppBodies[i]->Release();
//						ppBodies[i] = nullptr;
//					}
//				}
//			}
//		}
//		// --------------------------------------------------------------
//		
//		// In concept, this segment checks if the previous segment (^^^^) has run correctly 
//		// If we lose the player, wait
//		if (player1_id == 0) {
//			std::cout << "No player tracked, waiting...\r";
//
//			// --- FIX 2: RESET VGB TRACKING ID ---
//			// If we lost the player, explicitly tell VGB to stop tracking
//			if (last_tracking_id != 0) {
//				gestureSource->put_TrackingId(0);
//				last_tracking_id = 0;
//			}
//
//			Sleep(5);
//			continue;
//		}
//		
//		// 2. Manage Gesture ID updates
//		if (player1_id != last_tracking_id) {
//			hr = gestureSource->put_TrackingId(player1_id);
//			if (SUCCEEDED(hr)) {
//				last_tracking_id = player1_id;
//				std::cout << "\nTracking ID updated to " << player1_id << std::endl;
//			}
//			// --- FIX 3: REMOVE CONTINUE ---
//			// Removing 'continue;' here ensures we immediately flow into checking the gesture frame
//		}
//
//		// 3. INDEPENDENTLY poll for Gesture Frames
//		IVisualGestureBuilderFrame* pGestureFrame = nullptr;
//		hr = gestureReader->CalculateAndAcquireLatestFrame(&pGestureFrame);
//
//		if (SUCCEEDED(hr) && pGestureFrame) {
//			// --- FIX 4: INITIALIZE YOUR BOOLEANS ---
//			// C++ leaves this as garbage memory if the function fails. Always initialize to false.
//			BOOLEAN isTrackingIdValid = false;
//			pGestureFrame->get_IsTrackingIdValid(&isTrackingIdValid);
//
//			if (isTrackingIdValid) {
//				BOOLEAN gesturing = false;
//
//				for (UINT j = 0; j < gestureCount; ++j) {
//					if (gesturing) break;
//
//					// REFACTOR: This is unnecessary
//					GestureType gestureType;
//					pGestures[j]->get_GestureType(&gestureType);
//
//					// REFACTOR: this condition is unnecessary. All gestures being used are discrete.
//					if (gestureType == GestureType_Discrete) {
//						IDiscreteGestureResult* pGestureResult = nullptr;
//						hr = pGestureFrame->get_DiscreteGestureResult(pGestures[j], &pGestureResult);
//
//						if (SUCCEEDED(hr) && pGestureResult != nullptr) {
//							BOOLEAN isDetected = false;
//							float confidence = 0.0f;
//							pGestureResult->get_Detected(&isDetected);
//							pGestureResult->get_Confidence(&confidence);
//							
//							if (isDetected && confidence > 0.5f) {
//								// 1. Safely allocate the wide character buffer
//								wchar_t gestureNameW[260] = { 0 };
//
//								// 2. Fetch the name and capture the HRESULT to ensure it actually succeeds
//								// REFACTOR: this call explicitly uses 260. use a variable or get the size of gestureNameW
//								HRESULT nameHr = pGestures[j]->get_Name(260, gestureNameW);
//
//								if (SUCCEEDED(nameHr)) {
//									// 3. Convert the wide string to a standard narrow string
//									std::wstring ws(gestureNameW);
//									std::string gestureNameStr(ws.begin(), ws.end());
//
//									// 4. Print using the exact same std::cout you use everywhere else
//									std::cout << "\nDetected gesture: " << gestureNameStr << " (Confidence: " << confidence << ")\n";
//
//									gesturing = true;
//								}
//								else {
//									// If get_Name is failing internally, this will catch it and print the error code
//									std::cout << "\n[Error] Failed to get gesture name. HRESULT: " << std::hex << nameHr << std::dec << "\n";
//								}
//							}
//							pGestureResult->Release();
//						}
//					}
//				}
//
//				if (!gesturing) {
//					std::cout << "Player idle: " << player1_id << "          \r";
//				}
//			}
//			pGestureFrame->Release();
//		}
//
//		// Prevent CPU pegging while maintaining a fast polling rate
//		Sleep(5);
//	}
//}

