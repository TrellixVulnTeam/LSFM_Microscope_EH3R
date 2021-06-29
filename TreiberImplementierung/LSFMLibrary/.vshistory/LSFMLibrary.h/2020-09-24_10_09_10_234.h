#pragma once
#ifdef LSFMLIBRARY_EXPORTS

#define LSFMLIBRARY_API __declspec(dllexport)
#else
#define LSFMLIBRARY_API __declspec(dllimport)
#endif


extern "C" LSFMLIBRARY_API void Connect();

extern "C" LSFMLIBRARY_API void Disconnect();



