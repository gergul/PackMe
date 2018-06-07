/*
 * ArgumentMan.h
 */
#pragma once
#include <map>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <assert.h>
#include <stdlib.h>

#ifdef _MSC_VER
#pragma  warning( push ) 
#pragma warning(disable:4996)
#endif

namespace AM
{
#define TEMPLATE template<class _Str/*=std::string*/, typename _Chr/*=char*/>

	//_Str = std::string || std::wstring, _Chr = char || wchar_t
	template<class _Str = std::string, typename _Chr = char>
	class CArgumentMan
	{
	public:
		CArgumentMan(void);
		virtual ~CArgumentMan(void);

	public:
		//添加命令标识符
		void AddCmdFlag(_Chr cCmdFlag);
		//设置是否多值 bValue==true:-key value1 value2
		void SetMultiValue(bool bValue);
		//设置是否参数标志忽略大小写
		void SetIgnoreCase(bool bValue);
		//解析命令行
		void Parse(int argc, _Chr **argv);
		//重置
		void Reset();

	public://没有参数名的参数值使用 ""(空字符串)作为key 去获取
		//获得参数
		const std::map<_Str, std::vector<_Str>> &GetArgs();
		std::vector<_Str> GetArgs(const _Str &sArgName);
		//获得参数(只获得一个参数)
		_Str GetArg(const _Str &sArgName);
		//判断是否有参数
		bool Has(std::vector<_Str> &vctOutArgs, const _Str &sArgName);
		bool Has(_Str &sOutArg, const _Str &sArgName);

		////////////////////////只能在_Str为std::wstring时可使用//////////////////////
		//获得参数
		std::map<std::string, std::vector<std::string>> ToAnsiArgs();
		//获得参数
		std::vector<std::string> GetAnsiArgs(const std::string &sArgName);
		//获得参数(只获得一个参数)
		std::string GetAnsiArg(const std::string &sArgName);
		//判断是否有参数
		bool HasAnsi(std::string &sOutArg, const std::string &sArgName);
		bool HasAnsi(std::vector<std::string> &vctOutArgs, const std::string &sArgName);
		//////////////////////////////////////////////////////////////////////////////
		////////////////////////只能在_Str为std::string 时可使用//////////////////////
		//获得参数
		std::map<std::wstring, std::vector<std::wstring>> ToUnicodeArgs();
		//获得参数
		std::vector<std::wstring> GetUnicodeArgs(const std::wstring &sArgName);
		//获得参数(只获得一个参数)
		std::wstring GetUnicodeArg(const std::wstring &sArgName);
		//判断是否有参数
		bool HasUnicode(std::wstring &sOutArg, const std::wstring &sArgName);
		bool HasUnicode(std::vector<std::wstring> &vctOutArgs, const std::wstring &sArgName);
		//////////////////////////////////////////////////////////////////////////////

	protected:
		template<class T>
		T& makeName(T& name);

	private:
		std::map<_Str, std::vector<_Str>> m_mapArgument;//命令集合
		std::set<_Chr> m_setCmdFlag;  //名字标志符
		bool m_bMultiValue; //一个参数标志后面是否允许多个值（例如：/F v1 v2 v3）
		bool m_bIgnoreCase; //忽略参数标志大小写
	};

	//////////////////////////////////////////////////////////////////////////////////////////////

	//unicode 转为 ansi
	bool WideByte2Ansi(std::string &out, const wchar_t* wstrcode)
	{
#if 0
		int ansiSize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode, -1, NULL, 0, NULL, NULL);
		if (ansiSize == ERROR_NO_UNICODE_TRANSLATION)
		{
			//throw std::exception("Invalid UTF-8 sequence.");
			return false;
		}
		if (ansiSize == 0)
		{
			//throw std::exception("Error in conversion.");
			return false;
		}

		out.resize(ansiSize, '\0');
		int convresult = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode, -1, const_cast<char *>(out.c_str()), ansiSize, NULL, NULL);
		if (convresult != ansiSize)
		{
			//throw std::exception("La falla!");
			return false;
		}
		out.resize(strlen(out.c_str()));
#else
		size_t nLen = wcslen(wstrcode) * 2;
		if (nLen == 0)
		{
			out.resize(0);
			return true;
		}

		setlocale(LC_ALL, "zh_CN");
		char* ar = new char[nLen + 1];
		memset(ar, 0, nLen + 1);
		size_t read = wcstombs(ar, wstrcode, nLen);
		out = ar;
		out.resize(read);
		delete[] ar;
		setlocale(LC_ALL, "");
