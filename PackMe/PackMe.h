#pragma once
#include <stdio.h>
#include <vector>

/*
	�洢��ʽ��
	(ԭ�ļ�����...)(��1����:long|��1����...)(��2����:long|��2����...)(Ŀ¼����:int|��1��ȡ��ʼλ��:long|��2��ȡ��ʼλ��:long)(Ŀ¼��ȡ��ʼλ��:long|"PM")
*/

#define PAK_ID "PM"

class PackMe
{
public:
	PackMe();
	PackMe(const char* pFile, bool bRead);
	virtual ~PackMe();

	virtual bool SetFile(const char* pFile, bool bRead);

public:
	virtual bool IsValid();
	virtual bool IsPacked();

public:
	//����һ����
	virtual int AddBlock(const char* pData, long sizeData);
	
public:
	//��ʼдһ����
	virtual int BeginBlock();
	//���ӿ�����
	virtual long AppendBlockData(const char* pData, long sizeData);
	//����дһ����
	virtual long EndBlock();

public:
	//������
	virtual int IndexCount();
	virtual int FirstIndex();
	virtual int LastIndex();
	//��ÿ�ĳ���
	virtual long GetDataLen(int idx);
	//��ȡ������
	virtual long ReadData(int idx, char* pData);
	
public:
	//�ر�
	virtual void Close();

public:
	/* �������л��ĳ���͵����ݣ�������һ�����ݵĿ�ʼλ�� */
	template<typename T>
	static char* GetDataByType(const char* pData, T& typedData)
	{
		typedData = *((T*)pData);
		char* p = const_cast<char*>(pData);
		p = p + sizeof(T);
		return p;
	}
		
protected:
	virtual bool startWrite(const char* pFile);
	virtual bool startRead(const char* pFile);

	//����Ŀ¼λ��
	virtual long writeIndexs();
	virtual bool endWrite();

	virtual bool endRead();

protected:
	FILE* m_pTargetFile;
	bool  m_bRead;

	std::vector<long> m_vctBlocksPositions;
};

