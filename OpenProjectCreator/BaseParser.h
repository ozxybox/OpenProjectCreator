#pragma once

#include "ParserHelpers.h"

// super lazy. change this?
#define ARGUMENT_MAX_COUNT 3

// am I a criminal for doing this?

//WHY WONT THEY JUST LET ME GET THE ARG COUNT FOR THE MACRO???
template <typename... A>
constexpr int NumberOfArgs(A ...)
{
	return sizeof...(A);
}

#define DEFINE_INSTRUCTION(name, function, isPreprocessor, ...) {name, sizeof(name), {__VA_ARGS__}, NumberOfArgs(__VA_ARGS__), isPreprocessor, function},

// has its own custom set of sub instructions
// count is a smelly quick fix. FIX SOON!
#define BEGIN_CUSTOM_INSTRUCTION(name, function, isPreprocessor, count, ...) {name, sizeof(name), {__VA_ARGS__}, NumberOfArgs(__VA_ARGS__), isPreprocessor, function, new instruction_t[count] {
#define END_CUSTOM_INSTRUCTION() }},


enum class ErrorCode {
	NO_ERROR = 0,
	UNEXPECTED_END_OF_FILE,
	NUMBER_WITH_NO_NUMBERS,
	UNPREFIXED_INSTRUCTION,
	UNRECOGNIZED_INSTRUCTION,
	UNCLOSED_MULTILINE_COMMENT,
	COMPILED_WITH_TYPELESS_ARGUMENT,
	STRING_NOT_BEGUN_WITH_QUOTE,
	INVALID_QUOTELESS_STRING,
	NOT_IMPLEMENTED,
	SECONDARY_CONDITION,
	INCOMPLETE_QUOTED_STRING,
};


enum class ArgumentType
{
	STRING,
	QUOTELESS_STRING,
	QUOTED_STRING,
	NUMBER,
	//contains just a single element
	ARRAY,
	//can contain instructions
	SUB_BLOCK
};

enum class ValueType
{
	STRING,
	NUMBER,
	//contains just a single element
	ARRAY,
	//can contain instructions
	SUB_BLOCK
};


struct instructionData_t;
typedef void (*InstructionFunction)(instructionData_t);

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
	~instructionData_t() { 
		if (arguments) 
		{ 
			for (int i = 0; i < instruction->argumentCount; i++)
				delete arguments[i];
			
			delete[] arguments; 
		}
	}
	instruction_t* instruction;
	value_t** arguments = 0;
};



struct value_t
{
	ValueType type;
};

struct stringValue_t : public value_t
{
	ValueType type = ValueType::STRING;
	insetString_t string;
};

struct numberValue_t : public value_t
{
	ValueType type = ValueType::NUMBER;
	int number;
};

struct arrayValue_t : public value_t
{
	~arrayValue_t() { if (array) delete[] array; }
	ValueType type = ValueType::ARRAY;
	value_t* array = 0;
	size_t length = 0;
};

struct subBlockValue_t : public value_t
{
	~subBlockValue_t() { if(subBlock) delete[] subBlock; }
	ValueType type = ValueType::SUB_BLOCK;
	instructionData_t* subBlock = 0;
	size_t length = 0;
};


class BaseParser
{

	void Parse(const char* str, size_t length);

protected:
	
	virtual instruction_t* GetInstruction(const char* str, size_t length) = 0;
	inline instruction_t* GetInstruction(insetString_t str) { return GetInstruction(str.string, str.length); }

	// this is virtual to allow changing of comment stuff
	virtual void SkipWhitespace(const char* str, int& i, size_t length) = 0;

	void ThrowException(ErrorCode error) {}

	virtual value_t* ParseArgument(ArgumentType type, const char* str, size_t& i, size_t length, ErrorCode* error = 0);
	virtual void SeekEndOfArgument(ArgumentType type, const char* str, size_t& i, size_t length, ErrorCode* error = 0);

};

