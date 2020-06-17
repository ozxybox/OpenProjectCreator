#include <cstring>

#include "ConditionEvaluator.h"
#include "Errors.h"


typedef value_t* (*OperatorFunction)(value_t*, value_t*, ErrorCode&);
struct operator_t
{
	const char* key;
	size_t keyLength;
	OperatorFunction function;
};

#define DEFINE_OPERATOR(key, function) { key, sizeof(key) / sizeof(char), function },



value_t* Operator_Logical_Not(value_t* left, value_t* right, ErrorCode& error)
{
	error = ErrorCode::NOT_IMPLEMENTED;
	return nullptr;
}


//MUST BE IN THE SAME ORDER AS THE ENUM!
operator_t g_operators[] =
{
	DEFINE_OPERATOR("!", Operator_Logical_Not)
	

	{0}
};



//longest condition is the one that gets picked. This is to solve issues with stuff like != and !
ConditionOperator SearchForOperator(const char* str, size_t length, ErrorCode& error)
{
	operator_t* longestOp = nullptr;
	int longestIndex = -1;

	for (int i = 0; i < (int)ConditionOperator::OPERATOR_COUNT; i++)
	{
		//if we hit the 0, we have more enums than structs in this array...
		if (!g_operators->key)
		{
			error = ErrorCode::NOT_IMPLEMENTED;
			return ConditionOperator::NONE;
		}

		operator_t* op = &g_operators[i];

		if (op->keyLength < length)
			continue;

		if (strncmp(str, op->key, op->keyLength) == 0)
		{
			if (longestOp == nullptr || longestOp->keyLength <= op->keyLength)
			{
				longestOp = op;
				longestIndex = i;
			}
		}

	}

	if(longestOp == nullptr)
		error = ErrorCode::OPERATOR_NOT_FOUND;

	return (ConditionOperator)longestIndex;

}

bool EvaluateCondition(conditionChunk_t* chunkList, ErrorCode& error)
{
	error = ErrorCode::NOT_IMPLEMENTED;
	return false;
}
