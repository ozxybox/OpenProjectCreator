#include "VPCParser.h"
#include "ParserHelpers.h"
#include <stdlib.h>

#define INSTRUCTION_PREFIX '$'
#define BLOCK_START        '{'
#define BLOCK_END          '}'
#define STRING_QUOTE       '"'



insetString_t ReadQuotelessString(const char* str, size_t& i, size_t length, ErrorCode* error = 0)
{
	insetString_t inset;

	//first char must not be a symbol that's used by other types or whitespace
	char c = str[i];
	if (c == BLOCK_START || c == BLOCK_END || c == STRING_QUOTE || IsWhitespace(c))
	{
		inset.length = 0;
		inset.string = 0;

		if (error)
			*error = ErrorCode::INVALID_QUOTELESS_STRING;

		return inset;
	}



	inset.string = str + i;
	int start = i;

	i++;

	for (c = str[i]; i < length; i++, c = str[i])
	{
		
		if (IsWhitespace(c) || c == BLOCK_START || c == BLOCK_END || c == STRING_QUOTE)
			break;
	}
	
	inset.length = i - start;

	return inset;
}

insetString_t ReadQuotedString(const char* str, size_t& i, size_t length, ErrorCode* error = 0)
{
	insetString_t inset;

	//first character should be a quote
	if (str[i] != STRING_QUOTE)
	{
		inset.length = 0;
		inset.string = 0;

		if (error)
			*error = ErrorCode::STRING_NOT_BEGUN_WITH_QUOTE;

		return inset;
	}
	i++;

	int start = i;

	for (; i < length && str[i] != STRING_QUOTE; i++);
	
	// if we've hit the end of the string and havent gotten our quote, return null
	if (i >= length)
	{
		inset.length = 0;
		inset.string = 0;
		return inset;
	}

	inset.string = str + i;
	inset.length = i - start;
	
	return inset;
}

insetString_t ReadString(const char* str, size_t& i, size_t length, ErrorCode* error = 0)
{
	if (str[i] == STRING_QUOTE)
		return ReadQuotedString(str, i, length, error);
	return ReadQuotelessString(str, i, length, error);
}

int ReadNumber(const char* str, size_t& i, size_t length, ErrorCode* error = 0)
{
	bool isNegative = false;

	//overengineered? maybe... C++ lets you do +-+-1, so I felt this was appropriate
	if(str[i] == '+' || str[i] == '-')
		for (; i < length; i++)
		{
			switch (str[i])
			{
			case '+':
				isNegative = false;
				continue;
			case '-':
				isNegative = !isNegative;
				continue;
			}
			break;
		}

	//end of string with no numbers
	if (i >= length)
	{
		if (error)
			*error = ErrorCode::NUMBER_WITH_NO_NUMBERS;

		return 0;
	}

	//end of sign with no numbers
	if (!IsNumber(str[i]))
	{
		if (error)
			*error = ErrorCode::NUMBER_WITH_NO_NUMBERS;

		return 0;
	}

	//little shortcut
	//since we know for a fact that str[i] is currently a number, we can skip double checking if it's actually a number
	int number = str[i] - '0';
	i++;

	for (; i < length && IsNumber(str[i]); i++)
	{
		//move the decimal right one
		number *= 10;

		number += str[i] - '0';
	}
	
	return number * (isNegative ? -1 : 1);
}



//this is so sad

void VPC_Macro(instructionData_t data)
{
	
}

void VPC_Configuration(instructionData_t data)
{

}

void VPC_Folder(instructionData_t data)
{

}

void VPC_Folder_Add(instructionData_t data)
{

}


// hash table would be quicker
const instruction_t g_vpcInstructions[] =
{
	//Name, function, is preprocessor, argument types(MAX OF 4)
	DEFINE_INSTRUCTION("Macro", VPC_Macro, true, ArgumentType::QUOTELESS_STRING, ArgumentType::QUOTED_STRING)
	DEFINE_INSTRUCTION("Configuration", VPC_Configuration, false, ArgumentType::QUOTED_STRING, ArgumentType::SUB_BLOCK)

	BEGIN_CUSTOM_INSTRUCTION("Folder", VPC_Folder, false, ArgumentType::STRING, ArgumentType::SUB_BLOCK)
		DEFINE_INSTRUCTION("Add", VPC_Folder_Add, false, ArgumentType::STRING)
	END_CUSTOM_INSTRUCTION()
};



VPCParser::VPCParser(const char* str, size_t length)
{
	ErrorCode error = ErrorCode::NO_ERROR;
	for (size_t i = 0; i < length; i++)
	{
		SkipWhitespace(str, i, length);

		if (str[i] == INSTRUCTION_PREFIX)
		{
			i++;
			insetString_t instructionStr = ReadQuotelessString(str, i, length, &error);
			if (error != ErrorCode::NO_ERROR)
			{
				ThrowException(error);
				return;
			}

			instruction_t* instruction = GetInstruction(instructionStr);
			if (!instruction)
			{
				// we failed to find the instruction. syntax error
				ThrowException(ErrorCode::UNRECOGNIZED_INSTRUCTION);
				return;
			}

			instructionData_t* instructionData = new instructionData_t;
			if(instruction->argumentCount > 0)
			{ 
				instructionData->arguments = new value_t*[instruction->argumentCount];
				for (int argument = 0; argument < instruction->argumentCount; argument++)
				{
					SkipWhitespace(str, i, length);

					if (i >= length)
					{
						ThrowException(ErrorCode::UNEXPECTED_END_OF_FILE);
					}


					switch (instruction->argumentTypes[argument])
					{
					case ArgumentType::ARRAY:

						break;
					case ArgumentType::NUMBER:
						numberValue_t* nv = new numberValue_t;
						nv->number = ReadNumber(str, i, length, &error);

						instructionData->arguments[argument] = nv;
						break;
					case ArgumentType::QUOTED_STRING:
						stringValue_t* sv = new stringValue_t;
						sv->string = ReadQuotedString(str, i, length, &error);

						instructionData->arguments[argument] = sv;
						break;
					case ArgumentType::QUOTELESS_STRING:
						stringValue_t* sv = new stringValue_t;
						sv->string = ReadQuotelessString(str, i, length, &error);

						instructionData->arguments[argument] = sv;
						break;
					case ArgumentType::STRING:
						stringValue_t* sv = new stringValue_t;
						sv->string = ReadString(str, i, length, &error);

						instructionData->arguments[argument] = sv;
						break;
					case ArgumentType::SUB_BLOCK:

						break;
					default:
						// we should never hit an argument without a type
						ThrowException(ErrorCode::COMPILED_WITH_TYPELESS_ARGUMENT);
						return;
					}

					if (error != ErrorCode::NO_ERROR)
					{
						ThrowException(error);
						return;
					}

				}


			}
		}
		else
		{
			// we should never hit an instruction without a $
			ThrowException(ErrorCode::UNPREFIXED_INSTRUCTION);
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
				ThrowException(ErrorCode::UNCLOSED_MULTILINE_COMMENT);
				return;
			}
		}
	}
}
