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
	std::string strDir(szPath);//存放要创建的目录字符串
	//确保以'\'结尾以创建最后一个目录
	if (strDir[strDir.length() - 1] != '\\' )
	{
		strDir += '\\';
	}
	std::vector<std::string> vPath;//存放每一层目录字符串
	std::string strTemp;//一个临时变量,存放目录字符串
	bool bSuccess = false;//成功标志
	//遍历要创建的字符串
	for (int i = 0; i < strDir.length(); ++i)
	{
		if (strDir[i] != '\\')
		{//如果当前字符不是'\\'
			strTemp += strDir[i];
		}
		else
		{//如果当前字符是'\\'
			vPath.push_back(strTemp);//将当前层的字符串添加到数组中
			strTemp += '\\';
		}
	}

	//遍历存放目录的数组,创建每层目录
	std::vector<std::string>::const_iterator vIter;
	for (vIter = vPath.begin(); vIter != vPath.end(); vIter++)
	{
		//如果CreateDirectory执行成功,返回true,否则返回false
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

size_t MyReadFile(const char * path, char* outContent, size_t & in_outLen, size_t start=0)
{
	FILE *f = NULL;
	long sz;
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

		if (fseek(f, start, SEEK_SET) < 0)
		{
			break;
		}

		size_t nToRead = in_outLen;
		if (nToRead == 0)
		{
			nToRead = (size_t)sz;
		}

		size_t nReaded = fread(outContent, 1, nToRead, f);
		if (nReaded > nToRead)
		{
			break;
		}

		in_outLen = nReaded;

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
		return;

	std::vector<std::string> vctFiles = am.GetArgs("i");
	std::vector<std::string> vctDirs = am.GetArgs("d");
	std::vector<std::string> vctDir;
	for (int i = 0; i < vctDirs.size(); ++i)
	{
		ListFilesA(vctDirs[i].c_str(), vctDir, vctFiles);
	}
	
	if (vctFiles.size() > 0)
		bRead = false;
	else
		bRead = true;

	try
	{
		PackMeNamed pak(sOutFile.c_str(), bRead);
		if (!bRead)
		{//write
			if (pak.CheckID())
			{
				printf("已经被打包过!\n");
				return;
			}

			//备份
			CopyFileA(sOutFile.c_str(), (sOutFile+".bak").c_str(), FALSE);

			char* fileContent = new char[1024 * 1024 * 10];
			for (int i = 0; i < vctFiles.size(); ++i)
			{
				const char* pFileName = vctFiles[i].c_str();

				pak.BeginBlock(pFileName);
				//文件名长度|文件名|文件内容
				//file name
				long fileNameLen = strlen(pFileName);
				pak.AppendBlockData((const char*)&fileNameLen, sizeof(fileNameLen));
				pak.AppendBlockData(pFileName, fileNameLen);
				//file content
				size_t len = 0;
				if (MyReadFile(pFileName, (char*)fileContent, len) == 0)
				{
					printf("读取文件失败%s\n", pFileName);
				}
				else
				{
					printf("%s\n", pFileName);
				}
				pak.AppendBlockData(fileContent, len);

				pak.EndBlock();
			}
			delete[] fileContent;
		}
		else
		{//read
			long nCount = pak.IndexCount();
			for (int i = 0; i < nCount; ++i)
			{
				//读全部内容
				long len = pak.GetDataLen(i);
				char* pData = new char[len];
				pak.ReadData(i, pData);
				char* p = pData;
				//分离文件名长度
				long fileNameLen = 0;
				p = PackMe::GetBaseTypeInData(p, fileNameLen);
				//分离文件名
				char* fileName = new char[fileNameLen + 1];
				memcpy(fileName, p, fileNameLen);
				fileName[fileNameLen] = '\0';
				p = p + fileNameLen;
				//分离文件夹
				std::string sDir = GetDirFromPath(fileName);
				if (!sDir.empty())
				{//创建文件夹
					StringReplaceA(sDir, "/", "\\");
					CreateMultipleDirectory(sDir.c_str());
				}
				//分离文件内容
				FILE* f = fopen(fileName, "wb+");
				if (f == NULL)
				{
					printf("写入文件失败%s\n", fileName);
					continue;
				}
				fwrite(p, len - sizeof(fileNameLen) - fileNameLen, 1, f);
				fclose(f);

				printf("%s\n", fileName);

				delete[] fileName;
				delete[] pData;
			}
		}

		pak.Close();
	}
	catch (const std::exception& )
	{

	}	
	
}