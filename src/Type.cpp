#include "cvconst.h"
#include "type.h"
#include <algorithm>
#include "itostr.h"
#include <numeric>
#include "SymTag.h"
#include "function.h"


Type::Type(ULONG64 imageBase, ULONG typeIndex) : m_imageBase(imageBase), m_typeIndex(typeIndex), m_typeName("<no name>")
{
	WCHAR* namePtr = nullptr;
	std::string a;
	if (GetProperty(TI_GET_SYMNAME, namePtr))
	{
		m_typeName = strprintf("%S", namePtr);
		LocalFree(namePtr);
	}
	// TODO: Check that our tag is that of a type and not some crap.
	// For some info on why name fails at times, see
	// http://www.tech-archive.net/Archive/VisualStudio/microsoft.public.vsnet.debugging/2004-03/0138.html
}

Type Type::getType() const
{
	DWORD type;
	if (!GetProperty(TI_GET_TYPE, type))
	{
		throw std::runtime_error("Canna " __FUNCTION__);
	}
	return Type(m_imageBase, type);
}

DWORD Type::getBaseType() const
{
	DWORD baseType;

	if (!GetProperty(TI_GET_BASETYPE, baseType))
	{
		throw std::runtime_error("Canna " __FUNCTION__);
	}
	return baseType;
}

std::string Type::getBaseTypeString(DWORD baseType, ULONG64 size) const
{
	switch (baseType)
	{

	case btVoid:
		return "void";
	case btChar:
		return "char";
	case btWChar:
		return "wchar_t";
	case btInt:
		return "int";
	case btUInt:
		return "unsigned int";
	case btFloat:
		return "float";
	case btBool:
		return "bool";
	case btLong:
		return "long";
	case btULong:
		return "unsigned long";
	case btCurrency:
	case btDate:
	case btVariant:
	case btComplex:
	case btBit:
	case btBSTR:
	case btHresult:
	case btBCD:
	case btChar16:
	case btChar32:
	case btNoType:
	default:
		throw std::runtime_error("BAD BASETYPE CANT MAKE TO STRING");
	}
}

bool Type::operator==(const Type& other) const
{
	//printf(" comparing %s(%d) with %s(%d)\n", m_typeName.c_str(), m_typeIndex, other.m_typeName.c_str(), other.m_typeIndex);
	//printf("%s\n%s\n", toString().c_str(), other.toString().c_str());


	if (m_typeIndex == other.m_typeIndex)
	{
		return true;
	}

	auto thisTag = getTag();
	auto thatTag = other.getTag();
	if (thisTag != thatTag) return false;

	switch (thisTag)
	{
	case SymTagFunctionArgType:
	case SymTagPointerType:
	{
		auto&& a = getType();
		auto&& b = other.getType();
		return a == b;
	}

	case SymTagFunctionType:
	{
		auto&& returnValueA = getReturnType();
		auto&& returnValueB = other.getReturnType();

		if (returnValueA != returnValueB)
		{
			//throw std::runtime_error("Return type is different for things");
			return false;
		}

		auto parametersA = getParameters();
		auto parametersB = other.getParameters();
		if (parametersA.size() != parametersB.size())
		{
			//throw std::runtime_error("Number of parameters are not the same");
			return false;
		}

		for (size_t idx = 0; idx < parametersA.size(); idx++)
		{
			auto&& a = parametersA[idx];
			auto&& b = parametersB[idx];
			if (a != b)
			{
				auto isType = parametersA[idx].toString();
				auto wantType = parametersB[idx].toString();
				char buffer[256];
				sprintf_s(buffer, "Parameter %d differs for function; Wanted %s, is actually %s", idx, wantType.c_str(), isType.c_str());
				//throw std::runtime_error(buffer);
				return false;
			}
		}
		return true;
	}
	case SymTagNull:
	case SymTagExe:
	case SymTagCompiland:
	case SymTagCompilandDetails:
	case SymTagCompilandEnv:
	case SymTagFunction:
	case SymTagBlock:
	case SymTagData:
	case SymTagAnnotation:
	case SymTagLabel:
	case SymTagPublicSymbol:
	case SymTagUDT:
	case SymTagEnum:
	case SymTagArrayType:
	case SymTagBaseType:
	case SymTagTypedef:
	case SymTagBaseClass:
	case SymTagFriend:
	case SymTagFuncDebugStart:
	case SymTagFuncDebugEnd:
	case SymTagUsingNamespace:
	case SymTagVTableShape:
	case SymTagVTable:
	case SymTagCustom:
	case SymTagThunk:
	case SymTagCustomType:
	case SymTagManagedType:
	case SymTagDimension:
	case SymTagCallSite:
		return false;// I guess dunno
	}

	return false;

}
bool Type::operator!=(const Type& other) const
{
	return !(*this == other);
}


ULONG64 Type::getSize() const
{
	ULONG64 size = 0;
	if (!GetProperty(TI_GET_LENGTH, size))
	{
		throw std::runtime_error("Canna " __FUNCTION__);
	}
	return size;
}

