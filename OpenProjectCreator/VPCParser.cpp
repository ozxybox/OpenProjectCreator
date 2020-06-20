#include <stdlib.h>

#include "VPCParser.h"
#include "ParserHelpers.h"
#include "ConditionEvaluator.h"

#define INSTRUCTION_PREFIX '$'
#define MACRO_PREFIX '$'
#define CONDITION_BEGIN '['
#define CONDITION_END ']'
#define SUBBLOCK_BEGIN '{'
#define SUBBLOCK_END '}'



//this is so sad

void VPC_Macro(instructionData_t* data)
{
	
}

void VPC_Configuration(instructionData_t* data)
{

}

void VPC_Folder(instructionData_t* data)
{

}

void VPC_Folder_Add(instructionData_t* data)
{

}


// hash table would be quicker
const instruction_t g_vpcInstructions[] =
{
	//Name, function, is preprocessor, argument types(MAX OF 4)
	DEFINE_INSTRUCTION("Macro", VPC_Macro, true, ArgumentType::QUOTELESS_STRING, ArgumentType::QUOTED_STRING)
	DEFINE_INSTRUCTION("Configuration", VPC_Configuration, false, ArgumentType::QUOTED_STRING, ArgumentType::SUBBLOCK)

	BEGIN_CUSTOM_INSTRUCTION("Folder", VPC_Folder, false, 1, ArgumentType::STRING, ArgumentType::SUBBLOCK)
		DEFINE_INSTRUCTION("Add", VPC_Folder_Add, false, ArgumentType::STRING)
	END_CUSTOM_INSTRUCTION()
};



void VPCParser::Execute()
{
	for (instructionData_t* data : m_instructionDataList)
		data->instruction->function(data);
}

instructionData_t* VPCParser::ParseInstruction(const char* str, size_t& i, size_t length, ErrorCode& error)
{

	// all VPC instructions must begin with a prefix
	if (str[i] == INSTRUCTION_PREFIX)
	{
		i++;

		InsetString instructionStr = ReadQuotelessString(str, i, length, error);
		if (error != ErrorCode::NO_ERROR)
		{
			ThrowException(error);
			return nullptr;
		}

		instruction_t* instruction = GetInstruction(instructionStr);
		if (!instruction)
		{
			// we failed to find the instruction.
			ThrowException(ErrorCode::UNRECOGNIZED_INSTRUCTION);
			return nullptr;
		}


		instructionData_t* instructionData = new instructionData_t;
		// if this instruction contains arguments, we have to allocate space for them and parse them
		if (instruction->argumentCount > 0)
		{
			instructionData->arguments = new value_t * [instruction->argumentCount];

			bool hasParsedCondition = false;
			bool conditionReturn = true;
			for (int argument = 0; argument < instruction->argumentCount; argument++)
			{
				SkipWhitespace(str, i, length);

				if (i >= length)
				{
					ThrowException(ErrorCode::UNEXPECTED_END_OF_FILE);
					return nullptr;
				}


				// I'm not a huge fan of this
				// this allows for placing the condition into the instruction's arguments at any point rather than just at the end of a line
				if (str[i] == CONDITION_BEGIN)
				{
					if (hasParsedCondition)
					{
						ThrowException(ErrorCode::SECONDARY_CONDITION);
						return nullptr;
					}

					conditionReturn = ParseCondition(str, i, length, error);

					// failed to parse the condition
					if (error != ErrorCode::NO_ERROR)
					{
						ThrowException(error);
						return nullptr;
					}

					// gotta clean up the instruction data. No use for it anymore since the condition was false
					if (!conditionReturn)
					{
						delete instructionData;
						instructionData = nullptr;
					}
					hasParsedCondition = true;
				}

				if (conditionReturn)
				{
					if (instruction->argumentTypes[argument] == ArgumentType::SUBBLOCK)
					{
						subblockValue_t* sbv = new subblockValue_t;
						VPCParser* parser = new VPCParser;


						if (instruction->subInstructions)
							parser->m_instructionSet = instruction->subInstructions;
						else
							parser->m_instructionSet = m_instructionSet;

						parser->ReadSubblock(str, i, length, error);

						instructionData->arguments[argument] = sbv;
					}
					else
					{
						instructionData->arguments[argument] = ParseArgument(instruction->argumentTypes[argument], str, i, length, error);
					}
				}
				else
				{
					// if the condition returned false, seek our way out of this instruction
					SeekEndOfArgument(instruction->argumentTypes[argument], str, i, length, error);
				}

				// failed to parse the argument
				if (error != ErrorCode::NO_ERROR)
				{
					ThrowException(error);
					return nullptr;
				}
			}
		}

		return instructionData;

	}
	else
	{
		// we should never hit an instruction without a $
		ThrowException(ErrorCode::UNPREFIXED_INSTRUCTION);
		return nullptr;
	}


}


