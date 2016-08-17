#include <windows.h>
#include <string>
#include <inttypes.h>
#include <memory>
#include "SymModule.h"
#include "SymbolOptions.h"
#include <numeric>
#include <algorithm>
#include "instantiator.h"
#include "reflection.h"


#pragma comment(lib, "dbghelp.lib")

#include "interface.h"

int* functionToTest(char arg1, int arg2, uint16_t arg3, void* arg4, wchar_t& arg5)
{
    printf("functionToTest called with arguments %c %d %d %p %d\n", arg1, arg2, arg3, arg4, arg5);
    return (int*)42;
}


int main(int argc, const char *const  argv[])
{
    try {
        SymbolOptions::addOptions(SYMOPT_DEBUG);
        SymbolOptions::addOptions(SYMOPT_LOAD_LINES);
        SymbolOptions::addOptions(SYMOPT_ALLOW_ZERO_ADDRESS);
        SymbolOptions::addOptions(SYMOPT_INCLUDE_32BIT_MODULES);

        SymbolOptions::removeOptions(SYMOPT_UNDNAME);
        SymbolOptions::removeOptions(SYMOPT_NO_CPP);
        SymbolOptions::removeOptions(SYMOPT_DEFERRED_LOADS);

        BOOL bInvade = FALSE;
        auto status = SymInitialize(GetCurrentProcess(), nullptr, bInvade);
        if (status == FALSE)
        {
            throw std::runtime_error("could not SymInitialize");
        }
        class cleanup { public: ~cleanup() { SymCleanup(GetCurrentProcess()); } } _cleanup;

        reflection::init();
        SymRefreshModuleList(GetCurrentProcess());

        typedef decltype(&functionToTest) FunctionToTestPtrType;
        auto funcSymbol = reflection::getSymbolFromAddress(reinterpret_cast<ULONG64>(&functionToTest));
        auto&& type = funcSymbol.getType();
        printf("%s\n%s\n"
            , funcSymbol.toString().c_str()
            , type.toString().c_str());

        auto asFunction = funcSymbol.getFunction();
        asFunction.callFunction<void>();
    }
    catch (const std::exception& e)
    {
        printf("Exception caught:\n%s\n", e.what());
        return 1;
    }
    return 0;

}
