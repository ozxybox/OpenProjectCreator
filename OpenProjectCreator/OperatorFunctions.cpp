#include <cstring>

#include "OperatorFunctions.h"

#include "Values.h"
#include "Errors.h"

//is there a good way to shrink this?
//might be worth it to migrate these into the actual value structs themselves 

value_t* Operator_Logical_Not(value_t* left, value_t* right, ErrorCode& error)
{

	bool input;
	if (right->type == ValueType::BOOLEAN)
	{
		booleanValue_t* bv = (booleanValue_t*)right;
		input = bv->boolean;
	}
	else if (right->type == ValueType::NUMBER)
	{
		numberValue_t* nv = (numberValue_t*)right;
		input = nv->number;
	}
	else
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}


	booleanValue_t* boolRet = new booleanValue_t;

	boolRet->boolean = !input;

	return boolRet;
}

value_t* Operator_Bitwise_Not(value_t* left, value_t* right, ErrorCode& error)
{

	if (right->type != ValueType::NUMBER)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}


	numberValue_t* numberRet = new numberValue_t;

	numberRet->number = ~static_cast<numberValue_t*>(right)->number;

	return numberRet;
}


value_t* Operator_Bitwise_LeftShift(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != ValueType::NUMBER || right->type != ValueType::NUMBER)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}

	numberValue_t* numberRet = new numberValue_t;

	numberRet->number = static_cast<numberValue_t*>(left)->number << static_cast<numberValue_t*>(right)->number;

	return numberRet;
}

value_t* Operator_Bitwise_RightShift(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != ValueType::NUMBER || right->type != ValueType::NUMBER)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}

	numberValue_t* numberRet = new numberValue_t;

	numberRet->number = static_cast<numberValue_t*>(left)->number >> static_cast<numberValue_t*>(right)->number;

	return numberRet;
}

value_t* Operator_LessThan(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != ValueType::NUMBER || right->type != ValueType::NUMBER)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}

	booleanValue_t* ret = new booleanValue_t;

	ret->boolean = static_cast<numberValue_t*>(left)->number < static_cast<numberValue_t*>(right)->number;

	return ret;
}

value_t* Operator_LessThanOrEqualTo(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != ValueType::NUMBER || right->type != ValueType::NUMBER)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}

	booleanValue_t* ret = new booleanValue_t;

	ret->boolean = static_cast<numberValue_t*>(left)->number <= static_cast<numberValue_t*>(right)->number;

	return ret;
}

value_t* Operator_GreaterThan(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != ValueType::NUMBER || right->type != ValueType::NUMBER)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}

	booleanValue_t* ret = new booleanValue_t;

	ret->boolean = static_cast<numberValue_t*>(left)->number > static_cast<numberValue_t*>(right)->number;

	return ret;
}

value_t* Operator_GreaterThanOrEqualTo(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != ValueType::NUMBER || right->type != ValueType::NUMBER)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}

	booleanValue_t* ret = new booleanValue_t;

	ret->boolean = static_cast<numberValue_t*>(left)->number >= static_cast<numberValue_t*>(right)->number;

	return ret;
}


value_t* Operator_EqualTo(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != right->type)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return;
	}

	bool ret = false;

	switch (left->type)
	{
	case ValueType::NUMBER:
		ret = static_cast<numberValue_t*>(left)->number == static_cast<numberValue_t*>(right)->number;
		break; 
	case ValueType::BOOLEAN:
		ret = static_cast<booleanValue_t*>(left)->boolean == static_cast<booleanValue_t*>(right)->boolean;
		break;
	case ValueType::STRING:
		stringValue_t* leftString = static_cast<stringValue_t*>(left);
		stringValue_t* rightString = static_cast<stringValue_t*>(right);

		//if the lengths are equal, the values aren't equal for sure
		if (leftString->string.length != rightString->string.length)
		{
			ret = false;
			break;
		}

		ret = strncmp(leftString->string.string, rightString->string.string, leftString->string.length) == 0;

		break;
	default:
		error = ErrorCode::NOT_IMPLEMENTED;
		return;
	}

	booleanValue_t* boolRet = new booleanValue_t;
	boolRet->boolean = ret;

	return boolRet;
}


