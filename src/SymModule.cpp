#include "SymModule.h"
#include "SymFlags.h"
#include "Type.h"

bool endsWith(const char *src, const char  *find)
{
    auto findLen = strlen(find);
    auto srcLen = strlen(src);
    if (findLen > srcLen) return false;
    auto cmpPtr = src + srcLen - findLen;
    return _stricmp(cmpPtr, find) == 0;
}

SymModule SymModule::load(const std::string& symModulePath, bool findIfLoaded /*= false*/, ULONG64 loadAt/*=ULONG64_MAX*/)
{

    auto isPdb = endsWith(symModulePath.c_str(), "PDB");

    auto fileSize = isPdb
        ? GetFileSize(symModulePath)
        : 0;

    static auto loadAtBase = 0x20000000;
    loadAt = loadAt == ULONG64_MAX ? loadAtBase : loadAt;
    auto baseOfImage = SymLoadModuleEx(
        GetCurrentProcess(),
        NULL,
        symModulePath.c_str(),
        NULL,
        loadAt,
        fileSize,
        NULL,
        0);

    if (baseOfImage == 0)
    {
        if (GetLastError() == ERROR_SUCCESS && findIfLoaded)
        {
            return findLoaded(symModulePath, false);
        }
        throw std::runtime_error("Could not load " + symModulePath);
    }

    loadAtBase += loadAt == ULONG64_MAX ? fileSize + 10000000 : 0;

    return SymModule(symModulePath, baseOfImage);
}

SymModule SymModule::findLoaded(const std::string& symModulePath, bool loadIfNeeded /*= false*/)
{
    DWORD64 baseOfImage = 0;

    static const char* findPtr;
    static DWORD64* pBaseOfImage;
    findPtr = symModulePath.c_str();
    pBaseOfImage = &baseOfImage;

    auto cb = [](PCSTR  ModuleName, DWORD64 baseOfImage, PVOID userdata) {
        IMAGEHLP_MODULE64 moduleInfo;
        moduleInfo.SizeOfStruct = sizeof(moduleInfo);
        if (!SymGetModuleInfo64(GetCurrentProcess(), baseOfImage, &moduleInfo))
        {
            printf("error calling SymGetModuleInfo64 from SymModule::findLoaded::cb for %s\n", ModuleName);
            return TRUE;
        }
        //TODO: Check for more better way to determine if different paths refer to the same resource
        if (!_stricmp(moduleInfo.LoadedPdbName, findPtr))
        {
            *pBaseOfImage = baseOfImage;
            return FALSE;
        }
        if (!_stricmp(moduleInfo.LoadedImageName, findPtr))
        {
            *pBaseOfImage = baseOfImage;
            return FALSE;
        }
        return TRUE;
    };

    if (!SymEnumerateModules64(GetCurrentProcess(), cb, nullptr))
    {
        throw std::runtime_error("SymEnumerateModules64 failed in SymModule::findLoaded for " + symModulePath);
    }
    if (baseOfImage == 0 && loadIfNeeded)
    {
        return load(symModulePath, false);
    }
    if (baseOfImage == 0)
    {
        throw std::runtime_error("Could not yo");
    }


    return SymModule(symModulePath, baseOfImage);
}

SymModule::SymModule(const std::string& symModulePath, DWORD64 baseOfImage) : m_symModulePath(symModulePath)
, m_baseOfImage(baseOfImage)
{
    IMAGEHLP_MODULE64 moduleInfo;
    moduleInfo.SizeOfStruct = sizeof(moduleInfo);
    if (!SymGetModuleInfo64(GetCurrentProcess(), baseOfImage, &moduleInfo))
    {
        throw std::runtime_error("error calling SymGetModuleInfo64 from SymModule::SymModule for " + symModulePath);
    }
    m_moduleInfo = std::make_unique<IMAGEHLP_MODULE64>();

    *m_moduleInfo = moduleInfo;
}

