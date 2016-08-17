#include "reflection.h"
#include "SymModule.h"
#include "itostr.h"

namespace reflection
{
    static char ThisModuleFilename[MAX_PATH];
    static ULONG64 ThisModuleBase;
    static SymModule* ThisModule = nullptr;
    static bool ReflectionInitialized = false;

    void init()
    {
        if (ReflectionInitialized)
        {
            return;
        }
        ThisModuleFilename[GetModuleFileName(0, ThisModuleFilename, sizeof(ThisModuleFilename))] = 0;

        auto loadBase = GetModuleHandle(nullptr);
        ThisModuleBase = reinterpret_cast<ULONG64>(loadBase);

		/*
        auto cb = []( PCSTR ModuleName, DWORD64 ModuleBase, ULONG ModuleSize, PVOID UserContext)
        {
            printf("!%s!\n!%s!\n%s\n%s\n\n", ThisModuleFilename, ModuleName, 
                itostr::hex(ThisModuleBase).c_str(), 
                itostr::hex(ModuleBase).c_str());
            return TRUE;
        };

        EnumerateLoadedModules64(GetCurrentProcess(), cb, 0);
		*/
        
        static SymModule ThisModule_realization = SymModule::load(ThisModuleFilename, true, ThisModuleBase);
        ThisModule = &ThisModule_realization;
        ReflectionInitialized = true;

        //printf("%s\n", ThisModule_realization.toString().c_str());

        //static SymModule ThisModule_realization = SymModule::findLoaded(ThisModuleFilename);
        //ThisModule = &ThisModule_realization;
        //ReflectionInitialized = true;
    }

    Symbol::SharedPtr getSymbolFromAddress(ULONG64 address)
    {
        return ThisModule->getSymbolFromAddress(address);
    }

    Type::TypeVector getTypes(const std::string& mask, bool ignoreCase)
    {
        return ThisModule->getTypes(mask, ignoreCase);
    }



}

