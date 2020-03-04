#include <iostream>
#include "Memory.h"

#define oPlayer 0xD28B74
#define oHealth 0x100
#define oTeam	0xF4
#define oEntityList 0x4D3C7BC
#define oGlowIndex 0xA428
#define oGlowObjManager 0x527DFA0

Memory memory;
MemModule clientBase;

//спойлер для завтрашнего поста))
void wallhack()
{
	while (true) // создаем бесконечный цикл
	{
		Sleep(10); // таймаут 10 мс, чтобы не грузить процессор под 100

		DWORD glowObj = memory.read<DWORD>(clientBase.dwAddress + oGlowObjManager); // создаем объект glowObj из модельки игрока
		DWORD myTeam = memory.read<DWORD>(memory.read<DWORD>(clientBase.dwAddress + oPlayer) + oTeam); // создаем объект тиммейтов

		for (int x = 0; x < 32; x++) // сам вх
		{
			DWORD player = memory.read<DWORD>(clientBase.dwAddress + oEntityList + x * 0x10); // обычный игрок
			if (player == 0)
				continue;

			bool dormant = memory.read<bool>(player + 0xED); // спектатор
			if (dormant)
				continue;

			DWORD team = memory.read<DWORD>(player + oTeam); // тиммейт
			if (team != 2 && team != 3)
				continue;

			DWORD currentGlowIndex = memory.read<DWORD>(player + oGlowIndex); // текущий индекс игрока

			if (team != myTeam) // если игрок не тиммейт
			{
				// делаем его обводку красным
				memory.write<float>(glowObj + currentGlowIndex * 0x38 + 0x4, 255); // red
				memory.write<float>(glowObj + currentGlowIndex * 0x38 + 0x8, 0); // green
				memory.write<float>(glowObj + currentGlowIndex * 0x38 + 0xC, 0); // blue
				memory.write<float>(glowObj + currentGlowIndex * 0x38 + 0x10, 255);
				memory.write<bool>(glowObj + currentGlowIndex * 0x38 + 0x24, true);
				memory.write<bool>(glowObj + currentGlowIndex * 0x38 + 0x25, false);
			}
			else // если игрок тиммейт
			{
				// делаем его обводку синим
				memory.write<float>(glowObj + currentGlowIndex * 0x38 + 0x4, 0); // red
				memory.write<float>(glowObj + currentGlowIndex * 0x38 + 0x8, 0); // green
				memory.write<float>(glowObj + currentGlowIndex * 0x38 + 0xC, 255); // blue
				memory.write<float>(glowObj + currentGlowIndex * 0x38 + 0x10, 255);
				memory.write<bool>(glowObj + currentGlowIndex * 0x38 + 0x24, true);
				memory.write<bool>(glowObj + currentGlowIndex * 0x38 + 0x25, false);
			}
		}
	}
}

int main()
{
	//ищем процесс
	bool success = memory.search(L"csgo.exe", PROCESS_ALL_ACCESS);

	//если процесс не найден, завершаем работу
	if (!success) {
		std::cout << "process not found!";
		return 1;
	}

	clientBase = memory.getModule(L"client_panorama.dll"); // ищем клиент кс го

	std::cout << "clientBase " << clientBase.dwAddress << std::endl; //выводим адрес
	wallhack();

	memory.exit();
	system("pause");
	return 0;

}