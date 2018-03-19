#include "stdafx.h"
#include "GetPortNames.h"

/////////////////////////////// Macros / Defines //////////////////////////////

#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2)
#include <winioctl.h>

#ifndef _INC_SETUPAPI
#pragma message("To avoid this message, please put setupapi.h in your pre compiled header (normally stdafx.h)")
#include <setupapi.h>
#endif //#ifndef _INC_SETUPAPI

#pragma comment(lib, "setupapi.lib")
#endif //#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2)

#ifndef _WINSPOOL_
#pragma message("To avoid this message, please put winspool.h in your pre compiled header (normally stdafx.h)")
#include <winspool.h>
#endif //#ifndef _WINSPOOL_

#pragma comment(lib, "winspool.lib")

#pragma comment(lib, "advapi32.lib")

#ifndef __IWbemLocator_FWD_DEFINED__
#pragma message("To avoid this message, please put WBemCli.h in your pre compiled header (normally stdafx.h)")
#include <WbemCli.h>
#endif //#ifndef __IWbemLocator_FWD_DEFINED__

#ifndef _INC_COMDEF
#pragma message("To avoid this message, please put comdef.h in your pre compiled header (normally stdafx.h)")
#include <comdef.h>
#endif //#ifndef _INC_COMDEF

#pragma comment(lib, "WbemUuid.lib")

#ifndef _MSPORTS_H
#pragma message("To avoid this message, please put msports.h in your pre compiled header (normally stdafx.h)")
#include <msports.h>
#endif //#ifndef _MSPORTS_H

#pragma comment(lib, "msports.lib")

__if_not_exists(LSTATUS)
{
	typedef _Return_type_success_(return == ERROR_SUCCESS) LONG LSTATUS;
}


///////////////////////////// Implementation //////////////////////////////////

#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2)
_Return_type_success_(return != 0) BOOL GetPortNames::RegQueryValueString(_In_ ATL::CRegKey& key, _In_ LPCTSTR lpValueName, _Out_ LPTSTR& pszValue)
{
	//Initialize the output parameter
	pszValue = nullptr;

	//First query for the size of the registry value 
	ULONG nChars = 0;
	LSTATUS nStatus = key.QueryStringValue(lpValueName, nullptr, &nChars);
	if (nStatus != ERROR_SUCCESS)
	{
		SetLastError((DWORD)nStatus);
		return FALSE;
	}

	//Allocate enough bytes for the return value
	DWORD dwAllocatedSize = ((nChars + 1) * sizeof(TCHAR)); //+1 is to allow us to null terminate the data if required
	pszValue = reinterpret_cast<LPTSTR>(LocalAlloc(LMEM_FIXED, dwAllocatedSize));
	if (pszValue == nullptr)
		return FALSE;

	//We will use RegQueryValueEx directly here because ATL::CRegKey::QueryStringValue does not handle non-null terminated data
	DWORD dwType = 0;
	ULONG nBytes = dwAllocatedSize;
	pszValue[0] = _T('\0');
	nStatus = RegQueryValueEx(key, lpValueName, nullptr, &dwType, reinterpret_cast<LPBYTE>(pszValue), &nBytes);
	if (nStatus != ERROR_SUCCESS)
	{
		LocalFree(pszValue);
		pszValue = nullptr;
		SetLastError((DWORD)nStatus);
		return FALSE;
	}
	if ((dwType != REG_SZ) && (dwType != REG_EXPAND_SZ))
	{
		LocalFree(pszValue);
		pszValue = nullptr;
		SetLastError(ERROR_INVALID_DATA);
		return FALSE;
	}
	if ((nBytes % sizeof(TCHAR)) != 0)
	{
		LocalFree(pszValue);
		pszValue = nullptr;
		SetLastError(ERROR_INVALID_DATA);
		return FALSE;
	}
	if (pszValue[(nBytes / sizeof(TCHAR)) - 1] != _T('\0'))
	{
		//Forcibly null terminate the data ourselves
		pszValue[(nBytes / sizeof(TCHAR))] = _T('\0');
	}

	return TRUE;
}

