#pragma once

#include "Values.h"

class Macro
{
public:

	Macro();
	~Macro();

	const char* GetKey() { return m_keyStr; }
	size_t GetKeyLength() { return m_keyLength; }

	const char* GetValueStr() { return m_valueStr; }
	size_t GetValueLength() { return m_valueLength; }


	ValueType GetType() { return m_value.type; }

	//returns pointer to the int. Will be nullptr if the type is invalid!
	int* GetValueInt() { return (GetType() == ValueType::NUMBER) ? &m_number.number : nullptr; }

	//returns pointer to the value array. Will be nullptr if the type is invalid!
	valueArray_t* GetValueArray() { return (GetType() == ValueType::ARRAY) ? &m_array.array : nullptr; }

	//returns subblock. Will be nullptr if the type is invalid!
	BaseParser* GetValueSubblock() { return (GetType() == ValueType::SUBBLOCK) ? m_subblock.subblock : nullptr; }

	//returns a pointer to the bool. Will be nullptr if the type is invalid!
	bool* GetValueBoolean() { return (GetType() == ValueType::BOOLEAN) ? &m_boolean.boolean : nullptr; }

private:
	const char* m_keyStr;
	size_t m_keyLength;

	const char* m_valueStr;
	size_t m_valueLength;

	union
	{
		value_t m_value;

		numberValue_t m_number;
		booleanValue_t m_boolean;
		arrayValue_t m_array;
		subblockValue_t m_subblock;
	};
};


class MacroStore
{
public:
	void AddMacro(const char* key, const char* value);
	const char* GetMacroValue(const char* key);

	// this function looks for a macro at the start of a string, and if it finds one, it returns it. Otherwise, it returns nullptr
	Macro* SearchForMacro(const char* str, size_t length);
private:

};

