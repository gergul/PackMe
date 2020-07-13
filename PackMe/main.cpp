#include "PackMe.h"
#include "PackMeNamed.h"
#include <Windows.h>
#include <shlwapi.h>
#include "ArgumentMan.h"

void ListFilesA(const char* lpPath, std::vector<std::string>& vctDir, std::vector<std::string>& vctFiles, const char* filter = "*.*", bool bSubDir = true, bool bAppendPath = true)
{
	char szFind[MAX_PATH] = { 0 };
	StrCpyA(szFind, lpPath);
	StrCatA(szFind, "/");
	StrCatA(szFind, filter);

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = ::FindFirstFileA(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)
		return;

	char szFile[MAX_PATH] = { 0 };
	while (TRUE)
	{
		szFile[0] = '\0';
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				if (bAppendPath)
				{
					StrCpyA(szFile, lpPath);
					StrCatA(szFile, "\\");
					StrCatA(szFile, FindFileData.cFileName);
				}
				else
				{
					StrCpyA(szFile, FindFileData.cFileName);
				}
				vctDir.push_back(szFile);

				if (bSubDir)
				{
					ListFilesA(szFile, vctDir, vctFiles, filter, bSubDir, bAppendPath);
				}
			}
		}
		else
		{
			if (bAppendPath)
			{
				StrCpyA(szFile, lpPath);
				StrCatA(szFile, "\\");
				StrCatA(szFile, FindFileData.cFileName);
			}
			else
			{
				StrCpyA(szFile, FindFileData.cFileName);
			}
			vctFiles.push_back(szFile);
		}

		if (!FindNextFileA(hFind, &FindFileData))
			break;
	}

	FindClose(hFind);
}

std::string GetDirFromPath(const std::string& sPath)
{
	int len = strlen(sPath.c_str());
	std::string path = sPath;
	char c = path[len - 1];
	if (c == '\\' || c == '/')
		path = path.substr(0, len - 1);

	size_t split1 = path.find_last_of('/');
	size_t split2 = path.find_last_of('\\');
	size_t split;
	if (split1 == std::string::npos && split2 != std::string::npos)
	{
		split = split2;
	}
	else if (split1 != std::string::npos && split2 == std::string::npos)
	{
		split = split1;
	}
	else if (split1 != std::string::npos && split2 != std::string::npos)
	{
		split = max(split1, split2);
	}
	else
	{
		return "";
	}
	
	path = path.substr(0, split);

	return path;
}

std::string GetFileNameFromPath(const std::string& sPath)
{
	int len = strlen(sPath.c_str());
	char c = sPath[len - 1];
	if (c == '\\' || c == '/')
	{
		return "";
	}

	std::string path = sPath;
	size_t split1 = path.find_last_of('/');
	size_t split2 = path.find_last_of('\\');
	size_t split;
	if (split1 == std::string::npos && split2 != std::string::npos)
	{
		split = split2;
	}
	else if (split1 != std::string::npos && split2 == std::string::npos)
	{
		split = split1;
	}
	else if (split1 != std::string::npos && split2 != std::string::npos)
	{
		split = max(split1, split2);
	}
	else
	{
		return sPath;
	}
	
	return path.substr(split + 1);
}

bool CreateMultipleDirectory(const char* szPath)
{
	std::string strDir(szPath);//���Ҫ������Ŀ¼�ַ���
	//ȷ����'\'��β�Դ������һ��Ŀ¼
	if (strDir[strDir.length() - 1] != '\\' )
	{
		strDir += '\\';
	}
	std::vector<std::string> vPath;//���ÿһ��Ŀ¼�ַ���
	std::string strTemp;//һ����ʱ����,���Ŀ¼�ַ���
	bool bSuccess = false;//�ɹ���־
	//����Ҫ�������ַ���
	for (int i = 0; i < strDir.length(); ++i)
	{
		if (strDir[i] != '\\')
		{//�����ǰ�ַ�����'\\'
			strTemp += strDir[i];
		}
		else
		{//�����ǰ�ַ���'\\'
			vPath.push_back(strTemp);//����ǰ����ַ�����ӵ�������
			strTemp += '\\';
		}
	}

	//�������Ŀ¼������,����ÿ��Ŀ¼
	std::vector<std::string>::const_iterator vIter;
	for (vIter = vPath.begin(); vIter != vPath.end(); vIter++)
	{
		//���CreateDirectoryִ�гɹ�,����true,���򷵻�false
		bSuccess = CreateDirectoryA(vIter->c_str(), NULL) ? true : false;
	}

	return bSuccess;
}

