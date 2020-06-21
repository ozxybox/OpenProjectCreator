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

	const char* name = static_cast<stringValue_t*>(data->arguments[0])->string.Copy();
	const char* value = static_cast<stringValue_t*>(data->arguments[1])->string.Copy();
	printf("$Macro %s %s\n", name, value);
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
static size_t g_vpcInstructionCount = sizeof(g_vpcInstructions) / sizeof(instruction_t);


BaseParser* VPCParser::Copy()
{
	VPCParser* vpcParser = new VPCParser();
	vpcParser->m_instructionSet = m_instructionSet;
	vpcParser->m_macroStore = m_macroStore;
	vpcParser->m_instructionDataList = m_instructionDataList;

	return vpcParser;
}

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

		const instruction_t* instruction = GetInstruction(instructionStr);
		if (!instruction)
		{
			// we failed to find the instruction.
			ThrowException(ErrorCode::UNRECOGNIZED_INSTRUCTION);
			return nullptr;
		}


		instructionData_t* instructionData = new instructionData_t;
		instructionData->instruction = instruction;

		bool conditionReturn = true;
		bool hasParsedCondition = false;

		// if this instruction contains arguments, we have to allocate space for them and parse them
		if (instruction->argumentCount > 0)
		{
			instructionData->arguments = new value_t * [instruction->argumentCount];

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
						return nullptr;

					hasParsedCondition = true;


					// this wasn't an argument so we have to drop argument by one
					argument--;
					// ParseCondition ends on ], so we have to increment i by one
					i++;
					continue;
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
					instructionData->arguments[argument] = nullptr;
				}

				// failed to parse the argument
				if (error != ErrorCode::NO_ERROR)
				{
					ThrowException(error);
					return nullptr;
				}
			}
		}

		if (!hasParsedCondition)
		{
			// we didn't hit a condition in the arguments, but there might be one lingering right next to us.. Let's check!


			// make a new i so we don't murder the old one
			size_t tempI = i + 1;
			SkipWhitespace(str, tempI, length);

			if (tempI < length)
			{

				if (str[tempI] == CONDITION_BEGIN)
				{
					// hohoho! Lucky us, a condition!

					// set i to our temp i since we know for sure that this will be a condition for sure and we aren't killing our output i
					i = tempI;

					conditionReturn = ParseCondition(str, i, length, error);
				}
			}
		}

		if (!conditionReturn)
		{
			// the condition failed. dump the data, return null, and throw no errors as this is on purpose
			delete instructionData;
			return nullptr;
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

				bool closedMultiline = false;
				for(; i + 1 < length; i++)
				{
					if (str[i] == '*' && str[i + 1] == '/')
					{
						closedMultiline = true;
						break;
					}
				}

				if (!closedMultiline)
				{
					// if we've reached the end of the string and we still havent gotten to the end of the multi line, crash it
					ThrowException(ErrorCode::UNCLOSED_MULTILINE_COMMENT);
					return;
				}
				continue;
			}
		}
		if (!IsWhitespace(str[i]))
			break;
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

	std::vector<conditionChunk_t*> chunkList;


	bool lastChunkWasOperator = false;
	bool lastChunkWasValue = false;

	for (; i < length; )
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

			if (lastChunkWasValue)
			{
				error = ErrorCode::UNEXPECTED_VALUE;
				return false;
			}
			lastChunkWasValue = true;
			lastChunkWasOperator = false;

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

			conditionChunk_t* valueChunk = new conditionChunk_t;
			valueChunk->isOperator = false;
			valueChunk->value = nv;

			chunkList.push_back(valueChunk);

		}
		else if (c == '(' || c == ')')
		{
			//improve this!

			if (lastChunkWasValue)
			{
				error = ErrorCode::UNEXPECTED_VALUE;
				return false;
			}
			lastChunkWasValue = true;
			lastChunkWasOperator = false;


			ConditionOperator op;

			if (c == '(')
				op = ConditionOperator::OPEN_PARENTHESIS;
			else
				op = ConditionOperator::CLOSE_PARENTHESIS;

			conditionChunk_t* opChunk = new conditionChunk_t;
			opChunk->isOperator = true;
			opChunk->operation = op;

			chunkList.push_back(opChunk);

		}
		else if (IsSymbol(c))
		{

			if (lastChunkWasOperator)
			{
				error = ErrorCode::UNEXPECTED_OPERATOR;
				return false;
			}
			lastChunkWasOperator = true;
			lastChunkWasValue = false;


			ConditionOperator op = SearchForOperator(str + i, length - i, error);
			if (error != ErrorCode::NO_ERROR)
				return false;

			conditionChunk_t* opChunk = new conditionChunk_t;
			opChunk->isOperator = true;
			opChunk->operation = op;

			chunkList.push_back(opChunk);

			// skip to the end of the operator
			i += GetOperatorLength(op);
		}
		else if (IsNumber(c) || c == '-')
		{

			if (lastChunkWasValue)
			{
				error = ErrorCode::UNEXPECTED_VALUE;
				return false;
			}
			lastChunkWasValue = true;
			lastChunkWasOperator = false;

			//parse the number

			int number = ReadNumber(str, i, length, error);

			if (error != ErrorCode::NO_ERROR)
				return false;

			numberValue_t* nv = new numberValue_t;
			nv->number = number;

			conditionChunk_t* condChunk = new conditionChunk_t;
			condChunk->isOperator = false;
			condChunk->value = nv;


			chunkList.push_back(condChunk);


		}
		else
		{
			error = ErrorCode::UNKNOWN_TYPE;

			for (int i = 0; i < chunkList.size(); i++)
				delete chunkList[i];

			return false;
		}
	}
	
	bool ret = EvaluateCondition(chunkList.data(), chunkList.size(), error);


	for (int i = 0; i < chunkList.size(); i++)
		delete chunkList[i];

	return ret;

}

const instruction_t* VPCParser::GetInstruction(InsetString str)
{
	for (int i = 0; i < g_vpcInstructionCount; i++)
		if (g_vpcInstructions[i].nameLength == str.length && strncmp(g_vpcInstructions[i].name, str.string, str.length) == 0)
			return &g_vpcInstructions[i];
	return nullptr;
}
