#pragma once

#include <iostream>
#include <stdio.h>
#include <consoleapi.h>

class Debug {
public:
	static void init() {
		AllocConsole();

		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
		freopen_s(&f, "CONOUT$", "w", stderr);
	}

	static void write(const char* s) {
		std::cout << s << std::endl;
	}

	static void close() {
		FreeConsole();
	}
};