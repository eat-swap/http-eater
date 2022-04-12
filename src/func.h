#ifndef HTTP_EATER_FUNC_H
#define HTTP_EATER_FUNC_H

// Define WIN32 for MSVC
#ifdef _MSC_VER
#ifndef WIN32
#define WIN32
#endif // !WIN32
#pragma warning(disable: 4996)
#endif // _MSC_VER

#ifdef NO_PRINT
template<typename T, typename... Ts>
int DO_NOTHING_FUNC(T, Ts...) {
	return 0;
}
#define PRINT_FUNC DO_NOTHING_FUNC
#else
#define PRINT_FUNC std::printf
#endif // NO_PRINT

#ifdef _MSC_VER
// Microsoft Visual C++ is EVIL
#undef max
#endif // _MSC_VER

#include <cstring>

const int BUFFER_SIZE = 4096;
const char* const RESPOND_CONTENT = R"({"status":"ok"})";
const int RESPOND_CONTENT_LENGTH = std::strlen(RESPOND_CONTENT);

/**
 * Socket related includes
 * Mostly platform-dependent.
 */
#ifdef WIN32
// For Windows
#include <winsock2.h>
#include <ws2tcpip.h>
#else
// For Linux / Unix
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif // WIN32

typedef
#ifdef WIN32
SOCKET
#else
int
#endif // WIN32
	socket_t;

// misc.cpp
void CloseSocket(socket_t sfd);
void DoExit(int status);
void hexdump(const std::vector<char>& data);
void Welcome();

// handle.cpp
void HandleConnection(socket_t client_fd);

// multi_thread.cpp
void WorkThread();
void InitThreads();

#endif //HTTP_EATER_FUNC_H
