#pragma once
#include <stdio.h>
#include <vector>

/*
	存储格式：
	(原文件内容...)(段1长度:long|段1内容...)(段2长度:long|段2内容...)(目录数量:long|段1读取起始位置:long|段2读取起始位置:long)(目录读取起始位置:long|"PM")
*/

#define PAK_ID "PM"

class PackMe
{
public:
	PackMe(const char* pFile, bool bRead=true);
	virtual ~PackMe();

public:
	//增加一个块
	virtual long AddBlock(const char* pData, long sizeData);
	
public:
	//开始写一个块
	virtual long BeginBlock();
	//增加块内容
	virtual long AppendBlockData(const char* pData, long sizeData);
	//结束写一个块
	virtual long EndBlock();

public:
	//索引数
	virtual long IndexCount();
	//获得块的长度
	virtual long GetDataLen(int idx);
	//读取块内容
	virtual long ReadData(int idx, char* pData);
	
public:
	//关闭
	virtual void Close();

public:
	template<typename T>
	static char* GetBaseTypeInData(const char* pData, T& got)
	{
		got = *((T*)pData);
		char* p = const_cast<char*>(pData);
		p = p + sizeof(T);
		return p;
	}

	bool CheckID();

protected:
	virtual bool startWrite(const char* pFile);
	virtual bool startRead(const char* pFile);

	//返回目录位置
	virtual long writeIndexs();
	virtual bool endWrite();

	virtual bool endRead();

protected:
	FILE* m_pTargetFile;
	bool  m_bRead;

	std::vector<long> m_vctIndexs;
};

