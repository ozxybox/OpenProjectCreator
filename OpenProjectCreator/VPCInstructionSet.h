#pragma once
#include "InstructionSet.h"

class VPCInstructionSet : public InstructionSet
{
private:
	// private constructors to prevent any more VPCInstructionSets from existing
	VPCInstructionSet() {}
	VPCInstructionSet(size_t count, instruction_t* set) : InstructionSet(count, set) {}

public:

	static VPCInstructionSet* Get();
};
