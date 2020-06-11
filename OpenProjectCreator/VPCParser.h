#pragma once

#include "BaseParser.h"


class VPCParser : public BaseParser
{
public:
	VPCParser(const char* str, size_t length);

private:
	//virtual instruction_t* GetInstruction(const char* str, size_t length);
	virtual void SkipWhitespace(const char* str, size_t& i, size_t length);

	virtual bool ParseCondition(const char* str, size_t& i, size_t length, ErrorCode* error = 0);

};

