#include "VPCParser.h"

#define INSTRUCTION_PREFIX '$'

/*
struct instruction_t
{
	const char* name;
	int argumentCount;
	InstructionTypes* argumentTypes;
	bool isPreprocessor;
};
*/


//this is so sad

void VPC_Macro(instructionData_t data)
{
	
}

void VPC_Configuration(instructionData_t data)
{

}

// hash table would be quicker
instruction_t g_vpcInstructions[] =
{
	//Name, function, is preprocessor, argument types(MAX OF 4)
	DEFINE_INSTRUCTION("Macro", VPC_Macro, true, ArgumentType::QUOTELESS_STRING, ArgumentType::QUOTED_STRING)
	DEFINE_INSTRUCTION("Configuration", VPC_Configuration, false, ArgumentType::QUOTED_STRING, ArgumentType::SUBBLOCK)
};


VPCParser::VPCParser(const char* str, size_t length)
{
	for (int i = 0; i < length; i++)
	{

	}
}