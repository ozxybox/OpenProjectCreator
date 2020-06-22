#include <cstring>

#include "MacroStore.h"
#include "Errors.h"

Macro::Macro(InsetString key, InsetString value)
{
	m_keyStr = key.Copy();
	m_keyLength = key.length;

	m_valueStr = value.Copy();
	m_valueLength = value.length;

	AnalyzeType();

}

Macro::Macro(const char* key, const char* value)
{
	
	m_keyLength = strlen(key);
	m_keyStr = new char[m_keyLength+1];
	memcpy(m_keyStr, key, m_keyLength);
	m_keyStr[m_keyLength] = 0;

	m_valueLength = strlen(value);
	m_valueStr = new char[m_valueLength+1];
	memcpy(m_valueStr, value, m_valueLength);
	m_valueStr[m_valueLength] = 0;

	AnalyzeType();
}

Macro::~Macro()
{
	delete[] m_keyStr;
	delete[] m_valueStr;
}

void Macro::AnalyzeType()
{

	// check the value and see if it might be a valid type

	bool valueIsNumber = true;
	for (size_t i = 0; i < m_valueLength; i++)
	{
		char c = m_valueStr[i];
		if (!IsNumber(c) && c != '-' && c != '+')
			valueIsNumber = false;

		// add some other type checking here
	}

	ErrorCode error = ErrorCode::NO_ERROR;
	if (valueIsNumber)
	{
		m_number.number = ReadNumber(m_valueStr, m_valueLength, error);
		if (error == ErrorCode::NO_ERROR)
		{
			m_value.type = ValueType::NUMBER;
			return;
		}

		// if we get here, we somehow managed to fail to parse a number we were sure was a number... oh well

	}


	// if all else fails, string is never wrong
	m_value.type = ValueType::STRING;

}

LinkedMacroList::LinkedMacroList()
{
	// add a root macro so we don't have to be constantly checking if list and last exist
	m_list = new LinkedMacro("", "");
	m_last = m_list;
}

LinkedMacroList::~LinkedMacroList()
{

	for (LinkedMacro* macro = m_list; macro;)
	{
		LinkedMacro* last = macro;
		macro = macro->m_next;
		delete last;
	}
}

Macro* LinkedMacroList::Get(InsetString key)
{
	// no key, no macro!
	if (key.length == 0)
		return nullptr;

	for (LinkedMacro* macro = m_list; macro; macro = macro->m_next)
	{
		if (macro->GetKeyLength() == key.length && strncmp(macro->GetKey(), key.string, key.length) == 0)
		{
			return macro;
		}
	}
	return nullptr;
}

Macro* LinkedMacroList::Get(const char* key)
{
	// no key, no macro!
	if (key == nullptr)
		return nullptr;

	size_t keyLength = strlen(key);

	for (LinkedMacro* macro = m_list; macro; macro = macro->m_next)
	{
		if (macro->GetKeyLength() == keyLength && strncmp(macro->GetKey(), key, macro->GetKeyLength()) == 0)
		{
			return macro;
		}
	}
	return nullptr;
}

Macro* LinkedMacroList::SearchFor(const char* str, size_t length)
{
	// if there's no string, there's no macro
	if (length == 0)
		return nullptr;

	//we cant just use LinkedMacroList::Get since we're searching for a partial string, not a full InsetString
	for (LinkedMacro* macro = m_list; macro; macro = macro->m_next)
	{
		if (macro->GetKeyLength() != 0 && macro->GetKeyLength() <= length && strncmp(str, macro->GetKey(), macro->GetKeyLength()) == 0)
		{
			return macro;
		}
	}
	return nullptr;
}

void LinkedMacroList::Add(InsetString key, InsetString value)
{
	LinkedMacro* newMacro = new	LinkedMacro(key, value);
	m_last->m_next = newMacro;
	m_last = newMacro;
}

void LinkedMacroList::Add(const char* key, const char* value)
{
	LinkedMacro* newMacro = new	LinkedMacro(key, value);
	m_last->m_next = newMacro;
	m_last = newMacro;
}

void LinkedMacroList::Set(InsetString key, InsetString value)
{
	// if we already have the macro, reassign it
	LinkedMacro* macro = static_cast<LinkedMacro*>(Get(key));
	if (macro)
	{
		macro->SetValue(value);
	}
	else
	{
		// otherwise, we have to add it
		Add(key, value);
	}
}

void LinkedMacroList::Set(const char* key, const char* value)
{
	// if we already have the macro, reassign it
	LinkedMacro* macro = static_cast<LinkedMacro*>(Get(key));
	if (macro)
	{
		macro->SetValue(value);
	}
	else
	{
		// otherwise, we have to add it
		Add(key, value);
	}
}



void MacroStore::SetMacro(const char* key, const char* value)
{
	m_macroList.Set(key, value);
}

const char* MacroStore::GetMacroValue(const char* key)
{
	return m_macroList.Get(key)->GetValueStr();
}

Macro* MacroStore::SearchForMacro(const char* str, size_t length)
{
	return m_macroList.SearchFor(str, length);
}

void LinkedMacro::SetValue(const char* value)
{
	m_valueLength = strlen(value);
	m_valueStr = new char[m_valueLength + 1];
	memcpy(m_valueStr, value, m_valueLength);
	m_valueStr[m_valueLength] = 0;
	
}
