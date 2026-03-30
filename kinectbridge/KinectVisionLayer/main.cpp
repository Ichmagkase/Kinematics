#include "ipc.h"
#include "sensor.h"
#include "test.h"
#include <iostream>
#include "data.h"

static IPC ipc;

void GestureListener(struct Data data) {
	std::cout << "Player " << data.player << " performed gesture: " << data.event << std::endl;
	ipc.sendEventPayload(data);
}

int main() {
	Sensor sensor;

	// AWAIT PLAYERS READY ... Do this once to determine relative position
	std::array<IBody*, 2> players;
	players = sensor.awaitPlayersReady();

	while (true) {
		Sleep(1000); // Simulate waiting for sensor data
		sensor.listen(&GestureListener, players);		
	}
}