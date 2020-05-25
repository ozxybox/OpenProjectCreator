#pragma once
class MacroStore
{
public:
	void AddMacro(const char* key, const char* value);
	const char* GetMacroValue(const char* key);
private:

};

