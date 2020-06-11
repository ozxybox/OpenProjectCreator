#include "BaseParser.h"


void BaseParser::Parse(const char* str, size_t length)
{

}

// parse a value out of a string depending on its type and return it
value_t* BaseParser::ParseArgument(ArgumentType type, const char* str, size_t& i, size_t length, ErrorCode* error)
{

	switch (type)
	{
	case ArgumentType::ARRAY:
		break;

	case ArgumentType::NUMBER:
		numberValue_t* nv = new numberValue_t;
		nv->number = ReadNumber(str, i, length, error);
		return nv;

	case ArgumentType::QUOTED_STRING:
		stringValue_t* sv = new stringValue_t;
		sv->string = ReadQuotedString(str, i, length, error);
		return sv;

	case ArgumentType::QUOTELESS_STRING:
		stringValue_t* sv = new stringValue_t;
		sv->string = ReadQuotelessString(str, i, length, error);
		return sv;

	case ArgumentType::STRING:
		stringValue_t* sv = new stringValue_t;
		sv->string = ReadString(str, i, length, error);
		return sv;

	case ArgumentType::SUB_BLOCK:
		break;

	default:
		// we should never hit an argument without a type
		*error = ErrorCode::COMPILED_WITH_TYPELESS_ARGUMENT;
		return nullptr;

	// if we reach this, the something's totally broken, or the type is unimplemented
	*error = ErrorCode::UNIMPLEMENTED;
	return nullptr;
}
