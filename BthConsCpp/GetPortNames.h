#pragma once

#ifndef _Return_type_success_
#define _Return_type_success_(expr)
#endif //#ifndef _Return_type_success_

#ifndef _In_
#define _In_
#endif //#ifndef _In_

#ifndef _Out_
#define _Out_
#endif //#ifndef _Out_

#ifndef _Inout_
#define _Inout_
#endif //#ifndef _Inout_


///////////////////////// Includes ////////////////////////////////////////////                      
/*
#ifndef __ATLBASE_H__
#pragma message("To avoid this message, please put atlbase.h in your pre compiled header (normally stdafx.h)")
#include <atlbase.h>
#endif //#ifndef __ATLBASE_H__
*//*
#ifndef _VECTOR_
#pragma message("To avoid this message, please put vector in your pre compiled header (normally stdafx.h)")
#include <vector>
#endif //#ifndef _VECTOR_
*//*
#ifndef _STRING_
#pragma message("To avoid this message, please put string in your pre compiled header (normally stdafx.h)")
#include <string>
#endif //#ifndef _STRING_
*/
/*#ifdef _AFX
#ifndef __AFXTEMPL_H__
#pragma message("To avoid this message, please put afxtempl.h in your pre compiled header (normally stdafx.h)")
#include <afxtempl.h> 
#endif //#ifndef __AFXTEMPL_H__
#else
#ifndef __ATLSTR_H__
#pragma message("To avoid this message, please put atlstr.h in your pre compiled header (normally stdafx.h)")
#include <atlstr.h>
#endif //#ifndef __ATLSTR_H__
#endif //#ifdef _AFX*/


///////////////////////// Classes /////////////////////////////////////////////

class GetPortNames
{
public:
	//Typdefs
#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
	typedef std::vector<UINT> CPortsArray;
#ifdef _UNICODE
	typedef std::vector<std::wstring> CNamesArray;
#else
	typedef std::vector<std::string> CNamesArray;
#endif
#elif defined _AFX
	typedef CUIntArray CPortsArray;
	typedef CStringArray CNamesArray;
#else
	typedef CSimpleArray<UINT> CPortsArray;
	typedef CSimpleArray<ATL::CString> CNamesArray;
#endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS

	//Methods
#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1
	static _Return_type_success_(return != 0) BOOL UsingSetupAPI1(_Inout_ CPortsArray& ports, _Inout_ CNamesArray& friendlyNames);
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1

#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2
	static _Return_type_success_(return != 0) BOOL UsingSetupAPI2(_Inout_ CPortsArray& ports, _Inout_ CNamesArray& friendlyNames);
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2

	static bool UpdatePortsList(GetPortNames::CPortsArray *ports, GetPortNames::CNamesArray *names, size_t *i);
	//static std::string GetPortName(GetPortNames::CNamesArray *names, size_t val); //Причина удаления описана в реализации.

protected:
	//Methods
#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2)
	static _Return_type_success_(return != 0) BOOL RegQueryValueString(_In_ ATL::CRegKey& key, _In_ LPCTSTR lpValueName, _Out_ LPTSTR& pszValue);
	static _Return_type_success_(return != 0) BOOL QueryRegistryPortName(_In_ ATL::CRegKey& deviceKey, _Out_ int& nPort);
	static _Return_type_success_(return != 0) BOOL QueryUsingSetupAPI(const GUID& guid, _In_ DWORD dwFlags, _Inout_ CPortsArray& ports, _Inout_ CNamesArray& friendlyNames);
	static _Return_type_success_(return != 0) BOOL QueryDeviceDescription(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA& devInfo, ATL::CHeapPtr<BYTE>& byFriendlyName);
#endif //#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2)
	static _Return_type_success_(return != 0) BOOL IsNumeric(_In_ LPCSTR pszString, _In_ BOOL bIgnoreColon);
	static _Return_type_success_(return != 0) BOOL IsNumeric(_In_ LPCWSTR pszString, _In_ BOOL bIgnoreColon);
};
