#include <random>

enum Event getRandomEvent() {
	int random = rand() % 5; // Assuming there are 5 events in the enum
	return static_cast<Event>(random);
}