_Return_type_success_(return != 0) BOOL GetPortNames::QueryRegistryPortName(_In_ ATL::CRegKey& deviceKey, _Out_ int& nPort)
{
	//What will be the return value from the method (assume the worst)
	BOOL bAdded = FALSE;

	//Read in the name of the port
	LPTSTR pszPortName = nullptr;
	if (RegQueryValueString(deviceKey, _T("PortName"), pszPortName))
	{
		//If it looks like "COMX" then
		//add it to the array which will be returned
		size_t nLen = _tcslen(pszPortName);
		if (nLen > 3)
		{
			if ((_tcsnicmp(pszPortName, _T("COM"), 3) == 0) && IsNumeric((pszPortName + 3), FALSE))
			{
				//Work out the port number
				nPort = _ttoi(pszPortName + 3);

				bAdded = TRUE;
			}
		}
		LocalFree(pszPortName);
	}

	return bAdded;
}

_Return_type_success_(return != 0) BOOL GetPortNames::QueryUsingSetupAPI(const GUID& guid, _In_ DWORD dwFlags, _Inout_ CPortsArray& ports, _Inout_ CNamesArray& friendlyNames)
{
	//Set our output parameters to sane defaults
#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
	ports.clear();
	friendlyNames.clear();
#else
	ports.RemoveAll();
	friendlyNames.RemoveAll();
#endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS

	//Create a "device information set" for the specified GUID
	HDEVINFO hDevInfoSet = SetupDiGetClassDevs(&guid, nullptr, nullptr, dwFlags);
	if (hDevInfoSet == INVALID_HANDLE_VALUE)
		return FALSE;

	//Finally do the enumeration
	BOOL bMoreItems = TRUE;
	int nIndex = 0;
	SP_DEVINFO_DATA devInfo;
	while (bMoreItems)
	{
		//Enumerate the current device
		devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
		bMoreItems = SetupDiEnumDeviceInfo(hDevInfoSet, nIndex, &devInfo);
		if (bMoreItems)
		{
			//Did we find a serial port for this device
			BOOL bAdded = FALSE;

			//Get the registry key which stores the ports settings
			ATL::CRegKey deviceKey;
			deviceKey.Attach(SetupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE));
			if (deviceKey != INVALID_HANDLE_VALUE)
			{
				int nPort = 0;
				if (QueryRegistryPortName(deviceKey, nPort))
				{
#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
					ports.push_back((UINT)nPort);
#else
					ports.Add(nPort);
#endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
					bAdded = TRUE;
				}
			}

			//If the port was a serial port, then also try to get its friendly name
			if (bAdded)
			{
				ATL::CHeapPtr<BYTE> byFriendlyName;
				if (QueryDeviceDescription(hDevInfoSet, devInfo, byFriendlyName))
				{
#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
					friendlyNames.push_back(reinterpret_cast<LPCTSTR>(byFriendlyName.m_pData));
#else
					friendlyNames.Add(reinterpret_cast<LPCTSTR>(byFriendlyName.m_pData));
#endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
				}
				else
				{
#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
					friendlyNames.push_back(_T(""));
#else
					friendlyNames.Add(_T(""));
#endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
				}
			}
		}

		++nIndex;
	}

	//Free up the "device information set" now that we are finished with it
	SetupDiDestroyDeviceInfoList(hDevInfoSet);

	//Return the success indicator
	return TRUE;
}

_Return_type_success_(return != 0) BOOL GetPortNames::QueryDeviceDescription(HDEVINFO hDevInfoSet, SP_DEVINFO_DATA& devInfo, ATL::CHeapPtr<BYTE>& byFriendlyName)
{
	DWORD dwType = 0;
	DWORD dwSize = 0;
	//Query initially to get the buffer size required
	if (!SetupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, nullptr, 0, &dwSize))
	{
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return FALSE;
	}

