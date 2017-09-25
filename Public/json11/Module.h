#pragma once

#if defined(GNARLY_SHARED)
#if defined(JSON11_EXPORT)
#define JSON11_API __declspec(dllexport)
#else
#define JSON11_API __declspec(dllimport)
#endif
#else
#define JSON11_API
#endif