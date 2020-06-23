#include <iostream>
#include <cstddef>
#include <cassert>
#include "VPCParser.h"

char* ReadFile(const char* path, size_t& len)
{

	FILE* f;
	assert((f = fopen(path, "rb")));
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	rewind(f);
	char* str = (char*)malloc(len + 1);
	str[len] = 0;
	fread(str, len, 1, f);
	fclose(f);

	return str;
}



int main(int argc, char** argv)
{

	size_t len = 0;
	const char* buf = ReadFile(argv[1], len);

	VPCParser parser(buf, len);

}