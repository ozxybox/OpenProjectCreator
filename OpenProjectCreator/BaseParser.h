#pragma once

#include "ParserHelpers.h"
#include "Errors.h"
#include "InstructionSet.h"

#include <cstddef>


class BaseParser
{
public:
	virtual void Init(const char* str, size_t length) { Parse(str, length); Execute(); }
	virtual ~BaseParser();
protected:

	BaseParser() {}

public:
	virtual BaseParser* Copy() = 0;


protected:
	virtual void Parse(const char* str, size_t length);
	virtual void Execute() = 0;

	virtual instructionData_t* ParseInstruction(const char* str, size_t& i, size_t length, ErrorCode& error) = 0;
	virtual bool ParseCondition(const char* str, size_t& i, size_t length, ErrorCode& error) = 0;

	virtual void ReadSubblock(const char* str, size_t& i, size_t length, ErrorCode& error) = 0;
	virtual void SeekEndOfSubblock(const char* str, size_t& i, size_t length, ErrorCode& error) = 0;

	virtual const instruction_t* GetInstruction(InsetString str) = 0;
	virtual void CacheInstructionData(instructionData_t* instructionData) = 0;

	// this is virtual to allow changing of comment stuff
	virtual void SkipWhitespace(const char* str, size_t& i, size_t length) = 0;

	void ThrowException(ErrorCode error);

	virtual value_t* ParseArgument(ArgumentType type, const char* str, size_t& i, size_t length, ErrorCode& error);
	virtual void SeekEndOfArgument(ArgumentType type, const char* str, size_t& i, size_t length, ErrorCode& error);

};

