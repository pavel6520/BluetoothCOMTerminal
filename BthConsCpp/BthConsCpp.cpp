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

int main()
{
	setlocale(LC_ALL, "russian");
	size_t portsCount = 0;
	TCHAR* pcCommPort;
	std::cout << "Получение информации о COM портах..." << std::endl;
	if (GetPortNames::UpdatePortsList(&ports, &names, &portsCount)) { //Получение номеров и имен портов из системы методами из Enumser - Device Manager.
		std::cout << "Error UpdatePortsList func (return true)" << std::endl;
		system("pause");
		return -1;
	}
	if (portsCount > 0) {
		std::cout << "\nАктивные COM порты:\n";
		for (size_t i = 0; i < portsCount; i++) {
			std::cout << i << ".    COM" << ports[i] << "\t" << S_WS_Convert::WideStringToAnsi(names[i]) << std::endl;
		}
		std::cout << "Введите номер нужного COM порта (число слева): ";
		while (true) {
			int num;
			std::cin >> num;
			clearCin();
			if (num >= 0 && num < (int)portsCount) {
				char sC[3] = "";
				int numP = ports[num], i = 2;
				while (numP / ((int)pow(10, i)) % 10 == 0) i--;
				while (numP) {
					sC[i--] = numP % 10 + '0';
					//std::cout << numP << " " << numP % 10 << std::endl;
					numP /= 10;
				}
				std::cout << std::endl << "Выбран порт ";
				std::string s = "COM";
				s.append(sC);
				pcCommPort = new TCHAR[s.size()];
				std::copy(s.begin(), s.end(), pcCommPort);
				printf("%ws", pcCommPort);
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