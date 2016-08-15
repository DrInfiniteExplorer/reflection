#pragma once
#include <windows.h>
#include <string>
#include <memory>
#include <inttypes.h>
#include "itostr.h"
#include "SymType.h"
#include <time.h>
#include <vector>
#include "Symbol.h"
#include "Type.h"
#include <intsafe.h>

static inline int GetFileSize(const std::string& filePath)
{
    WIN32_FILE_ATTRIBUTE_DATA fileData;
    if (!GetFileAttributesExA(filePath.c_str(), GetFileExInfoStandard, &fileData))
    {
        throw std::logic_error("could not get attributes for " + filePath);
    }
    return fileData.nFileSizeLow;
}


class SymModule
{
    const std::string m_symModulePath;
    const DWORD64 m_baseOfImage;
    std::unique_ptr<IMAGEHLP_MODULE64> m_moduleInfo;
    SymModule(
        const std::string& symModulePath,
        DWORD64 baseOfImage
        );

public:
    static SymModule findLoaded(const std::string& symModulePath, bool loadIfNeeded = false);
    static SymModule load(const std::string& symModulePath, bool findIfLoaded = false, ULONG64 loadAt=ULONG64_MAX);

    std::string toString() const;
    Symbol::SymbolVector getSymbols(const std::string& symbolFilter) const;

    Symbol::SharedPtr getSymbolFromAddress(ULONG64 address) const;

    Type::TypeVector getTypes(const std::string& filter, bool ignoreCase = false) const;
};


