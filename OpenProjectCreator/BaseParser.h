#pragma once

// super lazy. change this?
#define ARGUMENT_MAX_COUNT 3

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


struct instructionData_t
{
	const char* arguments[ARGUMENT_MAX_COUNT];
	int argumentLength[ARGUMENT_MAX_COUNT];
};

typedef void (*InstructionFunction)(instructionData_t);

// am I a criminal for doing this?

//WHY WONT THEY JUST LET ME GET THE ARG COUNT FOR THE MACRO???
template <typename... A>
constexpr int NumberOfArgs(A ...)
{
	return sizeof...(A);
}

#define DEFINE_INSTRUCTION(name, function, isPreprocessor, ...) {name, sizeof(name), {__VA_ARGS__}, NumberOfArgs(__VA_ARGS__), isPreprocessor, function},

struct instruction_t
{
	const char* name;
	int nameLength;
	ArgumentType argumentTypes[ARGUMENT_MAX_COUNT];
	int argumentCount;
	bool isPreprocessor;
	InstructionFunction function;
};

class BaseParser
{

	void Parse(const char* str, size_t length);
private:

	
	virtual instruction_t GetInstruction(const char* str, size_t length) = 0;
	
	// this is virtual to allow changing of comment stuff
	virtual void SkipWhitespace(const char* str, int& i) = 0;

};

