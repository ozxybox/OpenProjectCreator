#pragma once
#include "MacroStore.h"


enum class ConditionOperator
{
	NONE = -1,

	// not really an operator, but a chunk for sure!
	OPEN_PARENTHESIS,
	CLOSE_PARENTHESIS,

	MULTIPLY,
	DIVIDE,
	REMAINDER,

	ADD,
	SUBTRACT,

	LOGICAL_NOT,
	BITWISE_NOT,

	BITSHIFT_LEFT,
	BITSHIFT_RIGHT,

	LESS_THAN,
	LESS_THAN_OR_EQUAL_TO,
	GREATER_THAN,
	GREATER_THAN_OR_EQUAL_TO,

	EQUAL_TO,
	NOT_EQUAL_TO,

	BITWISE_AND,

	BITWISE_XOR,

	BITWISE_OR,

	LOGICAL_AND,

	LOGICAL_OR,


	OPERATOR_COUNT,
};

struct conditionChunk_t
{
	~conditionChunk_t() { if (!isOperator && value) delete value; }
	union
	{
		ConditionOperator operation;
		value_t* value;
	};

	bool isOperator;
};


ConditionOperator SearchForOperator(const char* str, size_t length, ErrorCode& error);
bool EvaluateCondition(conditionChunk_t* chunkList, size_t chunkCount, ErrorCode& error);