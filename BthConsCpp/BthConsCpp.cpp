// BthConsCpp.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "GetPortNames.h"
#include <iostream>
#include <string>

std::wstring _stdcall AnsiStringToWide(std::string const &Str, UINT CodePage = CP_ACP)
{
	DWORD const BuffSize = MultiByteToWideChar(CodePage, 0, Str.c_str(), -1, NULL, 0);
	if (!BuffSize) return NULL;
	std::vector<wchar_t> s;
	s.resize(BuffSize);
	if (!MultiByteToWideChar(CodePage, 0, Str.c_str(), -1, &s[0], BuffSize)) return NULL;
	std::wstring ws(s.begin(), s.end());
	return ws;
}

std::string _stdcall WideStringToAnsi(std::wstring const &Str, UINT CodePage = CP_ACP)
{
	int const BuffSize = WideCharToMultiByte(CodePage, 0, Str.c_str(), -1, NULL, 0, NULL, NULL);
	if (!BuffSize) return NULL;
	std::vector<char> ws;
	ws.resize(BuffSize);
	if (!WideCharToMultiByte(CodePage, 0, Str.c_str(), -1, &ws[0], BuffSize, NULL, NULL)) return NULL;
	std::string s(ws.begin(), ws.end());
	return s;
}

int main()
{
	size_t i = 0;
	GetPortNames::CPortsArray ports;
	GetPortNames::CNamesArray names;
#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1
	_tprintf(_T("Device Manager (SetupAPI - GUID_DEVINTERFACE_COMPORT) reports\n"));
	if (GetPortNames::UsingSetupAPI1(ports, names))
	{
#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
		for (i = 0; i<ports.size(); i++)
			_tprintf(_T("COM%u <%s>\n"), ports[i], names[i].c_str());
#else
		for (i = 0; i<ports.GetSize(); i++)
			_tprintf(_T("COM%u <%s>\n"), ports[i], names[i].GetString());
#endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
	}
	else
		_tprintf(_T("CEnumerateSerial::UsingSetupAPI1 failed, Error:%u\n"), GetLastError());
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI1

#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2
	_tprintf(_T("Device Manager (SetupAPI - Ports Device information set) reports\n"));
	if (GetPortNames::UsingSetupAPI2(ports, names))
	{
#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
		for (i = 0; i < ports.size(); i++) {
			std::string str = WideStringToAnsi(names[i]);
			std::cout << "COM" << ports[i] << " " << str << std::endl;
		}
#else
		for (i = 0; i < ports.GetSize(); i++) {
			string str = names[i];
			std::cout << "COM" << ports[i] << " " << str << std::endl;
		}
#endif //#ifndef CENUMERATESERIAL_MFC_EXTENSIONS
	}
	else
		_tprintf(_T("CEnumerateSerial::UsingSetupAPI2 failed, Error:%u\n"), GetLastError());
#endif //#ifndef NO_CENUMERATESERIAL_USING_SETUPAPI2
	system("pause");
}
