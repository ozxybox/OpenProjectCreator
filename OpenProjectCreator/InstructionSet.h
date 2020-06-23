#pragma once

#include "ParserHelpers.h"

#include <cstddef>

// super lazy. change this?
#define ARGUMENT_MAX_COUNT 3


// am I a criminal for doing this?

//WHY WONT THEY JUST LET ME GET THE ARG COUNT FOR THE MACRO???
template <typename... A>
constexpr int NumberOfArgs(A ...)
{
	return sizeof...(A);
}

#define DEFINE_INSTRUCTION(name, function, isPreprocessor, ...) {name, sizeof(name) / sizeof(char) - 1, {__VA_ARGS__}, NumberOfArgs(__VA_ARGS__), isPreprocessor, &function},

// has its own custom set of sub instructions
// count is a smelly quick fix. FIX SOON!
#define BEGIN_CUSTOM_INSTRUCTION(name, function, isPreprocessor, count, ...) {name, sizeof(name) / sizeof(char) - 1, {__VA_ARGS__}, NumberOfArgs(__VA_ARGS__), isPreprocessor, &function, new InstructionSet(count, new instruction_t[count] {
#define END_CUSTOM_INSTRUCTION() })},

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

class InstructionSet;
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
	InstructionSet* subInstructions = 0;
};

struct value_t;
struct instructionData_t
{
	~instructionData_t();
	const instruction_t* instruction;
	value_t** arguments = 0;
};



class InstructionSet
{
public:
	InstructionSet() { m_instructionCount = 0; m_instructionSet = nullptr; }
	InstructionSet(size_t count, instruction_t* set);

	instruction_t* GetInstruction(InsetString str);
	size_t GetInstructionCount();

protected:
	instruction_t* m_instructionSet;
	size_t m_instructionCount;


};