DWORD Type::getCount() const
{
	DWORD count = 0;
	if (!GetProperty(TI_GET_COUNT, count))
	{
		throw std::runtime_error("Canna " __FUNCTION__);
	}
	return count;
}


ULONG Type::getTag() const
{
	DWORD tag = 0;
	if (!GetProperty(TI_GET_SYMTAG, tag))
	{
		throw std::runtime_error("Could not " __FUNCTION__);
	}
	return tag;
}

DWORD Type::getCallConvention() const
{
	DWORD cc;
	if (!GetProperty(TI_GET_SYMTAG, cc))
	{
		throw std::runtime_error("Could not " __FUNCTION__);
	}
	return cc;
}

template <typename T>
BOOL Type::GetProperty(IMAGEHLP_SYMBOL_TYPE_INFO info, T& t, ULONG* typeIndexPtr) const
{
	//TI_GET_SYMTAG
	auto typeIndex = typeIndexPtr ? *typeIndexPtr : m_typeIndex;
	if (!SymGetTypeInfo(GetCurrentProcess(), m_imageBase, typeIndex, info, &t))
	{
		if (GetLastError() == 1) return FALSE;
	}
	return TRUE;
}

std::vector<ULONG> Type::getChildren() const
{
	std::vector<ULONG> ret;

	DWORD childCount = 0;
	if (!GetProperty(TI_GET_CHILDRENCOUNT, childCount))
	{
		return ret;
	}
	if (childCount == 0) return ret;

	// TI_FINDCHILDREN_PARAMS ends with an unbound array of size 1.
	// Need to allocate enough crap to house things.
	size_t bufferSize = sizeof(TI_FINDCHILDREN_PARAMS) + (childCount - 1) * sizeof(((TI_FINDCHILDREN_PARAMS*)nullptr)->ChildId[0]);

	static TI_FINDCHILDREN_PARAMS* ptr = nullptr;
	ptr = reinterpret_cast<TI_FINDCHILDREN_PARAMS*>(malloc(bufferSize));
	struct release { ~release() { free(ptr); } } release_dummy;
	memset(ptr, 0, bufferSize);
	ptr->Count = childCount;
	ptr->Start = 0;

	if (!GetProperty(TI_FINDCHILDREN, *ptr))
	{
		throw std::runtime_error("Could not TI_FINDCHILDREN");
	}

	for (size_t idx = 0; idx < childCount; idx++)
	{
		const auto& typeIndex = ptr->ChildId[idx];
		ret.push_back(typeIndex);
	}

	return ret;
}

std::vector<ULONG> Type::getChildrenWithTag(ULONG tagType) const
{
	auto children = getChildren();
	auto filter = [tagType, this](ULONG childIndex)
	{
		DWORD tag;
		if (!GetProperty(TI_GET_SYMTAG, tag, &childIndex))
		{
			throw std::runtime_error("Could not " __FUNCTION__);
		}
		return tag != tagType;
	};
	auto end = std::remove_if(children.begin(), children.end(), filter);
	children.resize(std::distance(children.begin(), end));
	return children;
}

Type::TypeVector Type::getBaseClasses() const
{
	if (!isUDT())
	{
		throw std::runtime_error("Cant get base classes for things that are not UDT lol");
	}

	auto children = getChildrenWithTag(SymTagBaseClass);

	TypeVector ret;
	ret.resize(children.size());
	auto makeType = [this](ULONG type)
	{
		return Type(m_imageBase, type);
	};
	std::transform(children.begin(), children.end(), ret.begin(), makeType);
	return ret;
}

Function::FunctionVector Type::getMethodMembers() const
{
	if (!isUDT())
	{
		throw std::runtime_error("Cant " __FUNCTION__ " for things that are not UDT lol");
	}
	auto children = getChildrenWithTag(SymTagFunction);

	Function::FunctionVector ret;
	ret.resize(children.size());
	auto makeType = [this](ULONG type)
	{
		return Function(m_imageBase, type);
	};
	std::transform(children.begin(), children.end(), ret.begin(), makeType);
	return ret;
}


Function::FunctionVector Type::getConstructors(bool ignoreZeroAddress /*= true*/) const
{
	if (!isUDT())
	{
		throw std::runtime_error("Cant " __FUNCTION__ " for things that are not UDT lol");
	}

	auto children = getChildrenWithTag(SymTagFunction);

	Function::FunctionVector ret;
	ret.resize(children.size());
	auto makeType = [this](ULONG type)
	{
		return Function(m_imageBase, type);
	};
	std::transform(children.begin(), children.end(), ret.begin(), makeType);

	auto filterFunctions = [this, ignoreZeroAddress](const Function& function)
	{
		return function.getNameOriginal() != this->getName() || (ignoreZeroAddress && function.getAddress() == 0);
	};

	auto newEnd = std::remove_if(ret.begin(), ret.end(), filterFunctions);
	ret.resize(distance(ret.begin(), newEnd));
	return ret;
}

