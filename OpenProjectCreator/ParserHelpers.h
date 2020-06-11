#pragma once

#define END_LINE '\n'


struct insetString_t
{
	const char* string;
	size_t length;
};

enum class ErrorCode;

insetString_t ReadQuotelessString(const char* str, size_t& i, size_t length, ErrorCode* error = 0);
insetString_t ReadQuotedString(const char* str, size_t& i, size_t length, ErrorCode* error = 0);
insetString_t ReadString(const char* str, size_t& i, size_t length, ErrorCode* error = 0);
int ReadNumber(const char* str, size_t& i, size_t length, ErrorCode* error = 0);

void SeekEndOfQuotelessString(const char* str, size_t& i, size_t length, ErrorCode* error = 0);
void SeekEndOfQuotedString(const char* str, size_t& i, size_t length, ErrorCode* error = 0);
void SeekEndOfString(const char* str, size_t& i, size_t length, ErrorCode* error = 0);
void SeekEndOfNumber(const char* str, size_t& i, size_t length, ErrorCode* error = 0);

inline bool IsWhitespace(char c)
{
	return c <= ' ' || c == 127;
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