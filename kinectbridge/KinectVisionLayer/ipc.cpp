#include <iostream>
#include <Winsock.h>
#include <WS2tcpip.h>

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

		sockaddr_in endpoint;
		endpoint.sin_family = AF_INET;
		endpoint.sin_port = htons(12345);
		inet_pton(AF_INET, "127.0.0.1", &endpoint.sin_addr);
	};

	// int sendEventPayload(Event e) {}

	~IPC() {
		closesocket(ipc_socket);
		WSACleanup();
	}
private:
	static SOCKET ipc_socket;
};
