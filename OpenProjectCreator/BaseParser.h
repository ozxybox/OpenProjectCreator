#pragma once

#include "ParserHelpers.h"
#include "Errors.h"

// super lazy. change this?
#define ARGUMENT_MAX_COUNT 3

// am I a criminal for doing this?

//WHY WONT THEY JUST LET ME GET THE ARG COUNT FOR THE MACRO???
template <typename... A>
constexpr int NumberOfArgs(A ...)
{
	return sizeof...(A);
}

#define DEFINE_INSTRUCTION(name, function, isPreprocessor, ...) {name, sizeof(name) / sizeof(char), {__VA_ARGS__}, NumberOfArgs(__VA_ARGS__), isPreprocessor, &function},

// has its own custom set of sub instructions
// count is a smelly quick fix. FIX SOON!
#define BEGIN_CUSTOM_INSTRUCTION(name, function, isPreprocessor, count, ...) {name, sizeof(name) / sizeof(char), {__VA_ARGS__}, NumberOfArgs(__VA_ARGS__), isPreprocessor, &function, new instruction_t[count] {
#define END_CUSTOM_INSTRUCTION() }},


enum class ArgumentType
{
	STRING,
	QUOTELESS_STRING,
	QUOTED_STRING,
	NUMBER,
	//contains just a single element
	ARRAY,
	//can contain instructions
	SUBBLOCK
};


struct instructionData_t;
typedef void (*InstructionFunction)(instructionData_t*);

struct instruction_t
{
	const char* name;
	int nameLength;
	ArgumentType argumentTypes[ARGUMENT_MAX_COUNT];
	int argumentCount;
	bool isPreprocessor;
	InstructionFunction function;
	instruction_t* subInstructions = 0;
};

struct value_t;
struct instructionData_t
{
	~instructionData_t();
	instruction_t* instruction;
	value_t** arguments = 0;
};





class BaseParser
{
public:
	BaseParser(const char* str, size_t length) { Parse(str, length); Execute(); };
protected:

	BaseParser() {}

public:
	virtual BaseParser* Copy() = 0;


protected:
	virtual void Parse(const char* str, size_t length);
	virtual void Execute() = 0;

	virtual instructionData_t* ParseInstruction(const char* str, size_t& i, size_t length, ErrorCode& error) = 0;

	virtual void ReadSubblock(const char* str, size_t& i, size_t length, ErrorCode& error) = 0;
	virtual void SeekEndOfSubblock(const char* str, size_t& i, size_t length, ErrorCode& error) = 0;

	virtual instruction_t* GetInstruction(InsetString str) = 0;
	virtual void CacheInstructionData(instructionData_t* instructionData) = 0;

	// this is virtual to allow changing of comment stuff
	virtual void SkipWhitespace(const char* str, size_t& i, size_t length) = 0;

	void ThrowException(ErrorCode error) {}

	virtual value_t* ParseArgument(ArgumentType type, const char* str, size_t& i, size_t length, ErrorCode& error);
	virtual void SeekEndOfArgument(ArgumentType type, const char* str, size_t& i, size_t length, ErrorCode& error);

};

