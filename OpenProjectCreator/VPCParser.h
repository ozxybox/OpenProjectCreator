#pragma once

#include <cstddef>
#include <vector>

#include "BaseParser.h"
#include "MacroStore.h"
#include "VPCInstructionSet.h"

#include "Values.h"
#include "Errors.h"

class VPCParser : public BaseParser
{
public:
	VPCParser(const char* str, size_t length);
private:
	VPCParser() {}

public:

	virtual BaseParser* Copy();

private:


	virtual void Execute();
	virtual instructionData_t* ParseInstruction(const char* str, size_t& i, size_t length, ErrorCode& error);
	virtual void ReadSubblock(const char* str, size_t& i, size_t length, ErrorCode& error);
	virtual void SeekEndOfSubblock(const char* str, size_t& i, size_t length, ErrorCode& error);

	//virtual instruction_t* GetInstruction(const char* str, size_t length);
	virtual void SkipWhitespace(const char* str, size_t& i, size_t length);

	virtual bool ParseCondition(const char* str, size_t& i, size_t length, ErrorCode& error);

	virtual const instruction_t* GetInstruction(InsetString str) { return m_instructionSet->GetInstruction(str); };
	virtual void CacheInstructionData(instructionData_t* instructionData) { m_instructionDataList.push_back(instructionData); }

	MacroStore m_macroStore;

	std::vector<instructionData_t*> m_instructionDataList;
	InstructionSet* m_instructionSet;


};

