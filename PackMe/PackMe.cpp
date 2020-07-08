#include "PackMe.h"


PackMe::PackMe(const char* pFile, bool bRead/*=true*/)
	: m_pTargetFile(NULL)
	, m_bRead(bRead)
{
	bool bSucess = false;
	if (bRead)
	{
		bSucess = startRead(pFile);
	}
	else
	{
		bSucess = startWrite(pFile);
	}

	if (!bSucess)
		throw std::exception("FAIL_OPEN_FILE");
}

PackMe::~PackMe()
{
	if (m_pTargetFile)
	{
		fclose(m_pTargetFile);
		m_pTargetFile = NULL;
	}
}

bool PackMe::startWrite(const char* pFile)
{
	if ((m_pTargetFile = fopen(pFile, "rb+")) == NULL)
	{
		return false;
	}

	bool bRet = false;
	do 
	{
		if (fseek(m_pTargetFile, 0, SEEK_END) < 0)
		{
			bRet = false;
			break;
		}

		bRet = true;
	} while (0);
	
	if (!bRet)
	{
		fclose(m_pTargetFile);
		m_pTargetFile = NULL;
	}


	return bRet;
}

bool PackMe::startRead(const char* pFile)
{
	if ((m_pTargetFile = fopen(pFile, "rb")) == NULL)
	{
		return false;
	}

	bool bRet = false;
	do
	{
		//检查标识
		if (!IsPacked())
		{
			bRet = false;
			break;
		}

		//跳到目录位置读取处
		//.................................|目录位置|ID.
		if (fseek(m_pTargetFile, -(strlen(PAK_ID) + sizeof(long)), SEEK_END) < 0)
		{
			bRet = false;
			break;
		}
		
		//读取目录位置
		long posIndexs = -1;
		fread(&posIndexs, sizeof(posIndexs), 1, m_pTargetFile);
		if (-1 == posIndexs)
		{
			bRet = false;
			break;
		}

		//跳到目录位置读取处
		fseek(m_pTargetFile, posIndexs, SEEK_SET);
		//读取目录数
		long indexsCount = -1;
		fread(&indexsCount, sizeof(indexsCount), 1, m_pTargetFile);
		//读取目录项
		for (int i = 0; i < indexsCount; ++i)
		{
			long pos = -1;
			fread(&pos, sizeof(pos), 1, m_pTargetFile);
			m_vctIndexs.push_back(pos);
		}

		bRet = true;
	} while (0);

	if (!bRet)
	{
		fclose(m_pTargetFile);
		m_pTargetFile = NULL;
	}

	return bRet;
}

long PackMe::AddBlock(const char* pData, long sizeData)
{
	//跳到文件尾以添加数据
	fseek(m_pTargetFile, 0, SEEK_END);
	m_vctIndexs.push_back(ftell(m_pTargetFile));

	//写大小
	fwrite(&sizeData, sizeof(sizeData), 1, m_pTargetFile);
	//写内容
	fwrite(pData, sizeData, 1, m_pTargetFile);

	return m_vctIndexs[m_vctIndexs.size() - 1];
}

long PackMe::BeginBlock()
{
	//跳到文件尾以添加数据
	fseek(m_pTargetFile, 0, SEEK_END);
	m_vctIndexs.push_back(ftell(m_pTargetFile));

	//写大小的预留位置
	long sizeData = 0;
	fwrite(&sizeData, sizeof(sizeData), 1, m_pTargetFile);

	return m_vctIndexs[m_vctIndexs.size() - 1];
}

long PackMe::AppendBlockData(const char* pData, long sizeData)
{
	fwrite(pData, sizeData, 1, m_pTargetFile);

	return ftell(m_pTargetFile);
}

long PackMe::EndBlock()
{
	long blockLen = ftell(m_pTargetFile) - m_vctIndexs[m_vctIndexs.size() - 1] - sizeof(long);
	fseek(m_pTargetFile, m_vctIndexs[m_vctIndexs.size() - 1], SEEK_SET);
	fwrite(&blockLen, sizeof(blockLen), 1, m_pTargetFile);

	return ftell(m_pTargetFile);
}

long PackMe::IndexCount()
{
	return m_vctIndexs.size();
}

long PackMe::GetDataLen(int idx)
{
	if (idx >= m_vctIndexs.size())
		return -1;

	fseek(m_pTargetFile, m_vctIndexs[idx], SEEK_SET);

	long sizeChunk = -1;
	fread(&sizeChunk, sizeof(sizeChunk), 1, m_pTargetFile);
	return sizeChunk;
}

long PackMe::ReadData(int idx, char* pData)
{
	if (idx >= m_vctIndexs.size())
		return -1;

	fseek(m_pTargetFile, m_vctIndexs[idx], SEEK_SET);
	long nDataSize = GetDataLen(idx);//cur seeked
	fread(pData, nDataSize, 1, m_pTargetFile);

	return nDataSize;
}

void PackMe::Close()
{
	if (m_bRead)
	{
		endRead();
	}
	else
	{
		endWrite();
	}

	if (m_pTargetFile)
	{
		fclose(m_pTargetFile);
		m_pTargetFile = NULL;
	}
}

long PackMe::writeIndexs()
{
	fseek(m_pTargetFile, 0, SEEK_END);
	long posIndexs = ftell(m_pTargetFile);

	//写大小
	long idxCount = m_vctIndexs.size();
	fwrite(&idxCount, sizeof(idxCount), 1, m_pTargetFile);
	//写列表
	for (int i = 0; i < m_vctIndexs.size(); ++i)
	{
		fwrite(&m_vctIndexs[i], sizeof(m_vctIndexs[i]), 1, m_pTargetFile);
	}


	return posIndexs;
}

bool PackMe::endWrite()
{
	long posIndexs = writeIndexs();

	//到最后写结束标识
	fseek(m_pTargetFile, 0, SEEK_END);
	//write posIndexs
	fwrite(&posIndexs, sizeof(posIndexs), 1, m_pTargetFile);
	//write 标识
	fwrite(PAK_ID, strlen(PAK_ID), 1, m_pTargetFile);

	//close
	fclose(m_pTargetFile);
	m_pTargetFile = NULL;

	return true;
}


bool PackMe::endRead()
{
	//close
	fclose(m_pTargetFile);
	m_pTargetFile = NULL;

	return true;
}

bool PackMe::IsPacked()
{
	//跳到标识位置读取处
	long lenId = strlen(PAK_ID);
	if (fseek(m_pTargetFile, -lenId, SEEK_END) < 0)
	{
		return false;
	}
	//检查标识
	char* pID = new char[lenId + 1];
	fread(pID, lenId, 1, m_pTargetFile);
	pID[lenId] = '\0';
	if (strcmp(pID, PAK_ID) != 0)
	{
		return false;
	}

	return true;
}
