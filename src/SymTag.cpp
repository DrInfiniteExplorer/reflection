#include "cvconst.h"
#include "SymTag.h"
#include "dbghelp/dbghelp.h"


SymTag::SymTag(const ULONG& tag) : m_tag(tag)
{

}

std::string SymTag::toString() const
{
    if (m_tag == SymTagNull)
        return "SymTagNull";
    if (m_tag == SymTagExe)
        return "SymTagExe";
    if (m_tag == SymTagCompiland)
        return "SymTagCompiland";
    if (m_tag == SymTagCompilandDetails)
        return "SymTagCompilandDetails";
    if (m_tag == SymTagCompilandEnv)
        return "SymTagCompilandEnv";
    if (m_tag == SymTagFunction)
        return "SymTagFunction";
    if (m_tag == SymTagBlock)
        return "SymTagBlock";
    if (m_tag == SymTagData)
        return "SymTagData";
    if (m_tag == SymTagAnnotation)
        return "SymTagAnnotation";
    if (m_tag == SymTagLabel)
        return "SymTagLabel";
    if (m_tag == SymTagPublicSymbol)
        return "SymTagPublicSymbol";
    if (m_tag == SymTagUDT)
        return "SymTagUDT";
    if (m_tag == SymTagEnum)
        return "SymTagEnum";
    if (m_tag == SymTagFunctionType)
        return "SymTagFunctionType";
    if (m_tag == SymTagPointerType)
        return "SymTagPointerType";
    if (m_tag == SymTagArrayType)
        return "SymTagArrayType";
    if (m_tag == SymTagBaseType)
        return "SymTagBaseType";
    if (m_tag == SymTagTypedef)
        return "SymTagTypedef";
    if (m_tag == SymTagBaseClass)
        return "SymTagBaseClass";
    if (m_tag == SymTagFriend)
        return "SymTagFriend";
    if (m_tag == SymTagFunctionArgType)
        return "SymTagFunctionArgType";
    if (m_tag == SymTagFuncDebugStart)
        return "SymTagFuncDebugStart";
    if (m_tag == SymTagFuncDebugEnd)
        return "SymTagFuncDebugEnd";
    if (m_tag == SymTagUsingNamespace)
        return "SymTagUsingNamespace";
    if (m_tag == SymTagVTableShape)
        return "SymTagVTableShape";
    if (m_tag == SymTagVTable)
        return "SymTagVTable";
    if (m_tag == SymTagCustom)
        return "SymTagCustom";
    if (m_tag == SymTagThunk)
        return "SymTagThunk";
    if (m_tag == SymTagCustomType)
        return "SymTagCustomType";
    if (m_tag == SymTagManagedType)
        return "SymTagManagedType";
    if (m_tag == SymTagDimension)
        return "SymTagDimension";
    if (m_tag == SymTagCallSite)
        return "SymTagCallSite";
    if (m_tag == SymTagMax)
        return "SymTagMax";

    throw std::logic_error("m_tag has unknown value");
}
