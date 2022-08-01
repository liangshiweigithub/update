#pragma once

#ifdef PLUGINCOLLADA_EXPORTS
#define PLUGINCOLLADA_API __declspec(dllexport)
#else
#define PLUGINCOLLADA_API __declspec(dllimport)
#endif

