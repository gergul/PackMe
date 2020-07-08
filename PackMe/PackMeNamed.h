#pragma once
#include "PackMe.h"
#include <string>
#include <map>

class PackMeNamed
	: public PackMe
{
public:
	PackMeNamed(const char* pFile, bool bRead = true);
	virtual ~PackMeNamed();

	long AddBlock(const char* pData, long sizeData, const char* name);

	long BeginBlock(const char* name);
			
	virtual long IndexCount();
	long GetIndexByName(const char* name);

	long NamedCount();
	const char* GetName(int idxName);

protected:
	virtual long writeIndexs();

private:
	std::map<std::string, long> m_mpNameMap;
};

