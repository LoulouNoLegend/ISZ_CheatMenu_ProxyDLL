# Simple DLL Template

This is a basic starting point for creating Windows Dynamic Link Libraries (DLLs). This template a simple exemple of how to structure a DLL and has a basic implementation of the `DllMain` function: the entry point for a DLL.

*This was made as a test while working on [DL-Injector](https://github.com/LoulouNoLegend/LD-Injector)*

## Overview
This repository only contains a minimal DLL project for Windows that, when loaded into a process, will display a message box saying "Hello from the DLL!". This templates only serves as a foundation that can be used to do more complex DLL projects.

## Code Explanation
Here's the provided code in the `dllmain.cpp` file:
```cpp
#include "pch.h"
#include <Windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        MessageBox(NULL, L"Hello from the DLL!", L"Injected DLL", MB_OK);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
```

### Key Components
- **pch.h Library:** Includes the precompiled header files to speed up compilation by compiling common headers into a single file.
- **Windows.h Library:** Includes the Windows API header file that provides declararions for the Windows API functions, macros and types.

- **`DllMain` Function:** This is the entry point for the DLL. It's called by the system when loaded or unloaded.
- **`DLL_PROCESS_ATTACH`:** Triggered when the DLL is loaded into the address space of the process. Here, a message box is displayed to confirm the DLL's injection.
- **`DLL_THREAD_ATTACH`:** Triggered when a new thread is created in a process that has already loaded the DLL.
- **`DLL_THREAD_DETACH`:** Triggered when a thread exits a program that has the DLL loaded in it.
- **`DLL_PROCESS_DETACH`:** Triggered when the DLL is unloaded from the address space of the process.

## How to Use
1. **Clone or Download:** Get the template from [Github](https://github.com/LoulouNoLegend/DLL-Template).
2. **Build the DLL:** Use a C++ compiler that supports Windows API functions to compile the code into a DLL.
3. **Inject the DLL:** Use a DLL injector *(such as [DL-Injector](https://github.com/LoulouNoLegend/LD-Injector))* or integrate it into a project to test it.