void VPCParser::ReadSubblock(const char* str, size_t& i, size_t length, ErrorCode& error)
{

	if (str[i] != SUBBLOCK_BEGIN)
	{
		error = ErrorCode::SUBBLOCK_MUST_BEGIN_WITH_BRACE;
		return;
	}

	i++;

	for (; i < length; i++)
	{
		SkipWhitespace(str, i, length);

		// break loop on end of block
		if (str[i] == SUBBLOCK_END)
		{
			// gotta be one off so the next parse can work correctly
			i++;
			break;
		}


		instructionData_t* instructionData = ParseInstruction(str, i, length, error);

		if (error != ErrorCode::NO_ERROR)
		{
			ThrowException(error);
			return;
		}

		// if it's a preprocessor instruction, we need to run it now
		if (instructionData->instruction->isPreprocessor)
		{
			// run now and dump the instruction
			instructionData->instruction->function(instructionData);
			delete instructionData;
		}
		else
		{
			m_instructionDataList.push_back(instructionData);
		}

	}
	
}

void VPCParser::SeekEndOfSubblock(const char* str, size_t& i, size_t length, ErrorCode& error)
{
	if (str[i] != SUBBLOCK_BEGIN)
	{
		error = ErrorCode::SUBBLOCK_MUST_BEGIN_WITH_BRACE;
		return;
	}

	i++;

	int depth = 1;

	for (; i < length && depth != 0; i++)
	{
		char c = str[i];
		if (c == SUBBLOCK_BEGIN)
			depth++;
		else if (c == SUBBLOCK_END)
			depth--;
	}
	
	// if we escaped without a depth of 0, we hit the end of the file
	if (depth != 0)
	{
		error = ErrorCode::UNEXPECTED_END_OF_FILE;
		return;
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
						return;
					}
				}

				// if we've reached the end of the string and we still havent gotten to the end of the multi line, crash it
				ThrowException(ErrorCode::UNCLOSED_MULTILINE_COMMENT);
				return;
			}
		}
	}
}


bool VPCParser::ParseCondition(const char* str, size_t& i, size_t length, ErrorCode& error)
{
	if (str[i] != CONDITION_BEGIN)
	{
		error = ErrorCode::CONDITION_MUST_BEGIN_WITH_BRACKET;
		return false;
	}

	i++;

	std::vector<conditionChunk_t> chunkList;

	bool lastChunkWasOperator = false;

	for (; i < length; i++)
	{
		SkipWhitespace(str, i, length);

		if (i >= length)
		{
			error = ErrorCode::UNEXPECTED_END_OF_FILE;
			return false;
		}

		char c = str[i];

		if (c == CONDITION_END)
			break;

		//order must be macro, operator, number
		//this is to prevent issues with the $ being registered as an operator or a negative as a minus

		if (c == MACRO_PREFIX)
		{
			//parse out that macro
			i++;

			Macro* macro = m_macroStore.SearchForMacro(str + i, length - i);
			if (!macro)
			{
				error = ErrorCode::MACRO_NOT_FOUND;
				return false;
			}

			if (macro->GetType() != ValueType::NUMBER)
			{
				error = ErrorCode::EXPECTED_NUMBER;
				return false;
			}

			i += macro->GetKeyLength();

			numberValue_t* nv = new numberValue_t;
			//we can safely dereference this since we know it's a number
			nv->number = *macro->GetValueInt();

			conditionChunk_t valueChunk;
			valueChunk.isOperator = false;
			valueChunk.value = nv;
			

			chunkList.push_back(valueChunk);

			lastChunkWasOperator = false;
		}
		else if (IsSymbol(c) && !lastChunkWasOperator)
		{

			ConditionOperator op = SearchForOperator(str + i, length - i, error);
			if (error != ErrorCode::NO_ERROR)
				return false;

			conditionChunk_t opChunk;
			opChunk.isOperator = true;
			opChunk.operation = op;

			chunkList.push_back(opChunk);

			lastChunkWasOperator = true;
		}
		else if (IsNumber(c) || c == '-')
		{
			//parse the number

			int number = ReadNumber(str, i, length, error);
			if (error != ErrorCode::NO_ERROR)
				return false;

			numberValue_t* nv = new numberValue_t;
			nv->number = number;

			conditionChunk_t condChunk;
			condChunk.isOperator = false;
			condChunk.value = nv;

			lastChunkWasOperator = false;
		}

	}
	
	return EvaluateCondition(chunkList.data(), chunkList.size(), error);


}

instruction_t* VPCParser::GetInstruction(InsetString str)
{
	return nullptr;
}