#endif
		return true;
	}

	//ansi 转 Unicode
	bool Ansi2WideByte(std::wstring& out, const char* strAnsi)
	{
#if 0
		int widesize = ::MultiByteToWideChar(CP_ACP, 0, strAnsi, -1, NULL, 0);
		if (widesize == ERROR_NO_UNICODE_TRANSLATION)
		{
			//throw std::exception("Invalid UTF-8 sequence.");
			return false;
		}
		if (widesize == 0)
		{
			//throw std::exception("Error in conversion.");
			return false;
		}

		out.resize(widesize, '\0');
		int convresult = ::MultiByteToWideChar(CP_ACP, 0, strAnsi, -1, const_cast<wchar_t*>(out.c_str()), widesize);
		if (convresult != widesize)
		{
			//throw std::exception("La falla!");
			return false;
		}
		out.resize(wcslen(out.c_str()));
#else
		size_t nLen = strlen(strAnsi) * 2;
		if (nLen == 0)
		{
			out.resize(0);
			return true;
		}

		setlocale(LC_ALL, "zh_CN");
		wchar_t* ar = new wchar_t[nLen + 1];
		memset(ar, 0, nLen + 1);
		size_t read = mbstowcs(ar, strAnsi, nLen);
		out = ar;
		out.resize(read);
		delete[] ar;
		setlocale(LC_ALL, "");
#endif
		return true;
	}

	TEMPLATE
		CArgumentMan<_Str, _Chr>::CArgumentMan(void)
		: m_bMultiValue(false)
		, m_bIgnoreCase(false)
	{
	}

	TEMPLATE
		CArgumentMan<_Str, _Chr>::~CArgumentMan(void)
	{
	}

	TEMPLATE
		template<class T>
	T& AM::CArgumentMan<_Str, _Chr>::makeName(T& name)
	{
		if (m_bIgnoreCase)//是否区别大小写
		{
			transform(name.begin(), name.end(), name.begin(), towupper);//全部转换成大写
		}
		return name;
	}

	TEMPLATE
		void CArgumentMan<_Str, _Chr>::AddCmdFlag(_Chr cCmdFlag)
	{
		m_setCmdFlag.insert(cCmdFlag);
	}

	TEMPLATE
		void CArgumentMan<_Str, _Chr>::SetMultiValue(bool bValue)
	{
		m_bMultiValue = bValue;
	}

	TEMPLATE
		void CArgumentMan<_Str, _Chr>::SetIgnoreCase(bool bValue)
	{
		m_bIgnoreCase = bValue;
	}

	TEMPLATE
		void CArgumentMan<_Str, _Chr>::Parse(int argc, _Chr **argv)
	{
		std::set<_Chr>::iterator itSetEnd = m_setCmdFlag.end();
		std::map<_Str, std::vector<_Str>>::iterator itMapEnd = m_mapArgument.end();

		_Str sLocalCmd;
		for (int i = 0; i < argc; ++i)
		{
			_Str sCmd = argv[i];
			if (m_setCmdFlag.find(sCmd[0]) != itSetEnd)
			{//找到命令标志  
				sCmd.erase(sCmd.begin());
				makeName<_Str>(sCmd);

				std::map<_Str, std::vector<_Str>>::iterator itMapFinder = m_mapArgument.find(sCmd);
				if (itMapFinder == itMapEnd)
				{
					m_mapArgument.insert(std::make_pair(sCmd, std::vector<_Str>()));
				}

				sLocalCmd = sCmd;
			}
			else
			{
				std::map<_Str, std::vector<_Str>>::iterator itMapFinder = m_mapArgument.find(sLocalCmd);
				if (itMapFinder == itMapEnd)
				{
					m_mapArgument.insert(std::make_pair(sLocalCmd, std::vector<_Str>()));
					itMapFinder = m_mapArgument.find(sLocalCmd);
				}
				itMapFinder->second.push_back(sCmd);

				if (!m_bMultiValue)//是否多值
				{
					sLocalCmd.resize(0);
				}
			}
		}

	}

	TEMPLATE
		void CArgumentMan<_Str, _Chr>::Reset()
	{
		m_setCmdFlag.clear();
		m_mapArgument.clear();
	}

	TEMPLATE
		std::vector<_Str> CArgumentMan<_Str, _Chr>::GetArgs(const _Str &sArgName)
	{
		_Str sArgNameUpper = sArgName;
		makeName<_Str>(sArgNameUpper);

		std::map<_Str, std::vector<_Str>>::iterator itMapFinder
			= m_mapArgument.find(sArgNameUpper);
		if (itMapFinder != m_mapArgument.end())
		{
			return itMapFinder->second;
		}

		return std::vector<_Str>();
	}

	TEMPLATE
		const std::map<_Str, std::vector<_Str>> & CArgumentMan<_Str, _Chr>::GetArgs()
	{
		return m_mapArgument;
	}

	TEMPLATE
		_Str CArgumentMan<_Str, _Chr>::GetArg(const _Str &sArgName)
	{
		_Str sArgNameUpper = sArgName;
		makeName<_Str>(sArgNameUpper);

		std::map<_Str, std::vector<_Str>>::iterator itMapFinder
			= m_mapArgument.find(sArgNameUpper);
		if (itMapFinder != m_mapArgument.end())
		{
			return (itMapFinder->second).at(0);
		}

		return _Str();
	}

	TEMPLATE
		bool CArgumentMan<_Str, _Chr>::Has(std::vector<_Str> &vctOutArgs, const _Str &sArgName)
	{
		_Str sArgNameUpper = sArgName;
		makeName<_Str>(sArgNameUpper);

		std::map<_Str, std::vector<_Str>>::iterator itMapFinder
			= m_mapArgument.find(sArgNameUpper);
		if (itMapFinder != m_mapArgument.end())
		{
			vctOutArgs = itMapFinder->second;
			return true;
		}

		return false;
	}

	TEMPLATE
		bool CArgumentMan<_Str, _Chr>::Has(_Str &sOutArg, const _Str &sArgName)
	{
		_Str sArgNameUpper = sArgName;
		makeName<_Str>(sArgNameUpper);

		std::map<_Str, std::vector<_Str>>::iterator itMapFinder
			= m_mapArgument.find(sArgNameUpper);
		if (itMapFinder != m_mapArgument.end())
		{
			if (itMapFinder->second.empty())
			{
				sOutArg.resize(0);
			}
			else
			{
				sOutArg = itMapFinder->second.at(0);
			}
			return true;
		}

		return false;
	}

	TEMPLATE
		std::map<std::string, std::vector<std::string>> AM::CArgumentMan<_Str, _Chr>::ToAnsiArgs()
	{
		int nFromSize = sizeof(_Str().c_str()[0]);
		int nToSize = sizeof(std::string().c_str()[0]);
		assert(nFromSize > nToSize);

		std::map< std::string, std::vector<std::string> > ret;
		for (std::map< _Str, std::vector<_Str> >::iterator it = m_mapArgument.begin();
			it != m_mapArgument.end(); ++it)
		{
			std::string key; std::vector<std::string> args;
			WideByte2Ansi(key, it->first.c_str());

			std::vector<_Str>& _args = it->second;
			size_t nArgs = _args.size();
			for (size_t i = 0; i < nArgs; ++i)
			{
				std::string arg;
				WideByte2Ansi(arg, _args[i].c_str());
				args.push_back(arg);
			}

			ret.insert(std::make_pair(key, args));
		}

		return ret;
	}

	TEMPLATE
		std::map<std::wstring, std::vector<std::wstring>> AM::CArgumentMan<_Str, _Chr>::ToUnicodeArgs()
	{
		int nFromSize = sizeof(_Str().c_str()[0]);
		int nToSize = sizeof(std::wstring().c_str()[0]);
		assert(nFromSize < nToSize);

		std::map< std::wstring, std::vector<std::wstring> > ret;
		for (std::map< _Str, std::vector<_Str> >::iterator it = m_mapArgument.begin();
			it != m_mapArgument.end(); ++it)
		{
			std::wstring key; std::vector<std::wstring> args;
			Ansi2WideByte(key, it->first.c_str());

			std::vector<_Str>& _args = it->second;
			size_t nArgs = _args.size();
			for (size_t i = 0; i < nArgs; ++i)
			{
				std::wstring arg;
				Ansi2WideByte(arg, _args[i].c_str());
				args.push_back(arg);
			}

			ret.insert(std::make_pair(key, args));
		}

		return ret;
	}

	TEMPLATE
		std::vector<std::string> AM::CArgumentMan<_Str, _Chr>::GetAnsiArgs(const std::string &sArgName)
	{
		std::string name = sArgName;
		makeName<std::string>(name);

		std::map< std::string, std::vector<std::string> > argsMap = ToAnsiArgs();
		if (argsMap.find(name) != argsMap.end())
			return argsMap[name];
		return std::vector<std::string>();
	}

	TEMPLATE
		std::vector<std::wstring> AM::CArgumentMan<_Str, _Chr>::GetUnicodeArgs(const std::wstring &sArgName)
	{
		std::wstring name = sArgName;
		makeName<std::wstring>(name);

		std::map< std::wstring, std::vector<std::wstring> > argsMap = ToUnicodeArgs();
		if (argsMap.find(name) != argsMap.end())
			return argsMap[name];
		return std::vector<std::wstring>();
	}

	TEMPLATE
		std::string AM::CArgumentMan<_Str, _Chr>::GetAnsiArg(const std::string &sArgName)
	{
		std::string name = sArgName;
		makeName<std::string>(name);

		std::map< std::string, std::vector<std::string> > argsMap = ToAnsiArgs();
		if (argsMap.find(name) != argsMap.end())
		{
			std::vector<std::string>& args = argsMap[name];
			if (args.size() > 0)
				return args[0];
		}
		return std::string();
	}

	TEMPLATE
		std::wstring AM::CArgumentMan<_Str, _Chr>::GetUnicodeArg(const std::wstring &sArgName)
	{
		std::wstring name = sArgName;
		makeName<std::wstring>(name);

		std::map< std::wstring, std::vector<std::wstring> > argsMap = ToUnicodeArgs();
		if (argsMap.find(name) != argsMap.end())
		{
			std::vector<std::wstring>& args = argsMap[name];
			if (args.size() > 0)
				return args[0];
		}
		return std::wstring();
	}

	TEMPLATE
		bool AM::CArgumentMan<_Str, _Chr>::HasAnsi(std::string &sOutArg, const std::string &sArgName)
	{
		std::string name = sArgName;
		makeName<std::string>(name);

		std::map< std::string, std::vector<std::string> > argsMap = ToAnsiArgs();
		if (argsMap.find(name) == argsMap.end())
		{
			return false;
		}

		std::vector<std::string>& _args = argsMap[name];
		if (_args.size() > 0)
			sOutArg = _args[0];

		return true;
	}

	TEMPLATE
		bool AM::CArgumentMan<_Str, _Chr>::HasAnsi(std::vector<std::string> &vctOutArgs, const std::string &sArgName)
	{
		std::string name = sArgName;
		makeName<std::string>(name);

		std::map< std::string, std::vector<std::string> > argsMap = ToAnsiArgs();
		if (argsMap.find(name) == argsMap.end())
		{
			return false;
		}

		std::vector<std::string>& _args = argsMap[name];
		size_t nSize = _args.size();
		for (size_t i = 0; i < nSize; ++i)
		{
			vctOutArgs.push_back(_args[i]);
		}

		return true;
	}

	TEMPLATE
		bool AM::CArgumentMan<_Str, _Chr>::HasUnicode(std::wstring &sOutArg, const std::wstring &sArgName)
	{
		std::wstring name = sArgName;
		makeName<std::wstring>(name);

		std::map< std::wstring, std::vector<std::wstring> > argsMap = ToUnicodeArgs();
		if (argsMap.find(name) == argsMap.end())
		{
			return false;
		}

		std::vector<std::wstring>& _args = argsMap[name];
		if (_args.size() > 0)
			sOutArg = _args[0];

		return true;
	}

	TEMPLATE
		bool AM::CArgumentMan<_Str, _Chr>::HasUnicode(std::vector<std::wstring> &vctOutArgs, const std::wstring &sArgName)
	{
		std::wstring name = sArgName;
		makeName<std::wstring>(name);

		std::map< std::wstring, std::vector<std::wstring> > argsMap = ToUnicodeArgs();
		if (argsMap.find(name) == argsMap.end())
		{
			return false;
		}

		std::vector<std::wstring>& _args = argsMap[name];
		size_t nSize = _args.size();
		for (size_t i = 0; i < nSize; ++i)
		{
			vctOutArgs.push_back(_args[i]);
		}

		return true;
	}

}

#ifdef _MSC_VER
#pragma  warning(  pop  ) 
#endif