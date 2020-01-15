# DLL动态链接库

## 实验背景

- 动态链接库（Dynamic Link Library，缩写为DLL）是一个可以被其它应用程序共享的程序模块，其中封装了一些可以被共享的例程和资源。动态链接库文件的扩展名一般是dll，也有可能是drv、sys和fon，它和可执行文件（exe）非常类似，区别在于DLL中虽然包含了可执行代码却不能单独执行，而应由Windows应用程序直接或间接调用。
- 动态链接库两种调用方式
  - 载入时动态链接Load-time Dynamic Linking
    - 这种用法的前提是在编译之前已经明确知道要调用DLL中的哪几个函数，编译时在目标文件中只保留必要的链接信息，而不含DLL函数的代码；当程序执行时，利用链接信息加载DLL函数代码并在内存中将其链接入调用程序的执行空间中，其主要目的是便于代码共享。模块非常明确调用某个导出函数，使得他们就像本地函数一样。这需要链接时链接那些函数所在DLL的导入库，导入库向系统提供了载入DLL时所需的信息及DLL函数定位。
  - 运行时动态链接Run-time Dynamic Linkin
    - 这种方式是指在编译之前并不知道将会调用哪些DLL函数，完全是在运行过程中根据需要决定应调用哪个函数，并用LoadLibrary和GetProcAddress动态获得DLL函数的入口地址。运行时可以通过LoadLibrary或LoadLibraryEx函数载入DLL。DLL载入后，模块可以通过调用 GetProcAddress获取DLL函数的出口地址，然后就可以通过返回的函数指针调用DLL函数了。如此即可避免导入库文件了。

## 实验过程

1. Load-time动态链接
    - 创建新项目，添加base.cpp作为dll的函数，模块文件exp.def把函数导出
      ```
      //base.cpp
      #include<Windows.h>

      int internal_function()
      {
        return 0;
      }

      int lib_function(char* msg)
      {
        // do some works
        MessageBoxA(0, "Message from base lib", msg, MB_OK);
        return 0;
      }
      ```

      ```
      //exp.def
      LIBRARY  baselib
      EXPORTS
      lib_function
      ```
    - 打开vs2017命令提示符，进入base.cpp所在的文件夹使用``cl.exe /c base.cpp``编译得到base.obj
    ![ ](image\load-time编译.JPG)
    - 使用``link base.obj User32.lib /dll /def:exp.def``得到baselip.dll和base.lib
    ![ ](image\load-time链接.JPG)
    - 建立一个base.h给接下来调用dll文件的程序作为头文件引用
    - 新建项目dll，添加源文件app.cpp
      ```
      //app.cpp
      #include "..\baselib\base.h"
      int main()
      {
        lib_function("call a dll");
        return 0;
      }
      ```
    - 将刚生成的baselib.dll放入app.cpp所在文件夹，并进入此文件夹对app.cpp进行编译链接(注意base.lib库在链接时的调用)，运行app.exe得到call a dll。
    ![ ](image\调用dll.JPG)
2. Run-time动态链接
    - 用老师提供的代码来完成实验
      ```
      #include <windows.h>
      #include <stdio.h>

      typedef int (__cdecl *MYPROC)(LPWSTR);

      int main( void )
      {
        HINSTANCE hinstLib;
        MYPROC ProcAdd;
        BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

        hinstLib = LoadLibrary(TEXT("baselib.dll"));

        if (hinstLib != NULL)
        {
          ProcAdd = (MYPROC) GetProcAddress(hinstLib, "lib_function");

          if (NULL != ProcAdd)
          {
              fRunTimeLinkSuccess = TRUE;
              (ProcAdd) (L"Message sent to the DLL function\n");
          }

          fFreeResult = FreeLibrary(hinstLib);
        }

        if (! fRunTimeLinkSuccess)
          printf("Message printed from executable\n");

      return 0;
      }
      ```
    - LoadLibrary函数调用的链接库文件改为baselib.dll  
    ![ ](image\run-time函数调用dll.JPG)

    - 然后对run.cpp编译链接，运行收到提示错误，
    ![ ](image\run-time编译链接成功运行失败.JPG)
    - 发现是GetProcess的参数没改，即调用的dll函数是错误的，修改为base.c中的函数lib_function
    ![ ](image\run-time函数调用dll2.JPG)
    - 重新编译链接，得到结果
    ![ ](image\run-time编译链接成功运行.JPG)