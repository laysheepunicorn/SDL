#include <Windows.h>
#include <stdio.h>
#include <versionhelpers.h>
#include "fheaders.h"
#include "auxiliary.h"
#pragma warning(disable:4996)


DWORD wmain(int argc, wchar_t* argv[])
{
	DWORD dwProcessId = 0;

	dwProcessId = findPidByName("notepad.exe");
	demoCreateRemoteThreadW(L"D:\\����\\SDL\\����ʵ��\\DLLע��\\injectAllTheThings\\Debug\\baselib.dll", dwProcessId);
	return(0);
}

