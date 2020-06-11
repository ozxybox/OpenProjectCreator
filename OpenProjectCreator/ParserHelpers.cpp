#include "ParserHelpers.h"
#include "BaseParser.h"

// hopefully nothing should have to change these...
#define BLOCK_START        '{'
#define BLOCK_END          '}'
#define STRING_QUOTE       '"'

insetString_t ReadQuotelessString(const char* str, size_t& i, size_t length, ErrorCode* error = 0)
{
	insetString_t inset;

	//first char must not be a symbol that's used by other types or whitespace
	char c = str[i];
	if (c == BLOCK_START || c == BLOCK_END || c == STRING_QUOTE || IsWhitespace(c))
	{
		inset.length = 0;
		inset.string = 0;

		if (error)
			*error = ErrorCode::INVALID_QUOTELESS_STRING;

		return inset;
	}



	inset.string = str + i;
	int start = i;

	i++;

	for (c = str[i]; i < length; i++, c = str[i])
	{

		if (IsWhitespace(c) || c == BLOCK_START || c == BLOCK_END || c == STRING_QUOTE)
			break;
	}

	inset.length = i - start;

	return inset;
}

insetString_t ReadQuotedString(const char* str, size_t& i, size_t length, ErrorCode* error = 0)
{
	insetString_t inset;

	//first character should be a quote
	if (str[i] != STRING_QUOTE)
	{
		inset.length = 0;
		inset.string = 0;

		if (error)
			*error = ErrorCode::STRING_NOT_BEGUN_WITH_QUOTE;

		return inset;
	}
	i++;

	int start = i;

	for (; i < length && str[i] != STRING_QUOTE; i++);

	// if we've hit the end of the string and havent gotten our quote, return null
	if (i >= length)
	{
		inset.length = 0;
		inset.string = 0;
		return inset;
	}

	inset.string = str + i;
	inset.length = i - start;

	return inset;
}

insetString_t ReadString(const char* str, size_t& i, size_t length, ErrorCode* error = 0)
{
	if (str[i] == STRING_QUOTE)
		return ReadQuotedString(str, i, length, error);
	return ReadQuotelessString(str, i, length, error);
}

int ReadNumber(const char* str, size_t& i, size_t length, ErrorCode* error = 0)
{
	bool isNegative = false;

	//overengineered? maybe... C++ lets you do +-+-1, so I felt this was appropriate
	if (str[i] == '+' || str[i] == '-')
		for (; i < length; i++)
		{
			switch (str[i])
			{
			case '+':
				isNegative = false;
				continue;
			case '-':
				isNegative = !isNegative;
				continue;
			}
			break;
		}

	//end of string with no numbers
	if (i >= length)
	{
		if (error)
			*error = ErrorCode::NUMBER_WITH_NO_NUMBERS;

		return 0;
	}

	//end of sign with no numbers
	if (!IsNumber(str[i]))
	{
		if (error)
			*error = ErrorCode::NUMBER_WITH_NO_NUMBERS;

		return 0;
	}

	//little shortcut
	//since we know for a fact that str[i] is currently a number, we can skip double checking if it's actually a number
	int number = str[i] - '0';
	i++;

	for (; i < length && IsNumber(str[i]); i++)
	{
		//move the decimal right one
		number *= 10;

		number += str[i] - '0';
	}

	return number * (isNegative ? -1 : 1);
}
