#pragma once
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>
#include "structures.h"

class IPC {
public:
	IPC();
	int sendEventPayload(struct GestureData e);
	~IPC();

private:
	static const std::string actions[5];
	static SOCKET ipc_socket;
	static sockaddr_in endpoint;
};