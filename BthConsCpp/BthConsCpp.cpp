// BthConsCpp.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "GetPortNames.h"
#include <locale.h>

void clearCin() {
	std::cin.clear();
	std::ios_base::sync_with_stdio(0);
	std::cin.ignore(std::cin.rdbuf()->in_avail());
}

GetPortNames::CPortsArray ports; //Массив номеров COM портов в системе ("COM%d", ports[0]).
GetPortNames::CNamesArray names; //Имена устройств, присвоеные системой. Использовать S_WS_Convert::WideStringToAnsi(names[i]) для получения строки std::string.

LPCSTR COMportName;

int main()
{
	//HANDLE hSerial;
	DCB dcbSerialParams = { 0 };
	setlocale(LC_ALL, "russian");
	size_t portsCount = 0;
	
	std::cout << "Получение информации о COM портах..." << std::endl;
	if (GetPortNames::UpdatePortsList(&ports, &names, &portsCount)) { //Получение номеров и имен портов из системы методами из Enumser - Device Manager.
		std::cout << "Error UpdatePortsList func (return true)" << std::endl;
		system("pause");
		return -1;
	}
	if (portsCount > 0) {
		std::cout << "\nАктивные COM порты:\n";
		for (size_t i = 0; i < portsCount; i++) {
			std::cout << i << ".    COM" << ports[i] << "\t" /*<< S_WS_Convert::WideStringToAnsi(names[i])*/ << std::endl;
		}
		std::cout << "Введите номер нужного COM порта (число слева): ";
		while (true) {
			int num;
			std::cin >> num;
			clearCin();
			if (num >= 0 && num < (int)portsCount) {
				char sC[] = {'C', 'O', 'M', '\0', '\0', '\0', '\0'};
				int numP = ports[num], i = 2;
				while (numP / ((int)pow(10, i)) % 10 == 0) i--;
				while (numP) {
					*(sC + 3 + i--) = (char) (numP % 10 + '0');
					numP /= 10;
				}
				COMportName = sC;
				HANDLE hSerial = CreateFileA(COMportName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				std::cout << hSerial << " " << COMportName << " " << GetLastError() << std::endl;
				SecureZeroMemory(&dcbSerialParams, sizeof(DCB));
				dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
				//dcbSerialParams.fOutxCtsFlow = 0;
				//dcbSerialParams.fOutxDsrFlow = 0;
				if (!GetCommState(hSerial, &dcbSerialParams))
				{
					std::cout << "getting state error " << GetLastError() << std::endl;
				}
				dcbSerialParams.BaudRate = CBR_9600;
				dcbSerialParams.ByteSize = 8;
				dcbSerialParams.StopBits = ONESTOPBIT;
				dcbSerialParams.Parity = NOPARITY;
				if (!SetCommState(hSerial, &dcbSerialParams))
				{
					std::cout << "error setting serial port state\n";
				}
				char data[] = "m0007001000";  // строка для передачи
				DWORD dwSize = sizeof(data);   // размер этой строки
				DWORD dwBytesWritten;    // тут будет количество собственно переданных байт
				BOOL iRet = WriteFile(hSerial, data, dwSize, &dwBytesWritten, NULL);
				std::cout << dwSize << " Bytes in string. " << dwBytesWritten << " Bytes sended. " << GetLastError() << std::endl;

				std::cout << std::endl << std::endl;
				break;
			}
			else {
				std::cout << "Некорректный номер порта, введите число от 0 до " << portsCount - 1 << ": ";
			}
		}
	}
	else {
		std::cout << "\nАктивные COM порты не найдены.\nПроверьте наличие активных портов в диспетчере устройств (имя порта указано в скобках).\nЕсли имя нужного вам порта отличается от COM<число>, обратитесь к автору программы.\n\n";
		system("pause");
		return 0;
	}

	system("pause");
	return 0;
}