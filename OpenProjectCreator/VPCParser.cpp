#include "VPCParser.h"
#include "ParserHelpers.h"

#define INSTRUCTION_PREFIX '$'
#define BLOCK_START        '{'
#define BLOCK_END          '}'
#define STRING_QUOTE       '"'

insetString_t SeekEndOfQuotelessString(const char* str, size_t& i, size_t length)
{
	insetString_t inset;
	inset.string = str + i;
	int start = i;

	for (char c = str[i]; i < length; i++, c = str[i])
	{
		
		if (IsWhitespace(c) || c == BLOCK_START || c == BLOCK_END || c == STRING_QUOTE)
			break;
	}
	
	inset.length = i - start;

	return inset;
}

bool SeekEndOfQuoted(const char* str, size_t& i, size_t length)
{
	for (; i < length && str[i] != STRING_QUOTE; i++);
	// if we've hit the end of the string and havent gotten our quote, return false
	return i < length;
}

//this is meant to be ran on the start of a string
void SeekEndOfString(const char* str, size_t& i, size_t length)
{
	if (str[i] == STRING_QUOTE)
	{

	}
}

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
	DEFINE_INSTRUCTION("Configuration", VPC_Configuration, false, ArgumentType::QUOTED_STRING, ArgumentType::SUB_BLOCK)
};


VPCParser::VPCParser(const char* str, size_t length)
{
	for (size_t i = 0; i < length; i++)
	{
		SkipWhitespace(str, i, length);

		if (str[i] == INSTRUCTION_PREFIX)
		{
			i++;
			insetString_t instructionStr = SeekEndOfQuotelessString(str, i, length);
			instruction_t instruction = GetInstruction(instructionStr);
			
			for (int argument = 0; argument < instruction.argumentCount; argument++)
			{
				switch (instruction.argumentTypes[i])
				{
				case ArgumentType::ARRAY:

				case ArgumentType::NUMBER:

				case ArgumentType::QUOTED_STRING:

				case ArgumentType::QUOTELESS_STRING:

				case ArgumentType::STRING:

				case ArgumentType::SUB_BLOCK:

				default:
					// we should never hit an argument without a type
					ThrowException();
				}


			}
		}
		else
		{
			// we should never hit an instruction without a $
			ThrowException();
		}

	}
}

void VPCParser::SkipWhitespace(const char* str, size_t& i, size_t length)
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
