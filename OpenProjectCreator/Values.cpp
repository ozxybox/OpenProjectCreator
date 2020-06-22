#include "Values.h"
#include "BaseParser.h"

#include <cstring>


valueArray_t valueArray_t::Copy()
{ 
	valueArray_t valArray; 
	valArray.length = length; 
	valArray.array = new value_t[length]; 
	memcpy((void *)valArray.array, array, length * sizeof(value_t));
	
	return valArray;
}

value_t* subblockValue_t::Copy()
{
	return new subblockValue_t(subblock->Copy());
}
