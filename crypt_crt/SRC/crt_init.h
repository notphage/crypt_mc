#pragma once
#include <windows.h>

//
// Function prototypes which will be used for our constructor
// table, as well as the atexit function.
//
using ConstructorFn = void(__cdecl*)();
using DestructorFn  = void(__cdecl*)();

// Our constructor table.
extern ConstructorFn __xc_a[], __xc_z[];

//
// The following functions are responsible for housekeeping;
// calling the constructors and destructors for objects that
// are not explicitly initialized inside of a function, but
// rather initialized in the global namespace.
//
void __cdecl _initterm(ConstructorFn* first, ConstructorFn* last);

void __cdecl _atexit_init();
void __cdecl _DoExit();