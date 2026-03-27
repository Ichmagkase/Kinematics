#include "ipc.h"
#include "sensor.h"
#include "test.h"

int main() {
	IPC ipc;

	while (true) {
		Sleep(1000); // Simulate waiting for sensor data
		ipc.sendEventPayload(getRandomEvent());
	}
}