#pragma once
#include <string>

enum Event {
	JUMP,
	PUNCH,
	MOVE_R,
	MOVE_L,
	BLOCK
};

struct GestureData {
	Event event;
	std::string player;
};