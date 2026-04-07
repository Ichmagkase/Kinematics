#pragma once
#include <string>

// Note: P1_READY and P2_READY signals are currently not used.
enum Event {
	JUMP,
	PUNCH_R,
	PUNCH_L,
	MOVE_R,
	MOVE_L,
	BLOCK,
	IDLE,
	P1_READY,
	P2_READY,
};

struct GestureData {
	Event event;
	std::string player;
};