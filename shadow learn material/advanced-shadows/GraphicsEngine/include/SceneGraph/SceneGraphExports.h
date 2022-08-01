#pragma once

#ifdef SCENEGRAPH_EXPORTS
#define SCENEGRAPH_API __declspec(dllexport)
#else
#define SCENEGRAPH_API __declspec(dllimport)
#endif

