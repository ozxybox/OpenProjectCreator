#include "VPCInstructionSet.h"
#include "Values.h"

#include <cstdio>

//this is so sad

void VPC_Macro(instructionData_t* data)
{
	// not a fan of the copying, but stringValue uses inset strings so we have to if we don't want to print the entire file
	const char* name = static_cast<stringValue_t*>(data->arguments[0])->string.Copy();
	const char* value = static_cast<stringValue_t*>(data->arguments[1])->string.Copy();
	printf("$Macro %s %s\n", name, value);
	delete[] name;
	delete[] value;
}

void VPC_Configuration(instructionData_t* data)
{

}

void VPC_Folder(instructionData_t* data)
{

}

void VPC_Folder_Add(instructionData_t* data)
{

}



// hash table would be quicker
static const instruction_t g_vpcInstructions[] =
{
	//Name, function, is preprocessor, argument types(MAX OF 4)
	DEFINE_INSTRUCTION("Macro", VPC_Macro, true, ArgumentType::QUOTELESS_STRING, ArgumentType::QUOTED_STRING)
	DEFINE_INSTRUCTION("Configuration", VPC_Configuration, false, ArgumentType::QUOTED_STRING, ArgumentType::SUBBLOCK)

	BEGIN_CUSTOM_INSTRUCTION("Folder", VPC_Folder, false, 1, ArgumentType::STRING, ArgumentType::SUBBLOCK)
		DEFINE_INSTRUCTION("Add", VPC_Folder_Add, false, ArgumentType::STRING)
	END_CUSTOM_INSTRUCTION()
};

static size_t g_vpcInstructionCount = sizeof(g_vpcInstructions) / sizeof(instruction_t);



// I'd rather pass this by reference...
VPCInstructionSet* VPCInstructionSet::Get()
{
	static VPCInstructionSet s_vpcInstructionSet(g_vpcInstructionCount, const_cast<instruction_t*>(g_vpcInstructions));

	return &s_vpcInstructionSet;
}