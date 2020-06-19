#include <cstring>

#include "ConditionEvaluator.h"
#include "OperatorFunctions.h"
#include "Errors.h"

#define PRECEDEDCE_COUNT 10

enum class OperatorType {
	DEAD,
	NULLARY,
	UNARY,
	BINARY,
	BINARY_SHORTCIRCUIT,
	//TERNARY,
};


typedef value_t* (*OperatorFunction)(value_t*, value_t*, ErrorCode&);
typedef value_t* (*OperatorShortCircuitFunction)(value_t*, ErrorCode&);

struct operator_t
{
	const char* key;
	size_t keyLength;
	int precedence;
	OperatorType type;
	OperatorFunction function;
	OperatorShortCircuitFunction shortCircuit;
};

#define DEFINE_OPERATOR(key, precedence, type, function) { key, precedence, sizeof(key) / sizeof(char), type, function, 0 },
#define DEFINE_SHORT_CIRCUIT_OPERATOR(key, precedence, function, shortCircuit) { key, precedence, sizeof(key) / sizeof(char), OperatorType::BINARY_SHORTCIRCUIT, function, shortCircuit },


//MUST BE IN THE SAME ORDER AS THE ENUM!
operator_t g_operators[] =
{

	DEFINE_OPERATOR("(",  0, OperatorType::NULLARY, 0)
	DEFINE_OPERATOR(")",  0, OperatorType::NULLARY, 0)

	DEFINE_OPERATOR("!",  1, OperatorType::UNARY, Operator_Logical_Not)
	DEFINE_OPERATOR("~",  1, OperatorType::UNARY, Operator_Bitwise_Not)

	DEFINE_OPERATOR("<<", 2, OperatorType::BINARY, Operator_Bitwise_LeftShift)
	DEFINE_OPERATOR(">>", 2, OperatorType::BINARY, Operator_Bitwise_RightShift)


	DEFINE_OPERATOR("<",  3, OperatorType::BINARY, Operator_LessThan)
	DEFINE_OPERATOR("<=", 3, OperatorType::BINARY, Operator_LessThanOrEqualTo)
	DEFINE_OPERATOR(">",  3, OperatorType::BINARY, Operator_GreaterThan)
	DEFINE_OPERATOR(">=", 3, OperatorType::BINARY, Operator_GreaterThanOrEqualTo)

	DEFINE_OPERATOR("==", 4, OperatorType::BINARY, Operator_EqualTo)
	DEFINE_OPERATOR("!=", 4, OperatorType::BINARY, Operator_NotEqualTo)


	DEFINE_OPERATOR("&", 5, OperatorType::BINARY, Operator_Bitwise_AND)

	DEFINE_OPERATOR("^", 6, OperatorType::BINARY, Operator_Bitwise_XOR)

	DEFINE_OPERATOR("|", 7, OperatorType::BINARY, Operator_Bitwise_OR)

	DEFINE_SHORT_CIRCUIT_OPERATOR("&&", 8, Operator_Logical_And, Operator_ShortCircuit_Logical_And)

	DEFINE_SHORT_CIRCUIT_OPERATOR("||", 9, Operator_Logical_Or, Operator_ShortCircuit_Logical_Or)


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


//overkill?
operator_t* GetOperator(conditionChunk_t op)
{
	if (op.isOperator && op.operation != ConditionOperator::NONE && op.operation != ConditionOperator::OPERATOR_COUNT)
		return &g_operators[(int)op.operation];
	else
		return nullptr;
}


inline void NullChunk(conditionChunk_t* chunk)
{
	chunk->isOperator = false;

	if (chunk->value)
		delete chunk->value;
	chunk->value = nullptr;
}

value_t* EvaluateConditionInternal(conditionChunk_t* chunkList, size_t chunkCount, bool isRoot, ErrorCode& error)
{

	//i really don't want to be dropping and moving memory around, be it with my own implementation or with a vector. This allows us to just access the last valid value
	int lastValidValueIndex = -1;

	//we need to store the pos of the short circuit so that if we hit it again, we don't trip the precedence reset 
	int shortCircuitIndex = -1;

	bool lastWasOperator = false;

	for(int precedence = 0; precedence < PRECEDEDCE_COUNT; precedence++)
	{
		for (int i = 0; i < chunkCount; i++)
		{
			if (chunkList[i].isOperator)
			{
				lastWasOperator = true;

				operator_t* op = GetOperator(chunkList[i]);
				ConditionOperator opID = chunkList[i].operation;


				if (lastWasOperator && opID != ConditionOperator::OPEN_PARENTHESIS)
				{
					error = ErrorCode::UNEXPECTED_OPERATOR;
					return;
				}

				//don't want to execute something out of order!
				if (op->precedence != precedence)
				{
					//We're going to have to come back and precedence check the next stuff later. Right now, we need to run until we can check the short circuit
					if (op->type == OperatorType::BINARY_SHORTCIRCUIT)
					{
						
						break;
					}

					continue;
				}

				switch (op->type)
				{
				case OperatorType::NULLARY:

					if (opID == ConditionOperator::OPEN_PARENTHESIS)
					{
						value_t* val = EvaluateConditionInternal(chunkList + i + 1, chunkCount - i - 1, false, error);

						if (error != ErrorCode::NO_ERROR)
							return;

						//convert this into a value
						chunkList[i].isOperator = false;
						chunkList[i].value = val;

						lastWasOperator = false;
					}
					else if (opID == ConditionOperator::CLOSE_PARENTHESIS)
					{
						//we should never hit a ) as root! syntax error!
						if (isRoot)
						{
							error = ErrorCode::UNEXPECTED_CLOSING_PARENTHESIS;
							return;
						}

						//convert this to a blank
						NullChunk(&chunkList[i]);
						return;
					}
					break;
				case OperatorType::UNARY:
					//a unary operator needs just the operator to the right
					if (i == chunkCount - 1 || chunkList[i + 1].isOperator)
					{
						error = ErrorCode::UNEXPECTED_OPERATOR;
						return;
					}

					value_t* ret = op->function(nullptr, chunkList[i + 1].value, error);

					if (error != ErrorCode::NO_ERROR)
						return;

					//update the value and null the operator
					lastValidValueIndex = i + 1;
					chunkList[i + 1].value = ret;

					NullChunk(&chunkList[i]);


				case OperatorType::BINARY:

					//a binary operator needs two value arguments on both sides
					if (i == 0 || i == chunkCount - 1 || lastValidValueIndex == -1 || chunkList[i + 1].isOperator)
					{
						error = ErrorCode::UNEXPECTED_OPERATOR;
						return;
					}

					value_t* ret = op->function(chunkList[lastValidValueIndex].value, chunkList[i + 1].value, error);

					if (error != ErrorCode::NO_ERROR)
						return;

					//reuse the last valid's value and null the operator and next value
					chunkList[lastValidValueIndex].value = ret;
					
					NullChunk(&chunkList[i]);
					NullChunk(&chunkList[i+1]);

				case OperatorType::BINARY_SHORTCIRCUIT:

					//a binary operator needs two value arguments on both sides
					if (i == 0 || i == chunkCount - 1 || lastValidValueIndex == -1 || chunkList[i + 1].isOperator)
					{
						error = ErrorCode::UNEXPECTED_OPERATOR;
						return;
					}

					//no short circuits ran? check if we can short
					if (shortCircuitIndex == -1)
					{
						//but since we're short circuiting, we only need the left side...
						value_t* ret = op->shortCircuit(chunkList[lastValidValueIndex].value, error);

						if (error != ErrorCode::NO_ERROR)
							return;

						//if this was successful, we have to null out everything to the right that we can ignore
						if (ret)
						{
							
							//we leave the last valid index the same, null the operator chunk, and make the old valid value our new fancy value
							chunkList[lastValidValueIndex].value = ret;
							
							NullChunk(&chunkList[i]);


							for (; i < chunkCount; i++)
							{
								//we can reuse this since we're not using it again
								op = GetOperator(chunkList[i]);

								if (op->precedence <= precedence)
								{
									NullChunk(&chunkList[i]);
								}
							}
							

							//we have to reset the precedence and rescan now...
							precedence = 0;
							//we don't have to set i to 0 since everything before us should be a barren wasteland of nulled out values
							break;
						}
					}
					else
					{
						//if we reach here, the short failed earlier so we're just treating it like a normal operation

						value_t* ret = op->function(chunkList[lastValidValueIndex].value, chunkList[i + 1].value, error);

						if (error != ErrorCode::NO_ERROR)
							return;

						//reuse the last valid's value and null the operator and next value
						chunkList[lastValidValueIndex].value = ret;

						NullChunk(&chunkList[i]);
						NullChunk(&chunkList[i + 1]);
					}
				default:
					break;
					error = ErrorCode::NOT_IMPLEMENTED;
					return;
				}


				continue;
			}
			else
			{
				if (chunkList[i].value)
				{
					lastValidValueIndex = i;
					if (!lastWasOperator)
					{
						error = ErrorCode::UNEXPECTED_VALUE;
					}
				}
			}
		



		}
	}

	if (lastValidValueIndex == -1)
	{
		//what?
		error = ErrorCode::INVALID_CONDITION;
	}


	//the last valid should be the end result
	return chunkList[lastValidValueIndex].value;
}



bool EvaluateCondition(conditionChunk_t* chunkList, size_t chunkCount, ErrorCode& error)
{
	//let's make a copy so we don't ruin everything in case the caller wants to use that array again

	conditionChunk_t* chunks = new conditionChunk_t[chunkCount];
	memcpy(chunks, chunkList, sizeof(conditionChunk_t) * chunkCount);

	//we dont want to constantly copy this array, so we give the actual evaulator a different name 
	value_t* ret = EvaluateConditionInternal(chunks, chunkCount, true, error);

	if (error != ErrorCode::NO_ERROR)
		return;

	if (!ret || ret->type != ValueType::BOOLEAN)
	{
		//what
		error = ErrorCode::INVALID_CONDITION;
		return false;
	}

	booleanValue_t* boolRet = (booleanValue_t*)ret;

	return boolRet->boolean;

}