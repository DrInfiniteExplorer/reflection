#pragma once

#include <atlbase.h>
#include <dia2.h>

#include "ISymbolLoader.h"

class SymbolLoaderDIA : public ISymbolLoader
{
public:
//	void setLoaderOptions(const SymbolOptions& loaderOptions) override
//	{
//		m_loaderOptions = loaderOptions;
//	}
	SymbolLoaderDIA();
	virtual ~SymbolLoaderDIA();

	bool addSymbolSource(const std::string& symbolSource) override;

	std::shared_ptr<ISymbol> getSymbolFromAddress(uintptr_t address) override;
	std::shared_ptr<ISymbol> getSymbolFromName(const std::string& symbolName) override;
private:

	void initialize();
	bool isInitialized();

//	SymbolOptions m_loaderOptions;
	CComPtr<IDiaDataSource> m_pDiaDataSource;

	bool m_initialized;
};

