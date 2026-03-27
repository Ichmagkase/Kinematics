#pragma once
#include <winsock2.h>
#include <WS2tcpip.h>
#include "enum.h"

class IPC {
public:
	IPC();
	int sendEventPayload(enum Event e);
	~IPC();

private:
	static const char* actions[5];
	static SOCKET ipc_socket;
	static sockaddr_in endpoint;
};