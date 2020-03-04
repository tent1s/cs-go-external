#include "Memory.h"
#include <TlHelp32.h>
#include <comdef.h>

// pName - Название процесса, dwAccess - тип разрешения
// Смысл функции - найти поток и внедрится в него
bool Memory::search(const wchar_t* pName, DWORD dwAccess)
{
	// Получаем список запущенных процессов
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// Тип данных, который хранит процесс
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	// Ищем наш процесс по имени
	do
	{
		if (!wcscmp(pe32.szExeFile, pName))
		{
			_pId = pe32.th32ProcessID;						// Если находим, то запоминаем id процесса
			CloseHandle(handle);
			_process = OpenProcess(dwAccess, false, _pId);	// Внедряемся в наш найденный процесс
			return true;									// Говорим, что процесс найден
		}
	} while (Process32Next(handle, &pe32)); // Эта функция позволяет перебрать все запущенные процессы

	return false; // Сюда приходим в случае, если процесс не найден
}

// pModule - Название модуля
// Самая главная функция благодря ей находим нужный модуль и получаем адрес процесса и размер
MemModule Memory::getModule(const wchar_t* pModule)
{
	// Получаем список запущенных модулей
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, _pId);

	// Тип данных для модуля
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(me32);

	//Ищем наш модуль
	(Module32Next(handle, &me32));
	do
	{
		if (!wcscmp(me32.szModule, pModule))
		{
			CloseHandle(handle);
			// Если находим, возвращаем memModule
			return MemModule
			{
				reinterpret_cast<DWORD>(me32.hModule),
				me32.modBaseSize
			};
		}
	} while (Module32Next(handle, &me32));
	system("pause");
	return MemModule{ 0, 0 };
}

// Выходим из процесса
void Memory::exit()
{
	CloseHandle(_process);
}

