//시스템
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <condition_variable>
#include <mutex>

//파일입출력
#include <fstream>

//현재 시간 출력을 위한 lib
#include <time.h>
#include <string>

//Error
#include "./ErrorCheck/ErrorCheck.h"

//Utility
#include "./Utility/Utility.h"

//Convert
#include "./Convert/Convert.h"

//스레드 풀
#include "./ThreadPool/ThreadPool.h"

//패킷
#include "./Packet/Packet.h"

//통신
#include "./Session/Session.h"

