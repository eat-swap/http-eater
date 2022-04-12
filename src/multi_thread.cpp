// C++ standard headers
#include <queue>
#include <semaphore>
#include <mutex>
#include <thread>

#include "func.h"

std::queue<socket_t> clientQueue;
std::mutex queueMutex;
std::counting_semaphore queueSignal(0);
std::thread** threadPool;

void WorkThread() {
	while (true) {
		queueSignal.acquire();

		queueMutex.lock();
		auto cfd = clientQueue.front();
		clientQueue.pop();
		queueMutex.unlock();

		HandleConnection(cfd);
	}
}

void InitThreads() {
	auto threadCount = std::thread::hardware_concurrency();
	threadPool = new std::thread*[threadCount];
	for (int i = 0; i < threadCount; ++i) {
		threadPool[i] = new std::thread(WorkThread);
	}
}
