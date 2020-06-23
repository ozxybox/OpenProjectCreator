#include "ParserHelpers.h"
#include "BaseParser.h"
#include <cstddef>
#include <cstring>

// hopefully nothing should have to change these...
#define BLOCK_START        '{'
#define BLOCK_END          '}'
#define CONDITION_START    '['
#define CONDITION_END      ']'
#define STRING_QUOTE       '"'



bool operator==(const InsetString& lhs, const InsetString& rhs)
{
	// if the lengths aren't the same, the string isn't for sure.
	if(lhs.length != rhs.length)
		return false;

	return strncmp(lhs.string, rhs.string, lhs.length) == 0;
}

bool operator!=(const InsetString& lhs, const InsetString& rhs)
{
	// if the lengths aren't the same, the string isn't for sure.
	if (lhs.length != rhs.length)
		return true;

	return strncmp(lhs.string, rhs.string, lhs.length) != 0;
}
char* InsetString::Copy()
{

	char* copy = new char[length + 1];
	copy[length] = 0;
	memcpy(copy, string, length);
	return copy;
}



////////////////
// READ VALUE //
////////////////




InsetString ReadQuotelessString(const char* str, size_t& i, size_t length, ErrorCode& error)
{
	InsetString inset;

	//first char must not be a symbol that's used by other types or whitespace
	char c = str[i];
	if (c == BLOCK_START || c == BLOCK_END || c == STRING_QUOTE || c == CONDITION_START || c == CONDITION_END || IsWhitespace(c))
	{
		inset.length = 0;
		inset.string = 0;

		error = ErrorCode::INVALID_QUOTELESS_STRING;

		return inset;
	}



	size_t start = i;

	i++;

	for (c = str[i]; i < length; i++, c = str[i])
	{

		if (IsWhitespace(c) || c == BLOCK_START || c == BLOCK_END || c == STRING_QUOTE || c == CONDITION_START || c == CONDITION_END  )
			break;
	}

	inset.string = str + start;
	inset.length = i - start;

	return inset;
}

InsetString ReadQuotedString(const char* str, size_t& i, size_t length, ErrorCode& error)
{
	InsetString inset;

	//first character should be a quote
	if (str[i] != STRING_QUOTE)
	{
		inset.length = 0;
		inset.string = 0;

		error = ErrorCode::STRING_NOT_BEGUN_WITH_QUOTE;

		return inset;
	}
	i++;

	size_t start = i;

	for (; i < length && str[i] != STRING_QUOTE; i++);

	// if we've hit the end of the string and havent gotten our quote, return null
	if (i >= length)
	{
		inset.length = 0;
		inset.string = 0;
		error = ErrorCode::INCOMPLETE_QUOTED_STRING;

		return inset;
	}

	inset.string = str + start;
	inset.length = i - start;

	// this should end on the character after the string.
	i++;

	return inset;
}

InsetString ReadString(const char* str, size_t& i, size_t length, ErrorCode& error)
{
	if (str[i] == STRING_QUOTE)
		return ReadQuotedString(str, i, length, error);
	return ReadQuotelessString(str, i, length, error);
}

int ReadNumber(const char* str, size_t& i, size_t length, ErrorCode& error)
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
		error = ErrorCode::NUMBER_WITH_NO_NUMBERS;

		return 0;
	}

	//end of sign with no numbers
	if (!IsNumber(str[i]))
	{
		error = ErrorCode::NUMBER_WITH_NO_NUMBERS;

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



/////////////////////////////////////////////////////////////////////
//                      SEEK END OF VALUE                          //
// these are dumbed down versions of the readers for quick seeking //
/////////////////////////////////////////////////////////////////////



void SeekEndOfQuotelessString(const char* str, size_t& i, size_t length, ErrorCode& error)
{
	//first char must not be a symbol that's used by other types or whitespace
	char c = str[i];
	if (c == BLOCK_START || c == BLOCK_END || c == STRING_QUOTE || IsWhitespace(c))
	{
		error = ErrorCode::INVALID_QUOTELESS_STRING;

		return;
	}

	i++;

	for (c = str[i]; i < length; i++, c = str[i])
	{

		if (IsWhitespace(c) || c == BLOCK_START || c == BLOCK_END || c == STRING_QUOTE)
			break;
	}


	return;
}

void SeekEndOfQuotedString(const char* str, size_t& i, size_t length, ErrorCode& error)
{
	
	//first character should be a quote
	if (str[i] != STRING_QUOTE)
	{
		error = ErrorCode::STRING_NOT_BEGUN_WITH_QUOTE;

		return;
	}
	i++;

	size_t start = i;

	for (; i < length && str[i] != STRING_QUOTE; i++);

	// if we've hit the end of the string and havent gotten our quote, return null
	if (i >= length)
	{
		error = ErrorCode::INCOMPLETE_QUOTED_STRING;
		return;
	}

	// this should end on the character after the string.
	i++;

}

void SeekEndOfString(const char* str, size_t& i, size_t length, ErrorCode& error)
{
	if (str[i] == STRING_QUOTE)
		SeekEndOfQuotedString(str, i, length, error);
	else
		SeekEndOfQuotelessString(str, i, length, error);
}

void SeekEndOfNumber(const char* str, size_t& i, size_t length, ErrorCode& error)
{
	//overengineered? maybe... C++ lets you do +-+-1, so I felt this was appropriate
	if (str[i] == '+' || str[i] == '-')
	{
		i++;
		for (; i < length && (str[i] == '+' || str[i] == '-'); i++);
	}

	//end of string with no numbers
	if (i >= length)
	{
		error = ErrorCode::NUMBER_WITH_NO_NUMBERS;

		return;
	}

	//end of sign with no numbers
	if (!IsNumber(str[i]))
	{
		error = ErrorCode::NUMBER_WITH_NO_NUMBERS;

		return;
	}

	//little shortcut
	//since we know for a fact that str[i] is currently a number, we can skip double checking if it's actually a number
	i++;

	for (; i < length && IsNumber(str[i]); i++);

}
