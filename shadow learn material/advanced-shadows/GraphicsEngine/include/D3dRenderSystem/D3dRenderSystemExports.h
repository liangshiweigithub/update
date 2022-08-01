#pragma once

#ifdef D3DRENDERSYSTEM_EXPORTS
#define D3DRENDERSYSTEM_API __declspec(dllexport)
#else
#define D3DRENDERSYSTEM_API __declspec(dllimport)
#endif

