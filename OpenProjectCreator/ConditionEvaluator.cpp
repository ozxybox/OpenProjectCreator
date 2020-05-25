#include "ConditionEvaluator.h"

class Operator
{
	enum class SingleArgument
	{
		LOGICAL_NOT,
		BITWISE_NOT,

		END
	};

	enum class DoubleArgument
	{

		BITSHIFT_LEFT = (int)Operator::SingleArgument::END,
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

		END
	};

	const int OperatorCount = (int)Operator::DoubleArgument::END;
};




bool EvaluateCondition(const char* str, MacroStore* macroStore)
{
	return false;
}