#pragma warning(suppress: 6102)
	if (!byFriendlyName.Allocate(dwSize))
	{
		SetLastError(ERROR_OUTOFMEMORY);
		return FALSE;
	}

	return SetupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, byFriendlyName.m_pData, dwSize, &dwSize) && (dwType == REG_SZ);
}
#endif //#if !defined(NO_CENUMERATESERIAL_USING_SETUPAPI1) || !defined(NO_CENUMERATESERIAL_USING_SETUPAPI2)

_Return_type_success_(return != 0) BOOL GetPortNames::IsNumeric(_In_ LPCSTR pszString, _In_ BOOL bIgnoreColon)
{
	size_t nLen = strlen(pszString);
	if (nLen == 0)
		return FALSE;

	//What will be the return value from this function (assume the best)
	BOOL bNumeric = TRUE;

	for (size_t i = 0; i<nLen && bNumeric; i++)
	{
		bNumeric = (isdigit(static_cast<int>(pszString[i])) != 0);
		if (bIgnoreColon && (pszString[i] == ':'))
			bNumeric = TRUE;
	}

	return bNumeric;
}

_Return_type_success_(return != 0) BOOL GetPortNames::IsNumeric(_In_ LPCWSTR pszString, _In_ BOOL bIgnoreColon)
{
	size_t nLen = wcslen(pszString);
	if (nLen == 0)
		return FALSE;

	//What will be the return value from this function (assume the best)
	BOOL bNumeric = TRUE;

	for (size_t i = 0; i<nLen && bNumeric; i++)
	{
		bNumeric = (iswdigit(pszString[i]) != 0);
		if (bIgnoreColon && (pszString[i] == L':'))
			bNumeric = TRUE;
	}

	return bNumeric;
}

#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1
_Return_type_success_(return != 0) BOOL GetPortNames::UsingSetupAPI1(_Inout_ CPortsArray& ports, _Inout_ CNamesArray& friendlyNames)
{
	//Delegate the main work of this method to the helper method
	return QueryUsingSetupAPI(GUID_DEVINTERFACE_COMPORT, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE, ports, friendlyNames);
}
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1

#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2
_Return_type_success_(return != 0) BOOL GetPortNames::UsingSetupAPI2(_Inout_ CPortsArray& ports, _Inout_ CNamesArray& friendlyNames)
{
	//Delegate the main work of this method to the helper method
	return QueryUsingSetupAPI(GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR, DIGCF_PRESENT, ports, friendlyNames);
}
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2

bool GetPortNames::UpdatePortsList(GetPortNames::CPortsArray *ports, GetPortNames::CNamesArray *names, size_t *i) {
#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1
	if (GetPortNames::UsingSetupAPI1(*ports, *names)){
#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
		*i = (*ports).size();
#else
		*i = (*ports).GetSize();
#endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
		return false;
	}
	//else
#else
#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2
	if (GetPortNames::UsingSetupAPI2(ports, names)) {
#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
		*i = (*ports).size();
#else
		*i = (*ports).GetSize();
#endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
		return false;
	}
	//else
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2
#endif
	return true;
}

/*std::string GetPortNames::GetPortName(GetPortNames::CNamesArray *names, size_t val) {
#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
	return S_WS_Convert::WideStringToAnsi((*names)[val]);
	//При обращении напрямую по указателю происходит исключение при работе функции WideCharToMultiByte:
	//Вызвано исключение по адресу 0x742A586C (KernelBase.dll) в BthConsCpp.exe: 0xC0000005: нарушение прав доступа при чтении по адресу 0x2449C852.
	//Буду признателен, если кто нибуть найдет выход из этой ситуации.
#else
	for (i = 0; i<ports.GetSize(); i++)
		_tprintf(_T("COM%u <%s>\n"), ports[i], names[i].GetString());
#endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
}*/
