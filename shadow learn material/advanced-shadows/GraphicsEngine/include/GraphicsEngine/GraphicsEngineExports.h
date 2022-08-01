#pragma once

#ifdef GRAPHICSENGINE_EXPORTS
#define GRAPHICSENGINE_API __declspec(dllexport)
#else
#define GRAPHICSENGINE_API __declspec(dllimport)
#endif