bool StringReplaceA(std::string& strBase, const std::string& strSrc, const std::string& strDes)
{
	bool b = false;

	std::string::size_type pos = 0;
	std::string::size_type srcLen = strSrc.size();
	std::string::size_type desLen = strDes.size();
	pos = strBase.find(strSrc, pos);
	while ((pos != std::string::npos))
	{
		strBase.replace(pos, srcLen, strDes);
		pos = strBase.find(strSrc, (pos + desLen));
		b = true;
	}

	return b;
}
size_t ReadFile(const char* path, std::vector<char>& data, size_t start = 0)
{
	FILE *f = NULL;
	long sz;
	size_t nToRead = 0;
	size_t nFileSize = 0;

	if (!path)
	{
		return 0;
	}

	f = fopen(path, "rb");
	if (!f)
	{
		return 0;
	}

	do
	{
		if (fseek(f, 0, SEEK_END) < 0)
		{
			break;
		}

		sz = ftell(f);
		if (sz < 0)
		{
			break;
		}
		nFileSize = (size_t)sz;
		if (start >= nFileSize)
		{
			break;
		}

		if (fseek(f, start, SEEK_SET) < 0)
		{
			break;
		}

		nToRead = nFileSize - start;
		data.resize(nToRead);

		size_t nReaded = fread(data.data(), 1, nToRead, f);
		if (nReaded > nToRead)
		{
			nToRead = nReaded;
			break;
		}
	} while (0);

	fclose(f);

	return nFileSize;
}



void main(int argc, char** argv)
{	
	AM::CArgumentMan<> am;
	am.SetIgnoreCase(true);
	am.SetMultiValue(true);
	am.AddCmdFlag('-');
	am.AddCmdFlag('/');
	am.AddCmdFlag('\\');
	am.Parse(argc, argv);

	bool bRead = true;

	std::string sOutFile = am.GetArg("o");
	if (sOutFile.length() < 1)
	{
		printf("-o targetFile -f file1 [file2...] -d dir1 [dir2...] -t name1 text1 [name2 text2...]\n");
		return;
	}

	std::vector<std::string> vctFiles = am.GetArgs("f");
	std::vector<std::string> vctDirs = am.GetArgs("d");
	std::vector<std::string> vctDir;
	for (int i = 0; i < vctDirs.size(); ++i)
	{
		ListFilesA(vctDirs[i].c_str(), vctDir, vctFiles);
	}

	std::vector<std::string> vctTexts = am.GetArgs("t");
	
	if (vctFiles.size() > 0 || vctTexts.size() > 0)
		bRead = false;
	else
		bRead = true;

	bool bBak = false;
	std::string sBak;
	if (am.Has(sBak, "bak") && !bRead)
		bBak = true;

	PackMeNamed pak(sOutFile.c_str(), bRead);
	if (!pak.IsValid())
	{
		printf("��Ŀ���ļ�ʧ�ܡ�\n");
		return;
	}

	if (!bRead)
	{//write
		if (pak.IsPacked())
		{
			printf("�Ѿ��������!\n");
			return;
		}

		//����
		if (bBak)
			CopyFileA(sOutFile.c_str(), (sOutFile + ".bak").c_str(), FALSE);

		for (int i = 0; i < vctFiles.size(); ++i)
		{
			const char* pFileName = vctFiles[i].c_str();

			pak.BeginBlock(pFileName);

			//file content
			std::vector<char> data;
			ReadFile(pFileName, data);
			if (data.size() == 0)
			{
				printf("��ȡ�ļ�ʧ��%s\n", pFileName);
			}
			else
			{
				printf("%s\n", pFileName);
			}
			pak.AppendBlockData(data.data(), data.size());

			pak.EndBlock();
		}

		for (int i = 0; i < vctTexts.size(); i += 2)
		{
			std::string& name = vctTexts[i];
			std::string& text = vctTexts[i + 1];

			pak.BeginBlock(name.c_str());
			//file content
			pak.AppendBlockData(text.c_str(), text.size());
			pak.EndBlock();
		}
	}
	else
	{//read
		int nCount = pak.NamedCount();
		for (int i = 0; i < nCount; ++i)
		{
			const char* pName = pak.GetName(i);
			if (!pName)
				continue;

			//��ȫ������
			std::vector<char> data;
			if (!pak.ReadNamedData(pName, data))
				continue;

			//�����ļ���
			std::string sDir = GetDirFromPath(pName);
			if (!sDir.empty())
			{//�����ļ���
				StringReplaceA(sDir, "/", "\\");
				CreateMultipleDirectory(sDir.c_str());
			}
			//�����ļ�����
			FILE* f = fopen(pName, "wb+");
			if (f == NULL)
			{
				printf("д���ļ�ʧ��%s\n", pName);
				continue;
			}
			fwrite(data.data(), data.size(), 1, f);
			fclose(f);

			printf("%s\n", pName);
		}
	}

	pak.Close();
	
}