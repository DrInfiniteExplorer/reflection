#include "SymbolOptions.h"

#include <stdio.h>
#include "dbghelp/dbghelp.h"




DWORD SymbolOptions::getOptions()
{
    return SymGetOptions();
}

void SymbolOptions::setOptions(DWORD flags)
{
    SymSetOptions(flags);
}

void SymbolOptions::removeOptions(DWORD flags)
{
    flags = ~flags;
    setOptions(getOptions() & flags);
}

void SymbolOptions::addOptions(DWORD flags)
{
    setOptions(getOptions() | flags);
}

std::string SymbolOptions::toString(DWORD flags)
{
    std::string ret;
    if (flags & SYMOPT_CASE_INSENSITIVE)                      ret.append("CASE_INSENSITIVE ");
    if (flags & SYMOPT_UNDNAME)                               ret.append("UNDNAME ");
    if (flags & SYMOPT_DEFERRED_LOADS)                        ret.append("DEFERRED_LOADS ");
    if (flags & SYMOPT_NO_CPP)                                ret.append("NO_CPP ");
    if (flags & SYMOPT_LOAD_LINES)                            ret.append("LOAD_LINES ");
    if (flags & SYMOPT_OMAP_FIND_NEAREST)                     ret.append("OMAP_FIND_NEAREST ");
    if (flags & SYMOPT_LOAD_ANYTHING)                         ret.append("LOAD_ANYTHING ");
    if (flags & SYMOPT_IGNORE_CVREC)                          ret.append("IGNORE_CVREC ");
    if (flags & SYMOPT_NO_UNQUALIFIED_LOADS)                  ret.append("NO_UNQUALIFIED_LOADS ");
    if (flags & SYMOPT_FAIL_CRITICAL_ERRORS)                  ret.append("FAIL_CRITICAL_ERRORS ");
    if (flags & SYMOPT_EXACT_SYMBOLS)                         ret.append("EXACT_SYMBOLS ");
    if (flags & SYMOPT_ALLOW_ABSOLUTE_SYMBOLS)                ret.append("ALLOW_ABSOLUTE_SYMBOLS ");
    if (flags & SYMOPT_IGNORE_NT_SYMPATH)                     ret.append("IGNORE_NT_SYMPATH ");
    if (flags & SYMOPT_INCLUDE_32BIT_MODULES)                 ret.append("INCLUDE_32BIT_MODULES ");
    if (flags & SYMOPT_PUBLICS_ONLY)                          ret.append("PUBLICS_ONLY ");
    if (flags & SYMOPT_NO_PUBLICS)                            ret.append("NO_PUBLICS ");
    if (flags & SYMOPT_AUTO_PUBLICS)                          ret.append("AUTO_PUBLICS ");
    if (flags & SYMOPT_NO_IMAGE_SEARCH)                       ret.append("NO_IMAGE_SEARCH ");
    if (flags & SYMOPT_SECURE)                                ret.append("SECURE ");
    if (flags & SYMOPT_NO_PROMPTS)                            ret.append("NO_PROMPTS ");
    if (flags & SYMOPT_OVERWRITE)                             ret.append("OVERWRITE ");
    if (flags & SYMOPT_IGNORE_IMAGEDIR)                       ret.append("IGNORE_IMAGEDIR ");
    if (flags & SYMOPT_FLAT_DIRECTORY)                        ret.append("FLAT_DIRECTORY ");
    if (flags & SYMOPT_FAVOR_COMPRESSED)                      ret.append("FAVOR_COMPRESSED ");
    if (flags & SYMOPT_ALLOW_ZERO_ADDRESS)                    ret.append("ALLOW_ZERO_ADDRESS ");
    if (flags & SYMOPT_DISABLE_SYMSRV_AUTODETECT)             ret.append("DISABLE_SYMSRV_AUTODETECT ");
    if (flags & SYMOPT_READONLY_CACHE)                        ret.append("READONLY_CACHE ");
    if (flags & SYMOPT_SYMPATH_LAST)                          ret.append("SYMPATH_LAST ");
    if (flags & SYMOPT_DISABLE_FAST_SYMBOLS)                  ret.append("DISABLE_FAST_SYMBOLS ");
    if (flags & SYMOPT_DISABLE_SYMSRV_TIMEOUT)                ret.append("DISABLE_SYMSRV_TIMEOUT ");
    if (flags & SYMOPT_DISABLE_SRVSTAR_ON_STARTUP)            ret.append("DISABLE_SRVSTAR_ON_STARTUP ");
    if (flags & SYMOPT_DEBUG)                                 ret.append("DEBUG ");

    return ret;
}

std::string SymbolOptions::toString()
{
    return toString(getOptions());
}
