#pragma once
#include <cstddef>
#include "ParserHelpers.h"
#include "BaseParser.h" // for `delete subblock`


enum class ValueType
{
	NONE,
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
	virtual ~value_t() {}
	
	virtual value_t* Copy() { return new value_t; }
	
	ValueType type = ValueType::NONE;
};

struct stringValue_t : public value_t
{
	stringValue_t() { type = ValueType::STRING; }
	stringValue_t(InsetString value) : stringValue_t() { string = value; }
	
	virtual value_t* Copy() { return new stringValue_t(string); }
	
	InsetString string;
};

struct numberValue_t : public value_t
{
	numberValue_t() { type = ValueType::NUMBER; }
	numberValue_t(int value) : numberValue_t() { number = value; }

	virtual value_t* Copy() { return new numberValue_t(number); }
	
	int number;
};

struct booleanValue_t : public value_t
{
	booleanValue_t() { type = ValueType::BOOLEAN; }
	booleanValue_t(bool value) : booleanValue_t() { boolean = value; }

	virtual value_t* Copy() { return new booleanValue_t(boolean); }
	
	bool boolean;
};

struct valueArray_t
{
	~valueArray_t() { if (array) delete[] array; }

	valueArray_t Copy();
	
	value_t* array = 0;
	size_t length = 0;
};

struct arrayValue_t : public value_t
{
	arrayValue_t() { type = ValueType::ARRAY; }
	arrayValue_t(valueArray_t value) : arrayValue_t() { array = value; }

	virtual value_t* Copy() { return new arrayValue_t(array.Copy()); }
	
	valueArray_t array;
};

class BaseParser;
struct subblockValue_t : public value_t
{
	subblockValue_t() { type = ValueType::SUBBLOCK; }
	subblockValue_t(BaseParser* value) : subblockValue_t() { subblock = value; }
	~subblockValue_t() { if (subblock) delete subblock; }

	virtual value_t* Copy();
	
	BaseParser* subblock = 0;
};