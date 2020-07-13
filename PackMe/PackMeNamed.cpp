#include "PackMeNamed.h"

PackMeNamed::PackMeNamed()
{

}

PackMeNamed::PackMeNamed(const char* pFile, bool bRead /*= true*/)
	: PackMe()
{
	SetFile(pFile, bRead);
}

PackMeNamed::~PackMeNamed()
{
}

bool PackMeNamed::SetFile(const char* pFile, bool bRead)
{
	bool b = PackMe::SetFile(pFile, bRead);
	if (!b)
		return b;

	if (bRead && m_vctBlocksPositions.size() > 0)
	{
		int len = GetDataLen((int)m_vctBlocksPositions.size() - 1);
		char* pNamedData = new char[len];
		ReadData((int)m_vctBlocksPositions.size() - 1, pNamedData);
		char* pData = pNamedData;
		//读有多少个名
		int mapItemCount = *((int *)pData);
		pData = pData + sizeof(mapItemCount);
		for (int i = 0; i < mapItemCount; ++i)
		{
			//分别读取 namelen|name|idx
			long aNameLen = *((long *)pData);
			pData = pData + sizeof(aNameLen);
			std::string sName(pData, aNameLen);
			pData = (pData + aNameLen);
			int idx = *((int *)pData);
			m_mpNameMap.insert(std::make_pair(sName, idx));
			pData = pData + sizeof(idx);
		}
		delete[] pNamedData;
	}

	return true;
}

int PackMeNamed::AddBlock(const char* pData, long sizeData, const char* name)
{
	int idx = PackMe::AddBlock(pData, sizeData);
	m_mpNameMap.insert(std::make_pair(name, idx));
	return idx;
}

int PackMeNamed::BeginBlock(const char* name)
{
	int idx = PackMe::BeginBlock();
	m_mpNameMap.insert(std::make_pair(name, idx));
	return idx;
}

long PackMeNamed::writeIndexs()
{
	PackMe::BeginBlock();

	int namesCount = m_mpNameMap.size();
	AppendBlockData((const char*)&namesCount, sizeof(namesCount));
	for (std::map<std::string, int>::iterator it = m_mpNameMap.begin();
		it != m_mpNameMap.end(); ++it)
	{
		//namelen|name|idx
		long nameLen = strlen(it->first.c_str());
		AppendBlockData((const char*)&nameLen, sizeof(nameLen));
		AppendBlockData(it->first.c_str(), nameLen);
		AppendBlockData((const char*)&(it->second), sizeof(it->second));
	}
	EndBlock();

	return PackMe::writeIndexs();
}

int PackMeNamed::IndexCount()
{
	return (int)PackMe::IndexCount() - 1;//减去一个named段
}

int PackMeNamed::GetIndexByName(const char* name)
{
	if (m_mpNameMap.find(name) == m_mpNameMap.end())
		return -1;

	return m_mpNameMap[name];
}

int PackMeNamed::NamedCount()
{
	return (int)m_mpNameMap.size();
}

const char* PackMeNamed::GetName(int idxName)
{
	if (idxName >= (int)m_mpNameMap.size())
		return NULL;
	std::map<std::string, int>::iterator it = m_mpNameMap.begin();
	for (int i = 0; i < idxName && it != m_mpNameMap.end(); ++i)
		++it;
	return it->first.c_str();
}

long PackMeNamed::GetNamedDataLen(const char* name)
{
	int idx = GetIndexByName(name);
	if (idx < 0)
		return idx;

	return GetDataLen((int)idx);
}

long PackMeNamed::ReadNamedData(const char* name, char* pData)
{
	int idx = GetIndexByName(name);
	if (idx < 0)
		return -1;

	return ReadData(idx, pData);
}

std::string PackMeNamed::ReadNamedString(const std::string& name)
{
	long len = GetNamedDataLen(name.c_str());
	if (len <= 0)
		return "";

	char* pData = new char[len + 1];
	len = ReadNamedData(name.c_str(), pData);
	pData[len] = '\0';
	std::string sStr = pData;
	delete[] pData;
	return sStr;
}

