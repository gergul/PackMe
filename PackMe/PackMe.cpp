#include "PackMe.h"


PackMe::PackMe(const char* pFile, bool bRead/*=true*/)
	: m_pTargetFile(NULL)
	, m_bRead(bRead)
{
	if (bRead)
	{
		startRead(pFile);
	}
	else
	{
		startWrite(pFile);
	}
}

PackMe::~PackMe()
{
	Close();
}

bool PackMe::IsValid()
{
	return m_pTargetFile;
}

bool PackMe::startWrite(const char* pFile)
{
	if ((m_pTargetFile = fopen(pFile, "rb+")) == NULL)
	{
		if ((m_pTargetFile = fopen(pFile, "wb+")) == NULL)
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
		//����ʶ
		if (!IsPacked())
		{
			bRet = false;
			break;
		}

		//����Ŀ¼λ�ö�ȡ��
		//                                 ��
		//.................................|Ŀ¼λ��:long|ID.
		if (fseek(m_pTargetFile, -(strlen(PAK_ID) + sizeof(long)), SEEK_END) < 0)
		{
			bRet = false;
			break;
		}
		
		//��ȡĿ¼λ��
		long posIndexs = -1;
		fread(&posIndexs, sizeof(posIndexs), 1, m_pTargetFile);
		if (-1 == posIndexs)
		{
			bRet = false;
			break;
		}

		//����Ŀ¼λ�ö�ȡ��
		fseek(m_pTargetFile, posIndexs, SEEK_SET);
		//��ȡĿ¼��
		int indexsCount = -1;
		fread(&indexsCount, sizeof(indexsCount), 1, m_pTargetFile);
		//��ȡĿ¼��
		for (int i = 0; i < indexsCount; ++i)
		{
			long pos = -1;
			fread(&pos, sizeof(pos), 1, m_pTargetFile);
			m_vctBlocksPositions.push_back(pos);
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

int PackMe::AddBlock(const char* pData, long sizeData)
{
	//�����ļ�β���������
	fseek(m_pTargetFile, 0, SEEK_END);
	m_vctBlocksPositions.push_back(ftell(m_pTargetFile));

	//д��С
	fwrite(&sizeData, sizeof(sizeData), 1, m_pTargetFile);
	//д����
	fwrite(pData, sizeData, 1, m_pTargetFile);

	return m_vctBlocksPositions.size() - 1;
}

//            ��(m_vctIndexs���һ��Ԫ��)
//(ԭ�ļ�����)|���С(long)|������....|PM
int PackMe::BeginBlock()
{
	//�����ļ�β���������
	fseek(m_pTargetFile, 0, SEEK_END);
	m_vctBlocksPositions.push_back(ftell(m_pTargetFile));

	//д��С��Ԥ��λ�ã���λ����EndBlockʱ����
	long sizeData = 0;
	fwrite(&sizeData, sizeof(sizeData), 1, m_pTargetFile);

	return m_vctBlocksPositions.size() - 1;
}

long PackMe::AppendBlockData(const char* pData, long sizeData)
{
	fwrite(pData, sizeData, 1, m_pTargetFile);

	return ftell(m_pTargetFile);
}

long PackMe::EndBlock()
{
	//�ص����Сλ�ã���д���С����
	long blockDataLen = (ftell(m_pTargetFile) - m_vctBlocksPositions[m_vctBlocksPositions.size() - 1]) - sizeof(blockDataLen);
	fseek(m_pTargetFile, m_vctBlocksPositions[m_vctBlocksPositions.size() - 1], SEEK_SET);
	fwrite(&blockDataLen, sizeof(blockDataLen), 1, m_pTargetFile);

	return ftell(m_pTargetFile);
}

int PackMe::IndexCount()
{
	return (int)m_vctBlocksPositions.size();
}

int PackMe::FirstIndex()
{
	return IndexCount() - 1;
}

int PackMe::LastIndex()
{
	if (m_vctBlocksPositions.size() == 0)
		return -1;
	return (IndexCount() - 1);
}

long PackMe::GetDataLen(int idx)
{
	if (idx >= (int)m_vctBlocksPositions.size())
		return -1;

	fseek(m_pTargetFile, m_vctBlocksPositions[idx], SEEK_SET);

	long sizeChunk = -1;
	fread(&sizeChunk, sizeof(sizeChunk), 1, m_pTargetFile);
	return sizeChunk;
}

long PackMe::ReadData(int idx, char* pData)
{
	if (idx >= (int)m_vctBlocksPositions.size())
		return -1;

	fseek(m_pTargetFile, m_vctBlocksPositions[idx], SEEK_SET);
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
	//   ��posIndexs
	//...(Ŀ¼����:int|��1��ȡ��ʼλ��:long|��2��ȡ��ʼλ��:long))
	fseek(m_pTargetFile, 0, SEEK_END);
	long posIndexs = ftell(m_pTargetFile);

	//д��С
	int idxCount = (int)m_vctBlocksPositions.size();
	fwrite(&idxCount, sizeof(idxCount), 1, m_pTargetFile);
	//д�б�
	for (int i = 0; i < m_vctBlocksPositions.size(); ++i)
	{
		fwrite(&m_vctBlocksPositions[i], sizeof(m_vctBlocksPositions[i]), 1, m_pTargetFile);
	}

	return posIndexs;
}

bool PackMe::endWrite()
{
	if (!m_pTargetFile)
		return false;

	//   ��posIndexs
	//...(Ŀ¼����:int|��1��ȡ��ʼλ��:long|��2��ȡ��ʼλ��:long)(posIndexs|"PM")
	long posIndexs = writeIndexs();

	//�����д������ʶ
	fseek(m_pTargetFile, 0, SEEK_END);
	//write posIndexs
	fwrite(&posIndexs, sizeof(posIndexs), 1, m_pTargetFile);
	//write ��ʶ
	fwrite(PAK_ID, strlen(PAK_ID), 1, m_pTargetFile);
	
	return true;
}


bool PackMe::endRead()
{
	if (!m_pTargetFile)
		return false;

	return true;
}

bool PackMe::IsPacked()
{
	//������ʶλ�ö�ȡ��
	int lenId = strlen(PAK_ID);
	if (fseek(m_pTargetFile, -lenId, SEEK_END) < 0)
	{
		return false;
	}
	//����ʶ
	char* pID = new char[lenId + 1];
	fread(pID, lenId, 1, m_pTargetFile);
	pID[lenId] = '\0';
	if (strcmp(pID, PAK_ID) != 0)
	{
		return false;
	}

	return true;
}
