CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
PROJECT(HttpEater)

SET(CMAKE_CXX_STANDARD 23)

ADD_EXECUTABLE(HttpEater misc.cpp handle.cpp multi_thread.cpp main.cpp)

IF (WIN32)
    TARGET_LINK_LIBRARIES(HttpEater ws2_32)
ENDIF()
