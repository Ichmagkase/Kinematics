#include "ipc.h"
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
	const std::string actions[] = { "move_jump", "attack_1", "attack_2", "move_right", "move_left", "block", "idle", "player1_ready", "player2_ready" };

	std::cout << "Player " << data.player << " performed gesture: " << actions[data.event] << std::endl;
	ipc.sendEventPayload(data);
}

int main() {
	std::cout << "initializing sensor..." << std::endl;
	Sensor sensor(&GestureListener);
	Sleep(30); //Allow extra time to initialize
	std::cout << "waiting for players ..." << std::endl;
	sensor.awaitPlayersReady();
	std::cout << "listening for gestures" << std::endl;
	// It would make alot more sense if you passed the ipc pipeline into listen for it to use, alas.
	// This method is probably better for debugging anyway.
	sensor.listen();
}