Symbol::SymbolVector Type::getDataMembers() const
{
	if (!isUDT())
	{
		throw std::runtime_error("Cant " __FUNCTION__ " for things that are not UDT lol");
	}

	auto children = getChildrenWithTag(SymTagData);

	Symbol::SymbolVector ret;
	ret.resize(children.size());
	auto makeType = [this](ULONG type)
	{
		return Symbol(m_imageBase, type);
	};
	std::transform(children.begin(), children.end(), ret.begin(), makeType);
	return ret;
}

Type::TypeVector Type::getParameters() const
{
	auto parameterCount = getCount();
	auto children = getChildrenWithTag(SymTagFunctionArgType);

	Type::TypeVector ret;
	ret.resize(children.size());
	auto makeType = [this](ULONG type)
	{
		return Type(m_imageBase, type);
	};
	std::transform(children.begin(), children.end(), ret.begin(), makeType);
	return ret;
}

Type Type::getReturnType() const
{
	if (!isFunctionType())
	{
		throw std::runtime_error("Is not function type!");
	}
	return getType();
}

std::string Type::printTypes(const TypeVector& sv)
{
	return std::accumulate(sv.begin(), sv.end(), std::string(), [](std::string sum, const Type& b) {
		return strprintf("%s\n%s(%s) %d %s",
			sum.c_str(),
			b.getName().c_str(),
			SymTag(b.getTag()).toString().c_str(),
			b.m_typeIndex,
			0/*itostr::hex(b->getAddress()).c_str()*/);
	});
}

bool Type::isVirtualBaseClass() const
{
	if (!isBaseClass()) return false;
	BOOL isVirtual = FALSE;
	if (!GetProperty(TI_GET_VIRTUALBASECLASS, isVirtual)) return false;
	return isVirtual != FALSE;
}

bool Type::isBaseClass() const
{
	return getTag() == SymTagBaseClass;
}

bool Type::isBaseType() const
{
	return getTag() == SymTagBaseType;
}

bool Type::isArray() const
{
	return getTag() == SymTagArrayType;
}

bool Type::isPointer() const
{
	return getTag() == SymTagPointerType;
}

bool Type::isFunctionType() const
{
	return getTag() == SymTagFunctionType;
}

bool Type::isEnum() const
{
	return getTag() == SymTagEnum;
}

bool Type::isUDT() const
{
	return getTag() == SymTagUDT;
}

bool Type::isData() const
{
	return getTag() == SymTagData;
}


std::string Type::getName() const
{
	return m_typeName;
}

bool Type::hasBaseType(const Type& baseType, bool recurse/*=true*/) const
{
	//printf("Checking if %s has %s as basetype\n", m_typeName.c_str(), baseType->m_typeName.c_str());
	//printf("%s\n%s\n", toString().c_str(), baseType->toString().c_str());
	auto baseTypes = getBaseClasses();
	auto callback = [&baseType, recurse](const Type& baseClass)
	{
		auto&& baseClassType = baseClass.getType();
		auto baseClassIsFound = baseClassType == baseType;
		return baseClassIsFound || (recurse && baseClassType.hasBaseType(baseType));
	};
	auto found = std::find_if(baseTypes.begin(), baseTypes.end(), callback);
	return found != baseTypes.end();
}

std::string Type::toString() const
{
	auto thisTag = getTag();
	auto type = SymTag(thisTag).toString();

	switch (thisTag)
	{
	case SymTagFunctionArgType:
		return std::string("argument which is ") + getType().toString();
	case SymTagPointerType:
		return std::string("pointer to ") + getType().toString();
	case SymTagNull:
		return "null";
	case SymTagFunction:
		return std::string("function(do more)");
	case SymTagUDT:
		return std::string("UDT named ") + getName() + " (do more)";
	case SymTagFunctionType:
	{
		auto ret = getReturnType().toString() + "(";
		auto args = getParameters();
		for (const auto& arg : args)
		{
			ret += arg.toString() + ", ";
		}
		return ret + ")";		
	}
	case SymTagBaseType:
	{
		auto baseType = getBaseType();
		auto size = getSize();

		return getBaseTypeString(baseType, size);
	}
	case SymTagExe:
	case SymTagCompiland:
	case SymTagCompilandDetails:
	case SymTagCompilandEnv:
	case SymTagBlock:
	case SymTagData:
	case SymTagAnnotation:
	case SymTagLabel:
	case SymTagPublicSymbol:
	case SymTagEnum:
	case SymTagArrayType:
	case SymTagTypedef:
	case SymTagBaseClass:
	case SymTagFriend:
	case SymTagFuncDebugStart:
	case SymTagFuncDebugEnd:
	case SymTagUsingNamespace:
	case SymTagVTableShape:
	case SymTagVTable:
	case SymTagCustom:
	case SymTagThunk:
	case SymTagCustomType:
	case SymTagManagedType:
	case SymTagDimension:
	case SymTagCallSite:
		return false;// I guess dunno
	}

	return strprintf(
		"Name: %s\n"
		"Type: %s\n"
		"TypeIdx: %d\n"
		, m_typeName.c_str()
		, type.c_str()
		, m_typeIndex
	);

}
