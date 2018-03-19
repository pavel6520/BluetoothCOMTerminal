#include "stdafx.h"
#include "S_WS_Convert.h"


S_WS_Convert::S_WS_Convert()
{
}


S_WS_Convert::~S_WS_Convert()
{
}

std::wstring _stdcall S_WS_Convert::AnsiStringToWide(std::string &Str) //Конвертация из std::string в std::wstring.
{
	DWORD const BuffSize = MultiByteToWideChar(CP_ACP, 0, Str.c_str(), -1, NULL, 0);
	if (!BuffSize) return NULL;
	std::vector<wchar_t> s;
	s.resize(BuffSize);
	if (!MultiByteToWideChar(CP_ACP, 0, Str.c_str(), -1, &s[0], BuffSize)) return NULL;
	std::wstring ws(s.begin(), s.end());
	return ws;
}

std::string _stdcall S_WS_Convert::WideStringToAnsi(std::wstring &Str) //Конвертация из std::wstring в std::string.
{
	int const BuffSize = WideCharToMultiByte(CP_ACP, 0, Str.c_str(), -1, NULL, 0, NULL, NULL);
	if (!BuffSize) return NULL;
	std::vector<char> ws;
	ws.resize(BuffSize);
	if (!WideCharToMultiByte(CP_ACP, 0, Str.c_str(), -1, &ws[0], BuffSize, NULL, NULL)) return NULL;
	std::string s(ws.begin(), ws.end());
	return s;
}