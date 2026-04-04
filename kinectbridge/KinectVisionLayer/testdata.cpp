#include <random>
#include <string>
#include "structures.h"
#include "test.h"

struct GestureData getRandomData() {
	int random_event = rand() % 5; // Assuming there are 5 events in the enum
	struct GestureData data;
	data.event = static_cast<Event>(random_event);
	data.player = std::to_string(rand() % 2 + 1); // Random player number (1 or 2)
	return data;
}