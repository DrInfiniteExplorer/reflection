#pragma once

#include <string>
#include <memory>

class ISymbol;

class ISymbolLoader
{
public:
	virtual ~ISymbolLoader() {};

	//virtual void setLoaderOptions(const SymbolOptions&) = 0;

	virtual bool addSymbolSource(const std::string& symbolSource) = 0;

	virtual std::shared_ptr<ISymbol> getSymbolFromAddress(uintptr_t address) = 0;
	virtual std::shared_ptr<ISymbol> getSymbolFromName(const std::string& symbolName) = 0;
private:
};

