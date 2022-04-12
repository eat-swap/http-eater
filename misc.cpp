// C standard headers
#include <cstdio>
#include <cstdlib>

// C++ standard headers
#include <vector>
#include <string>
#include <thread>

#include "func.h"

void CloseSocket(socket_t sfd) {
#ifdef WIN32
	closesocket(sfd);
#else
	close(sfd);
#endif
}

extern socket_t socket_fd;

void DoExit(int status) {
	CloseSocket(socket_fd);
	std::exit(status);
}

void hexdump(const std::vector<char>& data) {
	std::size_t n = data.size();
	for (std::size_t i = 0; i < n; i += 16) {
		PRINT_FUNC("%08zX | ", i);
		for (std::size_t j = 0; j < 16; ++j) {
			if (i + j < n) {
				PRINT_FUNC("%02X ", static_cast<unsigned char>(data[i + j]));
			}
			else {
				PRINT_FUNC("   ");
			}
		}
		PRINT_FUNC("| ");
		for (std::size_t j = 0; j < 16; ++j) {
			if (i + j < n) {
				PRINT_FUNC("%c", std::isprint(data[i + j]) ? data[i + j] : '.');
			}
		}
		PRINT_FUNC("\n");
	}
}

void Welcome() {
	std::printf("--------------------------------------------\n");
	std::printf("HttpDisplayer in C++, version v0.3.1\n");
	{
		const std::time_t t = std::time(nullptr);
		std::printf("Server started at %s\n", std::asctime(std::localtime(&t)));
	}
	std::printf("Build time: %s %s\n", __DATE__, __TIME__);
	std::printf("C++ compiler: %s / %s\n",
#ifdef _MSC_VER
		"MSVC"
#elif defined(__clang__)
				"Clang"
#elif defined(__GNUC__)
		"GCC"
#else
		"Unknown"
#endif // _MSC_VER
		,
#ifdef _MSC_VER
		[]() {
			auto MSC_FULL_VER = std::to_string(_MSC_FULL_VER);
			MSC_FULL_VER.insert(MSC_FULL_VER.begin() + std::to_string(_MSC_VER).length(), '.');
			return MSC_FULL_VER;
		}().c_str()
#elif defined(__clang__)
				__clang_version__
#elif defined(__GNUC__)
		__VERSION__
#else
		"Unknown Version"
#endif // _MSC_VER
	);
	std::printf("C++ standard: %ld\n", __cplusplus);
	std::printf("Handling incoming connections using %d threads.\n", std::thread::hardware_concurrency());
	std::printf("--------------------------------------------\n");
}
