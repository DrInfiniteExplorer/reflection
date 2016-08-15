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

void func()
{
    printf("asd\n");
}

int g = 4;

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
            MessageBoxA(nullptr, "COULD NOT INITIALIZE SYMBOL STUFF", "ERROR", MB_ICONERROR);
            return 1;
        }
        class cleanup{ public: ~cleanup(){ SymCleanup(GetCurrentProcess()); } } _cleanup;

        reflection::init();
        auto aaa = &g;
        SymRefreshModuleList(GetCurrentProcess());

        auto funcSymbol = reflection::getSymbolFromAddress(reinterpret_cast<ULONG64>(&func));
        printf("Func %p %s\n", &func, funcSymbol->toString().c_str());

        try {
            auto gSymbol = reflection::getSymbolFromAddress(reinterpret_cast<ULONG64>(&g));
            printf("g %p %s\n", &g, gSymbol->toString().c_str());

            static int i = 3;
            auto iSymbol = reflection::getSymbolFromAddress(reinterpret_cast<ULONG64>(&i));
            printf("i %p %s\n", &i, iSymbol->toString().c_str());
        }
        catch (std::exception& e)
        {
            MessageBoxA(NULL, e.what(), "Error", 0);
        }

        std::string pdbFilePath;
        std::string mask;
        mask = "*";
        mask = "*ITestInt*";
        mask = "*TestClass*";

#ifdef _DEBUG
        pdbFilePath = "d:\\C++\\debugtools\\pdbInspector\\Debug\\pdbInspector.pdb";
        pdbFilePath = "d:\\Kod\\reflection\\pdbInspector\\Debug\\pdbInspector.exe";

		char asdasd[80];
		asdasd[GetModuleFileNameA(0, asdasd, 80)] = 0;
		pdbFilePath = asdasd;

        //pdbFilePath = "d:\\C++\\WorldTool\\AppRoot\\bin\\WorldTool.pdb";
#else
        pdbFilePath = "d:\\C++\\pdbInspector\\Release\\pdbInspector.pdb";
#endif

        DWORD imageSize = GetFileSize(pdbFilePath);

        auto loadBase = (ULONG64)GetModuleHandle(nullptr);
        auto symModule = SymModule::load(pdbFilePath, true, loadBase);

        printf("%s\n", symModule.toString().c_str());

        auto symbols = symModule.getSymbols(mask);

        printf(" === Symbols ===\n");
        int c = 0;
        int total = symbols.size();
        for (auto& symbol : symbols)
        {
            c++;
            printf("%s\n(%d/%d)\n", symbol->toString().c_str(), c, total);
        }

        printf(" ===  Types  ===\n");
        auto types = symModule.getTypes(mask);
        c = 0;
        total = types.size();
        for (auto& type : types)
        {
            c++;
            printf("%s\n(%d/%d)\n", type->getName().c_str(), c, total);
            printf("TypeIndex: %d\n", type->m_typeIndex);

            if (type->isUDT())
            {
                auto baseClasses = type->getBaseClasses();
                auto dataMembers = type->getDataMembers();
                auto methods = type->getMethodMembers();
                printf("     Base classes: %s\n", Type::printTypes(baseClasses).c_str());
                //printf("      dataMembers: %s\n", Type::printTypes(dataMembers).c_str());
                //printf("          methods: %s\n", Type::printTypes(methods).c_str());
            }
            printf("\n");

        }

        // If not used at all, will be optimized away when linking.
        // Need to force reference
        //auto dummy = new TestClass(3);
        //dummy->funcA();


		int var = 2;
        auto instance = instantiate < ITestInterface >::doit<int>(static_cast<int&&>(var++));
        instance->funcA();
        instance->funcB();
        instance = instantiate < ITestInterface >::doit<int>(std::forward<int>(var++));
        instance->funcA();
        instance->funcB();
        instance = instantiate < ITestInterface >::specify<int>::call(var++);
        instance->funcA();
        instance->funcB();
		instance = instantiate < ITestInterface >::doit<int>(9);
		instance->funcA();
		instance->funcB();
	}
    catch (const std::exception& e)
    {
        //MessageBoxA(NULL, e.what(), "Exception!", MB_ICONERROR);
		printf("Exception caught:\n%s\n", e.what());
    }

}
