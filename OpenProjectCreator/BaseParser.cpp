#include "BaseParser.h"
#include "Values.h"

void BaseParser::Parse(const char* str, size_t length)
{
	ErrorCode error = ErrorCode::NO_ERROR;
	for (size_t i = 0; i < length; i++)
	{
		SkipWhitespace(str, i, length);

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
			CacheInstructionData(instructionData);
		}

	}

}

// parse a value out of a string depending on its type and return it
value_t* BaseParser::ParseArgument(ArgumentType type, const char* str, size_t& i, size_t length, ErrorCode& error)
{

	switch (type)
	{
		case ArgumentType::ARRAY:
		{
			break;
		}

		case ArgumentType::NUMBER:
		{
			numberValue_t* nv = new numberValue_t;
			nv->number = ReadNumber(str, i, length, error);
			return nv;
		}

		case ArgumentType::QUOTED_STRING:
		{
			stringValue_t* sv = new stringValue_t;
			sv->string = ReadQuotedString(str, i, length, error);
			return sv;
		}

		case ArgumentType::QUOTELESS_STRING:
		{
			stringValue_t* sv = new stringValue_t;
			sv->string = ReadQuotelessString(str, i, length, error);
			return sv;
		}

		case ArgumentType::STRING:
		{
			stringValue_t* sv = new stringValue_t;
			sv->string = ReadString(str, i, length, error);
			return sv;
		}

		case ArgumentType::SUBBLOCK:
		{
			break;
		}

		default:
		{
			// we should never hit an argument without a type
			error = ErrorCode::COMPILED_WITH_TYPELESS_ARGUMENT;
			return nullptr;
		}
	}

	// if we reach this, the something's totally broken, or the type is unimplemented
	error = ErrorCode::NOT_IMPLEMENTED;
	return nullptr;
}


void BaseParser::SeekEndOfArgument(ArgumentType type, const char* str, size_t & i, size_t length, ErrorCode& error)
{
	switch (type)
	{
	case ArgumentType::ARRAY:
		break;

	case ArgumentType::NUMBER:
		SeekEndOfNumber(str, i, length, error);
		return;

	case ArgumentType::QUOTED_STRING:
		SeekEndOfQuotedString(str, i, length, error);
		return;

	case ArgumentType::QUOTELESS_STRING:
		SeekEndOfQuotelessString(str, i, length, error);
		return;

	case ArgumentType::STRING:
		SeekEndOfString(str, i, length, error);
		return;

	case ArgumentType::SUBBLOCK:
		SeekEndOfSubblock(str, i, length, error);
		break;

	default:
		// we should never hit an argument without a type
		error = ErrorCode::COMPILED_WITH_TYPELESS_ARGUMENT;
		return;
	}

	// if we reach this, then something's totally broken, or the type is unimplemented
	error = ErrorCode::NOT_IMPLEMENTED;
}
