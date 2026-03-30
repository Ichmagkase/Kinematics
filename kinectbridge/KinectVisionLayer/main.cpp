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
	std::array<UINT64, 2> players;
	Sleep(3);
	players = sensor.awaitPlayersReady();
	sensor.listen(&GestureListener, players);
}