#pragma once

enum Event {
	JUMP,
	PUNCH,
	MOVE_R,
	MOVE_L,
	BLOCK
};

struct Data {
	Event event;
	std::string player;
};