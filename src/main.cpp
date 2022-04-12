// C standard headers
#include <csignal>
#include <cstdio>

// C++ standard headers
#include <queue>
#include <semaphore>
#include <mutex>

#include "func.h"

/*
 * To compile on Windows:
 * Linking with ws2_32.lib is required.
 */

extern std::queue<socket_t> clientQueue;
extern std::mutex queueMutex;
extern std::counting_semaphore<> queueSignal;

socket_t socket_fd;

int main() {
#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
	addrinfo* res;
	addrinfo hints{};
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = IPPROTO_TCP;

	for (int notOK = getaddrinfo(nullptr, "80", &hints, &res); notOK;) {
		PRINT_FUNC("getaddrinfo returned non-zero code: %s\n", gai_strerror(notOK));
		return 1;
	}

	socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (socket_fd == -1) {
		PRINT_FUNC("socket returned -1\n");
		return 2;
	}

	// Handle Ctrl+C
	std::signal(SIGINT, [](int) {
		PRINT_FUNC("Ctrl+C was captured\n");
		DoExit(0);
	});

#ifdef WIN32
	// Dual stack
	{
		DWORD no = 0;
		setsockopt(socket_fd, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<char*>(&no), sizeof(no));
	}
#endif

	int bind_result = bind(socket_fd, res->ai_addr, res->ai_addrlen);
	if (bind_result == -1) {
		PRINT_FUNC("bind returned -1\n");
		PRINT_FUNC("errno: %s\n", strerror(errno));
		DoExit(3);
	}

	int listen_result = listen(socket_fd, 5);
	if (listen_result == -1) {
		PRINT_FUNC("listen returned -1\n");
		DoExit(4);
	}

#ifndef DISABLE_MULTI_THREADING
	InitThreads();
#endif

	Welcome();

	sockaddr_storage client_addr{};
	socklen_t client_addr_len = sizeof(client_addr);

	for (socket_t client_fd; -1 != (client_fd = accept(socket_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len));) {
#ifndef DISABLE_MULTI_THREADING
		queueMutex.lock();
		clientQueue.push(client_fd);
		queueMutex.unlock();

		queueSignal.release();
#else
		HandleConnection(client_fd);
#endif
	}

	DoExit(0);
}
