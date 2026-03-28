#include "pch.h"
#include <winsock2.h>
#include <WS2tcpip.h>
#include "../KinectVisionLayer/ipc.h"

#pragma comment(lib, "ws2_32.lib")

class IPCTest : public ::testing::Test {
protected:
    SOCKET receiverSocket;
    sockaddr_in receiverAddr;

    void SetUp() override {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        // Create receiver socket on localhost:1234
        receiverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        ASSERT_NE(receiverSocket, INVALID_SOCKET);

        receiverAddr.sin_family = AF_INET;
        receiverAddr.sin_port = htons(1234);
        inet_pton(AF_INET, "127.0.0.1", &receiverAddr.sin_addr);

        int bindResult = bind(receiverSocket, (sockaddr*)&receiverAddr, sizeof(receiverAddr));
        ASSERT_NE(bindResult, SOCKET_ERROR);

        // Set socket to non-blocking with 2-second timeout
        DWORD timeout = 2000;
        setsockopt(receiverSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    }

    void TearDown() override {
        closesocket(receiverSocket);
        WSACleanup();
    }
};

TEST_F(IPCTest, ReceiveDataOnLocalhost) {
    char buffer[256];
    sockaddr_in senderAddr;
    int senderAddrLen = sizeof(senderAddr);

    // Receive data
    int recvResult = recvfrom(receiverSocket, buffer, sizeof(buffer), 0, (sockaddr*)&senderAddr, &senderAddrLen);

    ASSERT_GT(recvResult, 0) << "Failed to receive data, WSA Error: " << WSAGetLastError();
    EXPECT_NE(std::string(buffer, recvResult).find("test"), std::string::npos);
}