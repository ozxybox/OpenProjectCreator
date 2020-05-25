#include "VPCParser.h"
#include "ParserHelpers.h"

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

void VPCParser::SkipWhitespace(const char* str, int& i, size_t length)
{
	for (; i < length; i++)
	{
		if (!IsWhitespace(str[i]))
			break;
		if (i + 1 < length && str[i] == '/')
		{
			i++;
			
			if (str[i] == '/')
			{
				// single line comment
				// read until endline

				for (;  i < length && str[i] != END_LINE; i++);
				continue;
			}
			else if (str[i] == '*')
			{
				// multi line comment
				// read until */

				for(; i + 1 < length; i++)
				{
					if (str[i] == '*' && str[i + 1] == '/')
					{
						break;
					}
				}
				// if we've reached the end of the string and we still havent gotten to the end of the multi line, crash it
				ThrowException();
				return;
			}
		}
	}
}
