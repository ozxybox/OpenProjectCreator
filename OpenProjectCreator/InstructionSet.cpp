#include "InstructionSet.h"

#include <cstring>

InstructionSet::InstructionSet(size_t count, instruction_t* set)
{
	m_instructionSet = set;
	m_instructionCount = count;
}

instruction_t* InstructionSet::GetInstruction(InsetString str)
{
	for (size_t i = 0; i < m_instructionCount; i++)
		if (m_instructionSet[i].nameLength == str.length && strncmp(m_instructionSet[i].name, str.string, str.length) == 0)
			return &m_instructionSet[i];
	return nullptr;
}

size_t InstructionSet::GetInstructionCount()
{
	return m_instructionCount;
}
