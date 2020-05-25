#pragma once

#define END_LINE '\n'

inline bool IsWhitespace(char c)
{
	return c <= ' ' || c == 127;
}

inline bool IsNumber(char c)
{
	return c >= '0' && c <= '9';
}

inline bool IsNumber(char c)
{
	return c >= '0' && c <= '9';
}

inline bool IsLowercase(char c)
{
	return c >= 'a' && c <= 'z';
}

inline bool IsUppercase(char c)
{
	return c >= 'A' && c <= 'Z';
}

inline bool IsSymbol(char c)
{
	return (c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~');
}

inline bool IsAlphanumerical(char c)
{
	return IsNumber(c) || IsUppercase(c) || IsLowercase(c);
}