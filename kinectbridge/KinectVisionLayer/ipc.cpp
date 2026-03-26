#include <iostream>
#include <Winsock.h>
#include <WS2tcpip.h>
#include "enum.h"

class IPC {
public:
	IPC() {
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
		endpoint.sin_port = htons(12345);
		inet_pton(AF_INET, "127.0.0.1", &endpoint.sin_addr);
	};

	int sendEventPayload(enum Event e) {
		int sendResult = sendto(ipc_socket, actions[e], sizeof(actions[e]), 0, (sockaddr*)&endpoint, sizeof(endpoint));
		if (sendResult == SOCKET_ERROR) {
			std::cout << "sendto failed with error: " << WSAGetLastError() << std::endl;
		}
		else {
			std::cout << "Sent" << sendResult << " bytes" << std::endl;
		}
	}

	~IPC() {
		closesocket(ipc_socket);
		WSACleanup();
	}
private:
	static const char* actions[5];
	static SOCKET ipc_socket;
	static sockaddr_in endpoint;
};

const char* IPC::actions[] = { "move_jump", "attack_1", "move_right", "move_left", "block" };

