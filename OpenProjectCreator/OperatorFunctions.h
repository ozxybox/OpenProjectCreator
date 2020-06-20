#pragma once

// as to not destroy OperatorFunctions.cpp, this file holds the operator functions...

#include <cstring>

#include "OperatorFunctions.h"

#include "Values.h"
#include "Errors.h"

//is there a good way to shrink this?
//might be worth it to migrate these into the actual value structs themselves 

value_t* Operator_Multiply(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_Divide(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_Remainder(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_Add(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_Subtract(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_Logical_Not(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_Bitwise_Not(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_Bitwise_LeftShift(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_Bitwise_RightShift(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_LessThan(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_LessThanOrEqualTo(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_GreaterThan(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_GreaterThanOrEqualTo(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_EqualTo(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_NotEqualTo(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_Bitwise_AND(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_Bitwise_XOR(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_Bitwise_OR(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_Logical_And(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_ShortCircuit_Logical_And(value_t* left, ErrorCode& error);
value_t* Operator_Logical_Or(value_t* left, value_t* right, ErrorCode& error);
value_t* Operator_ShortCircuit_Logical_Or(value_t* left, ErrorCode& error);

