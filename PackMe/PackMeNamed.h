#pragma once
#include "PackMe.h"
#include <string>
#include <map>

class PackMeNamed
	: public PackMe
{
public:
	PackMeNamed();
	PackMeNamed(const char* pFile, bool bRead = true);
	virtual ~PackMeNamed();

	virtual bool SetFile(const char* pFile, bool bRead) override;

	virtual int AddBlock(const char* pData, long sizeData, const char* name);
	virtual int BeginBlock(const char* name);

	virtual int IndexCount() override;
			
public:
	virtual int GetIndexByName(const char* name);

	virtual int NamedCount();
	virtual const char* GetName(int idxName);
	
	virtual long GetNamedDataLen(const char* name);	
	virtual long ReadNamedData(const char* name, char* pData);
	virtual std::string ReadNamedString(const std::string& name);

protected:
	virtual long writeIndexs();

private:
	std::map<std::string, int> m_mpNameMap;
};

