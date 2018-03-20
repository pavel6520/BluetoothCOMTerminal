#pragma once
class Convert
{
public:
	Convert();
	static std::wstring _stdcall AnsiStringToWide(std::string &Str);
	static std::string _stdcall WideStringToAnsi(std::wstring &Str);
	//static int _stdcall CharToLPCSTR(LPCSTR &portName, int portNum); //невозможно использовать из за типа LPCSTR
	~Convert();
};
