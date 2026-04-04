#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include "ipc.h"

#pragma comment(lib, "ws2_32.lib")

#define HOST_IP "192.168.145.143"
#define HOST_PORT 4242

// Static member initializations
const std::string IPC::actions[] = { "move_jump", "attack_1", "move_right", "move_left", "block" };
SOCKET IPC::ipc_socket = INVALID_SOCKET;
sockaddr_in IPC::endpoint = {};

// Constructor implementation
IPC::IPC() {
	int status;
	WSADATA wsaData;
	status = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (status != 0) {
		std::cout << "WSAStartup failed with error: " << status << std::endl;
		exit(1);
	}
	ipc_socket = INVALID_SOCKET;
	ipc_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ipc_socket == INVALID_SOCKET) {
		std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		exit(1);
	}

	endpoint.sin_family = AF_INET;
	endpoint.sin_port = htons(HOST_PORT);
	inet_pton(AF_INET, HOST_IP, &endpoint.sin_addr);
}

// Method implementation
int IPC::sendEventPayload(struct GestureData e) {
	std::string packet = e.player + "|" + actions[e.event] + "\n";
	std::cout << "Sending packet: " << packet << std::endl;
	int sendResult = sendto(ipc_socket, packet.c_str(), packet.size(), 0, (sockaddr*)&endpoint, sizeof(endpoint));
	if (sendResult == SOCKET_ERROR) {
		std::cout << "sendto failed with error: " << WSAGetLastError() << std::endl;
	}
	else {
		std::cout << "Sent " << sendResult << " bytes" << std::endl;
	}
	return sendResult;
}

// Destructor implementation
IPC::~IPC() {
	closesocket(ipc_socket);
	WSACleanup();
}

