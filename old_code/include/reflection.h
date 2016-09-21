#pragma once

#include <windows.h>
#include <string>
#include "Symbol.h"
#include "Type.h"

namespace reflection
{
    void init();

    Symbol getSymbolFromAddress(ULONG64 address);
    
    Type::TypeVector getTypes(const std::string& mask, bool ignoreCase = false);


}

