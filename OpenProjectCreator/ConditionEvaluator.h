#pragma once
#include "MacroStore.h"


enum class ConditionOperator
{
	NONE = -1,

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
	union
	{
		ConditionOperator operation;
		int value;
	};

	bool isOperator;
};


ConditionOperator SearchForOperator(const char* str, size_t length, ErrorCode& error);
bool EvaluateCondition(conditionChunk_t* chunkList, ErrorCode& error);