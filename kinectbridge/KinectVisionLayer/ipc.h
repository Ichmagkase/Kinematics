#pragma once
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>
#include "data.h"

class IPC {
public:
	IPC();
	int sendEventPayload(struct Data e);
	~IPC();

private:
	static const std::string actions[5];
	static SOCKET ipc_socket;
	static sockaddr_in endpoint;
};