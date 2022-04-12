// C standard headers
#include <cstdio>
#include <cstring>
#include <ctime>

// C++ standard headers
#include <vector>
#include <string>
#include <algorithm>

#include "func.h"

unsigned long long reqCnt = 0;

void HandleConnection(socket_t client_fd) {
	char timeStr[64];
	char* buffer = new char[std::max(BUFFER_SIZE + 1, RESPOND_CONTENT_LENGTH + 150)];
	PRINT_FUNC("------------------------- BEGIN OF REQUEST %llu -------------------------\n", ++reqCnt);

	std::vector<char> header, requestBody;
	int contentLength = 0;
	bool isRequestBody = false;
	for (auto bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0); bytes_read > 0; bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0)) {
		buffer[bytes_read] = '\0';
		if (!isRequestBody) {
			char* ptr = std::strstr(buffer, "\r\n\r\n");
			if (ptr) {
				*ptr = '\0';
				header.insert(header.end(), buffer, ptr);

				isRequestBody = true;

				std::string headerStr(header.begin(), header.end());
				for (char& ch : headerStr)
					ch = std::tolower(ch);
				auto contentLengthPos = headerStr.find("content-length:");
				if (contentLengthPos != std::string::npos)
					std::sscanf(headerStr.c_str() + contentLengthPos + 15, "%d", &contentLength);

				requestBody.insert(requestBody.end(), ptr + 4, buffer + bytes_read);
			}
			else {
				header.insert(header.end(), buffer, buffer + bytes_read);
			}
		}
		else {
			requestBody.insert(requestBody.end(), buffer, buffer + bytes_read);
		}
		if (requestBody.size() >= contentLength)
			break;
	}
	if (std::any_of(header.begin(), header.end(), [](char ch) { return !std::isprint(ch) && ch != '\r' && ch != '\n'; }))
		hexdump(header);
	else {
		header.push_back('\0');
		PRINT_FUNC("%s", header.data());
	}
	PRINT_FUNC("\n");
	if (requestBody.empty())
		PRINT_FUNC("Request body is empty\n");
	else
		hexdump(requestBody);

	const std::time_t timeNow = std::time(nullptr);
	std::strftime(timeStr, sizeof timeStr, "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&timeNow));
	std::sprintf(buffer, "HTTP/1.1 200 OK\r\n"
						 "Date: %s\r\n"
						 "Content-Length: %d\r\n"
						 "Content-Type: application/json; charset=utf-8\r\n"
						 "Connection: close\r\n"
						 "\r\n"
						 "%s", timeStr, RESPOND_CONTENT_LENGTH, RESPOND_CONTENT);

	const auto sent = send(client_fd, buffer, std::strlen(buffer), 0);
	PRINT_FUNC("\n%d bytes has been sent.\n", sent);
	PRINT_FUNC("Time: %s\n", timeStr);

	CloseSocket(client_fd);
	PRINT_FUNC("-------------------------- END OF REQUEST %llu --------------------------\n\n", reqCnt);
	delete[] buffer;
}
