# HttpEater

## Description

Simple demo utilizing socket.

- Listens TCP port 80
- Responds everything with HTTP response `{"status":"ok")`
- Lightweight, multithreading, and (nearly) cross-platform

## Running

To run (take Linux as example):

```shell
mkdir build
cd build
cmake ../src
./HttpEater
```

## Compile-time controls

- Define `NO_PRINT` to inhibit printing detailed request.
- Define `DISABLE_MULTI_THREADING` to use single thread.

## Notes

You may need a compiler supporting C++20 to compile.