value_t* Operator_NotEqualTo(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != right->type)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return;
	}

	bool ret = false;

	switch (left->type)
	{
	case ValueType::NUMBER:
		ret = static_cast<numberValue_t*>(left)->number != static_cast<numberValue_t*>(right)->number;
		break;
	case ValueType::BOOLEAN:
		ret = static_cast<booleanValue_t*>(left)->boolean != static_cast<booleanValue_t*>(right)->boolean;
		break;
	case ValueType::STRING:
		stringValue_t* leftString = static_cast<stringValue_t*>(left);
		stringValue_t* rightString = static_cast<stringValue_t*>(right);

		//if the lengths are equal, the values aren't equal for sure
		if (leftString->string.length != rightString->string.length)
		{
			ret = true;
			break;
		}

		ret = strncmp(leftString->string.string, rightString->string.string, leftString->string.length) != 0;

		break;
	default:
		error = ErrorCode::NOT_IMPLEMENTED;
		return;
	}

	booleanValue_t* boolRet = new booleanValue_t;
	boolRet->boolean = ret;

	return boolRet;
}

value_t* Operator_Bitwise_AND(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != ValueType::NUMBER || right->type != ValueType::NUMBER)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}

	numberValue_t* numberRet = new numberValue_t;

	numberRet->number = static_cast<numberValue_t*>(left)->number & static_cast<numberValue_t*>(right)->number;

	return numberRet;
}

value_t* Operator_Bitwise_XOR(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != ValueType::NUMBER || right->type != ValueType::NUMBER)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}

	numberValue_t* numberRet = new numberValue_t;

	numberRet->number = static_cast<numberValue_t*>(left)->number ^ static_cast<numberValue_t*>(right)->number;

	return numberRet;
}

value_t* Operator_Bitwise_OR(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != ValueType::NUMBER || right->type != ValueType::NUMBER)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}

	numberValue_t* numberRet = new numberValue_t;

	numberRet->number = static_cast<numberValue_t*>(left)->number | static_cast<numberValue_t*>(right)->number;

	return numberRet;
}


value_t* Operator_Logical_And(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != ValueType::BOOLEAN || right->type != ValueType::BOOLEAN)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}

	booleanValue_t* ret = new booleanValue_t;

	ret->boolean = static_cast<booleanValue_t*>(left)->boolean && static_cast<booleanValue_t*>(right)->boolean;

	return ret;
}

value_t* Operator_ShortCircuit_Logical_And(value_t* left, ErrorCode& error)
{

	if (left->type != ValueType::BOOLEAN)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}


	//if this is false, we can instantly skip the right side
	if (!static_cast<booleanValue_t*>(left)->boolean)
	{
		booleanValue_t* ret = new booleanValue_t;
		ret->boolean = false;
		return ret;
	}

	//else, nullptr to show the short failed
	return nullptr;
}

value_t* Operator_Logical_Or(value_t* left, value_t* right, ErrorCode& error)
{

	if (left->type != ValueType::BOOLEAN || right->type != ValueType::BOOLEAN)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}

	booleanValue_t* ret = new booleanValue_t;

	ret->boolean = static_cast<booleanValue_t*>(left)->boolean || static_cast<booleanValue_t*>(right)->boolean;

	return ret;
}


value_t* Operator_ShortCircuit_Logical_Or(value_t* left, ErrorCode& error)
{

	if (left->type != ValueType::BOOLEAN)
	{
		error = ErrorCode::INVALID_CONDITION_PARAMETER;
		return nullptr;
	}


	//if this is true, we can instantly skip the right side
	if (static_cast<booleanValue_t*>(left)->boolean)
	{
		booleanValue_t* ret = new booleanValue_t;
		ret->boolean = true;
		return ret;
	}

	//else, nullptr to show the short failed
	return nullptr;
}