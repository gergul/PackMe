#include "PackMeNamed.h"


PackMeNamed::PackMeNamed(const char* pFile, bool bRead /*= true*/)
	: PackMe(pFile, bRead)
{
	if (bRead && m_vctIndexs.size() > 0)
	{
		int len = GetDataLen(m_vctIndexs.size() - 1);
		char* pNamedData = new char[len];		
		ReadData(m_vctIndexs.size() - 1, pNamedData);
		char* pData = pNamedData;
		//读有多少个名
		long mapItemCount = *((long *)pData);
		pData = pData + sizeof(long);
		for (int i = 0; i < mapItemCount; ++i)
		{
			//分别读取 namelen|name|idx
			long aNameLen = *((long *)pData);
			pData = pData + sizeof(long);
			std::string sName(pData, aNameLen);
			pData = (pData + aNameLen);
			long idx = *((long *)pData);
			m_mpNameMap.insert(std::make_pair(sName, idx));
			pData = pData + sizeof(long);
		}
		delete[] pNamedData;
	}
}

PackMeNamed::~PackMeNamed()
{
}

long PackMeNamed::AddBlock(const char* pData, long sizeData, const char* name)
{
	long ret = PackMe::AddBlock(pData, sizeData);
	m_mpNameMap.insert(std::make_pair(name, m_vctIndexs.size() - 1));
	return ret;
}

long PackMeNamed::BeginBlock(const char* name)
{
	long ret = PackMe::BeginBlock();
	m_mpNameMap.insert(std::make_pair(name, m_vctIndexs.size() - 1));
	return ret;
}

long PackMeNamed::writeIndexs()
{
	PackMe::BeginBlock();
	long namesCount = m_mpNameMap.size();
	AppendBlockData((const char*)&namesCount, sizeof(namesCount));
	for (std::map<std::string, long>::iterator it = m_mpNameMap.begin();
		it != m_mpNameMap.end(); ++it)
	{
		long nameLen = strlen(it->first.c_str());
		AppendBlockData((const char*)&nameLen, sizeof(nameLen));
		AppendBlockData(it->first.c_str(), nameLen);
		AppendBlockData((const char*)&(it->second), sizeof(it->second));
	}
	EndBlock();

	return PackMe::writeIndexs();
}

long PackMeNamed::IndexCount()
{
	return PackMe::IndexCount() - 1;//减去一个named段
}

long PackMeNamed::GetIndexByName(const char* name)
{
	if (m_mpNameMap.find(name) == m_mpNameMap.end())
		return -1;

	return m_mpNameMap[name];
}

long PackMeNamed::NamedCount()
{
	return m_mpNameMap.size();
}

const char* PackMeNamed::GetName(int idxName)
{
	if (idxName >= m_mpNameMap.size())
		return NULL;
	std::map<std::string, long>::iterator it = m_mpNameMap.begin();
	for (int i = 0; i < idxName && it != m_mpNameMap.end(); ++i)
		++it;
	return it->first.c_str();
}