std::string SymModule::toString() const
{
    std::string ret;
    strprintf(ret, "Loaded as: %s\n", m_symModulePath.c_str());
    strprintf(ret, "m_baseOfImage: %s\n", m_symModulePath.c_str());
    strprintf(ret, "m_moduleInfo<IMAGEHLP_MODULE64>\n");
    if (m_moduleInfo.get() == nullptr)
    {
        strprintf(ret, " >this == nullptr\n");
    }
    else
    {
        strprintf(ret, " >BaseOfImage:     %s\n", itostr::hex(m_moduleInfo->BaseOfImage).c_str());
        strprintf(ret, " >ImageSize:       %s\n", itostr::hex(m_moduleInfo->ImageSize).c_str());
        strprintf(ret, " >(image end):     %s\n", itostr::hex(m_moduleInfo->BaseOfImage + m_moduleInfo->ImageSize).c_str());
        strprintf(ret, " >TimeDateStamp:   %s(%s)\n", itostr::hex(m_moduleInfo->TimeDateStamp).c_str(), itostr::str(time_t(m_moduleInfo->TimeDateStamp)).c_str());
        strprintf(ret, " >CheckSum:        %s\n", itostr::hex(m_moduleInfo->CheckSum).c_str());
        strprintf(ret, " >NumSyms:         %s%s\n", itostr::hex(m_moduleInfo->NumSyms).c_str(), m_moduleInfo->SymType == SymPdb ? "(with SymType==SymPdb it has no meaning)" : "");
        strprintf(ret, " >SymType:         %s\n", SymType(m_moduleInfo->SymType).toString().c_str());
        strprintf(ret, " >ModuleName:      %s\n", itostr::str(m_moduleInfo->ModuleName).c_str());
        strprintf(ret, " >ImageName:       %s\n", itostr::str(m_moduleInfo->ImageName).c_str());
        strprintf(ret, " >LoadedImageName: %s\n", itostr::str(m_moduleInfo->LoadedImageName).c_str());
        strprintf(ret, " >LoadedPdbName:   %s\n", itostr::str(m_moduleInfo->LoadedPdbName).c_str());
        strprintf(ret, " >CVSig:           %s\n", itostr::hex(m_moduleInfo->CVSig).c_str());
        strprintf(ret, " >CVData:          %s\n", itostr::str(m_moduleInfo->CVData).c_str());
        strprintf(ret, " >PdbSig:          %s\n", itostr::hex(m_moduleInfo->PdbSig).c_str());
        strprintf(ret, " >PdbSig70:        %s\n", itostr::hex(m_moduleInfo->PdbSig70.Data1).c_str());
        strprintf(ret, " >PdbAge:          %s\n", itostr::hex(m_moduleInfo->PdbAge).c_str());
        strprintf(ret, " >PdbUnmatched:    %s\n", itostr::hex(m_moduleInfo->PdbUnmatched).c_str());
        strprintf(ret, " >DbgUnmatched:    %s\n", itostr::hex(m_moduleInfo->DbgUnmatched).c_str());
        strprintf(ret, " >LineNumbers:     %s(Line number info available:          %s)\n", itostr::hex(m_moduleInfo->LineNumbers).c_str(), m_moduleInfo->LineNumbers ? "YES" : " - ");
        strprintf(ret, " >GlobalSymbols:   %s(Global symbol information available: %s)\n", itostr::hex(m_moduleInfo->GlobalSymbols).c_str(), m_moduleInfo->GlobalSymbols ? "YES" : " - ");
        strprintf(ret, " >TypeInfo:        %s(Type information available:          %s)\n", itostr::hex(m_moduleInfo->TypeInfo).c_str(), m_moduleInfo->TypeInfo ? "YES" : " - ");
        strprintf(ret, " >SourceIndexed:   %s(pdb source server supported:         %s)\n", itostr::hex(m_moduleInfo->SourceIndexed).c_str(), m_moduleInfo->SourceIndexed ? "YES" : " - ");
        strprintf(ret, " >Publics:         %s(Public symbol information available: %s)\n", itostr::hex(m_moduleInfo->Publics).c_str(), m_moduleInfo->Publics ? "YES" : " - ");
        strprintf(ret, " >MachineType:     %s\n", itostr::hex(m_moduleInfo->MachineType).c_str());
        strprintf(ret, " >Reserved:        %s\n", itostr::hex(m_moduleInfo->Reserved).c_str());
    }
    return ret;
}

Symbol::SymbolVector SymModule::getSymbols(const std::string& symbolFilter) const
{
    Symbol::SymbolVector ret;
    static Symbol::SymbolVector* svPtr;
    svPtr = &ret;

    auto callback = [](PSYMBOL_INFO pInfo, ULONG symbolSize, PVOID userContext) {
        auto symbol = std::make_shared<Symbol>(pInfo, symbolSize);
        svPtr->push_back(symbol);
        return TRUE;
    };
    if (!SymEnumSymbols(GetCurrentProcess(), m_baseOfImage, symbolFilter.c_str(), callback, nullptr))
    {
        throw std::runtime_error("Error calling SymEnumSymbols in SymModule::getSymbols");
    }

    return ret;
}

Symbol::SharedPtr SymModule::getSymbolFromAddress(ULONG64 address) const
{

    size_t bufferSize = sizeof(SYMBOL_INFO) + sizeof(TCHAR) * MAX_SYM_NAME;
    static SYMBOL_INFO* ptr = nullptr;
    ptr = reinterpret_cast<SYMBOL_INFO*>(malloc(bufferSize));
    struct release{ ~release(){ free(ptr); } } release_dummy;

    memset(ptr, 0, bufferSize);

    ptr->SizeOfStruct = sizeof(SYMBOL_INFO);
    ptr->MaxNameLen = MAX_SYM_NAME;
    ULONG64 displacement = 0;
    if (!SymFromAddr(GetCurrentProcess(), address, &displacement, ptr))
    {
        throw std::runtime_error("Could not SymFromAddr from " __FUNCTION__);
    }
    return std::make_shared<Symbol>(ptr, ptr->Size);
}

Type::TypeVector SymModule::getTypes(const std::string& filter, bool ignoreCase) const
{
    auto ret = Type::TypeVector();
    static Type::TypeVector* retPtr;
    retPtr = &ret;
    static const char* filterPtr;
    filterPtr = filter.c_str();
    static BOOL compareCase;
    compareCase = !ignoreCase;

    auto callback = [](PSYMBOL_INFO pSymInfo, ULONG SymbolSize, PVOID UserContext)
    {
        if (!SymMatchString(pSymInfo->Name, filterPtr, compareCase)) return TRUE;
        auto type = Type(pSymInfo->ModBase, pSymInfo->TypeIndex);
        retPtr->push_back(type);
        return TRUE;
    };

    if (!SymEnumTypes(GetCurrentProcess(), m_baseOfImage, callback, nullptr))
    {
        throw std::runtime_error("Error calling SymEnumTypes in SymModule::getTypes");
    }

    return ret;
}

