#pragma once

#if defined(GNARLY_SHARED)
#if defined(JSON11_EXPORT)
#define JASON11_API __declspec(dllexport)
#else
#define JASON11_API __declspec(dllimport)
#endif
#else
#define JASON11_API
#endif