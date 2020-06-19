#pragma once
#include "ParserHelpers.h"


enum class ValueType
{
	STRING,
	NUMBER,
	BOOLEAN,
	//contains just a single element
	ARRAY,
	//can contain instructions
	SUBBLOCK,
};


struct value_t
{
	ValueType type;
};

struct stringValue_t : public value_t
{
	ValueType type = ValueType::STRING;
	InsetString string;
};

struct numberValue_t : public value_t
{
	ValueType type = ValueType::NUMBER;
	int number;
};

struct booleanValue_t : public value_t
{
	ValueType type = ValueType::BOOLEAN;
	bool boolean;
};

struct valueArray_t
{
	~valueArray_t() { if (array) delete[] array; }
	value_t* array = 0;
	size_t length = 0;
};

struct arrayValue_t : public value_t
{
	ValueType type = ValueType::ARRAY;
	valueArray_t array;
};

class BaseParser;
struct subblockValue_t : public value_t
{
	~subblockValue_t() { if (subblock) delete subblock; }
	ValueType type = ValueType::SUBBLOCK;
	BaseParser* subblock = 0;
};