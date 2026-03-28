#include <random>
#include <string>
#include "data.h"
#include "test.h"

struct Data getRandomData() {
	int random_event = rand() % 5; // Assuming there are 5 events in the enum
	struct Data data;
	data.event = static_cast<Event>(random_event);
	data.player = std::to_string(rand() % 2 + 1); // Random player number (1 or 2)
	return data;
}