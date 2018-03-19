#pragma once
class S_WS_Convert
{
public:
	S_WS_Convert();
	static std::wstring _stdcall AnsiStringToWide(std::string &Str);
	static std::string _stdcall WideStringToAnsi(std::wstring &Str);
	~S_WS_Convert();
};

