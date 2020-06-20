#pragma once

#include "Values.h"


// normal macros are read only. Nothing should have to modify these once they're created
class Macro
{
public:

	Macro(InsetString key, InsetString value);
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

protected:
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


class LinkedMacro : public Macro
{
public:
	LinkedMacro(InsetString key, InsetString value) : Macro(key, value) {}
private:

	void SetValue(InsetString value) { m_valueStr = value.string;  m_valueLength = value.length; }

	LinkedMacro* m_next;

	friend class LinkedMacroList;

};

// I'm really not a fan of this, but we shouldn't have too many macros I guess?
class LinkedMacroList
{
public:
	~LinkedMacroList();


	LinkedMacro* m_list;
	LinkedMacro* m_last;

	Macro* Get(InsetString key);

	// this function looks for a macro at the start of a string, and if it finds one, it returns it. Otherwise, it returns nullptr
	Macro* SearchFor(const char* str, size_t length);
	void Add(InsetString key, InsetString value);
	void Set(InsetString key, InsetString value);
};

class MacroStore
{
public:
	void SetMacro(const char* key, const char* value);
	const char* GetMacroValue(const char* key);

	// this function looks for a macro at the start of a string, and if it finds one, it returns it. Otherwise, it returns nullptr
	Macro* SearchForMacro(const char* str, size_t length);
private:

	LinkedMacroList m_macroList;
};

