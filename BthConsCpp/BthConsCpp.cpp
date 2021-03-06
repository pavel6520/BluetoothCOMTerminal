﻿// BthConsCpp.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "GetPortNames.h"
#include <locale.h>

GetPortNames::CPortsArray ports;
GetPortNames::CNamesArray names;

int main()
{
	setlocale(LC_ALL, "russian");
	size_t portsCount = 0;
	std::string portName;
	std::cout << "Получение информации о COM портах..." << std::endl;
	if (GetPortNames::UpdatePortsList(&ports, &names, &portsCount)) {
		std::cout << "Error UpdatePortsList func (return true)" << std::endl;
		system("pause");
		return -1;
	}
	if (portsCount > 0) {
		std::cout << "\nАктивные COM порты:\n";
		for (size_t i = 0; i < portsCount; i++) {
			std::cout << i << ".    COM" << ports[i] << "\t" << S_WS_Convert::WideStringToAnsi(names[i]) << std::endl;
		}
	}
	else {
		std::cout << "Активные COM порты не найдены.\n\nПроверьте наличие активных портов в диспетчере устройств (имя порта указано в скобках).\nЕсли имя вашего порта отличается от \"COM<число>\", введите его вручную.\nУчтите, что программа требует ИСХОДЯЩИЙ порт (для каждого устройства создаются свои входящий и исходящий порты).\nЕсли на вашем компьютере программа \"не хочет видеть\" порт(ы), даже если они есть и вы ввели имя порта вручную, обратитесь к автору программы.\n\nВведите имя порта (с точностью до символа): ";
		std::cin >> portName;
	}

	system("pause");
	return 0;
}