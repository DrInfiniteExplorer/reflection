#include "reflection\SymbolLoaderDIA.h"

#include "reflection\ISymbol.h"

SymbolLoaderDIA::SymbolLoaderDIA()
	: m_initialized(false)
{
}

SymbolLoaderDIA::~SymbolLoaderDIA()
{
}

bool SymbolLoaderDIA::addSymbolSource(const std::string& symbolSource)
{
	if (!isInitialized())
	{
		initialize();
	}

	CComPtr<IDiaDataSource> pSource;
	auto hr = CoCreateInstance(CLSID_DiaSource,
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(IDiaDataSource),
		(void **)&pSource);

	if (FAILED(hr))
	{
		throw std::runtime_error("Could not CoCreate CLSID_DiaSource. Register msdia80.dll.");
	}

	wchar_t wszFilename[_MAX_PATH];
	swprintf(wszFilename, sizeof(wszFilename) / sizeof(wszFilename[0]), L"%S", symbolSource.c_str());
	if (FAILED(pSource->loadDataFromPdb(wszFilename)))
	{
		if (FAILED(pSource->loadDataForExe(wszFilename, NULL, NULL)))
		{
			throw std::runtime_error("loadDataFromPdb/Exe");
		}
	}
	return true;
}

std::shared_ptr<ISymbol> SymbolLoaderDIA::getSymbolFromAddress(uintptr_t symbolAddress)
{
	if (!isInitialized())
	{
		return nullptr;
	}

	std::shared_ptr<ISymbol> ret;
	ret.reset<ISymbol>(nullptr);
	return ret;
}

std::shared_ptr<ISymbol> SymbolLoaderDIA::getSymbolFromName(const std::string& symbolName)
{
	if (!isInitialized())
	{
		return nullptr;
	}

	std::shared_ptr<ISymbol> ret;
	ret.reset<ISymbol>(nullptr);
	return ret;
}

void SymbolLoaderDIA::initialize()
{
	if (m_initialized)
	{
		return;
	}
	auto hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		throw std::runtime_error("Could not CoInitialize");
	}

}

bool SymbolLoaderDIA::isInitialized()
{
	return m_initialized;
}
