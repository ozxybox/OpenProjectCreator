#pragma once

#include <cstddef>
#define END_LINE '\n'


class InsetString
{
public:
	InsetString() { string = 0; length = 0; }
	InsetString(const char* str, size_t len) { string = str; length = len; }


	char* Copy();


	const char* string;
	size_t length;
};

bool operator==(const InsetString& lhs, const InsetString& rhs);
bool operator!=(const InsetString& lhs, const InsetString& rhs);

enum class ErrorCode;

InsetString ReadQuotelessString(const char* str, size_t& i, size_t length, ErrorCode& error);
inline InsetString ReadQuotelessString(const char* str, size_t length, ErrorCode& error) { size_t i = 0; return ReadQuotelessString(str, i, length, error); }

InsetString ReadQuotedString(const char* str, size_t& i, size_t length, ErrorCode& error);
inline InsetString ReadQuotedString(const char* str, size_t length, ErrorCode& error) { size_t i = 0; return ReadQuotedString(str, i, length, error); }

InsetString ReadString(const char* str, size_t& i, size_t length, ErrorCode& error);
inline InsetString ReadString(const char* str, size_t length, ErrorCode& error) { size_t i = 0; return ReadString(str, i, length, error); }

int ReadNumber(const char* str, size_t& i, size_t length, ErrorCode& error);
inline int ReadNumber(const char* str, size_t length, ErrorCode& error) { size_t i = 0; return ReadNumber(str, i, length, error); }

void SeekEndOfQuotelessString(const char* str, size_t& i, size_t length, ErrorCode& error);
inline void SeekEndOfQuotelessString(const char* str, size_t length, ErrorCode& error) { size_t i = 0; return SeekEndOfQuotelessString(str, i, length, error); }

void SeekEndOfQuotedString(const char* str, size_t& i, size_t length, ErrorCode& error);
inline void SeekEndOfQuotedString(const char* str, size_t length, ErrorCode& error) { size_t i = 0; return SeekEndOfQuotedString(str, i, length, error); }

void SeekEndOfString(const char* str, size_t& i, size_t length, ErrorCode& error);
inline void SeekEndOfString(const char* str, size_t length, ErrorCode& error) { size_t i = 0; return SeekEndOfString(str, i, length, error); }

void SeekEndOfNumber(const char* str, size_t& i, size_t length, ErrorCode& error);
inline void SeekEndOfNumber(const char* str, size_t length, ErrorCode& error) { size_t i = 0; return SeekEndOfNumber(str, i, length, error); }

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