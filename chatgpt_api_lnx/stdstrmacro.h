#pragma once

//https://github.com/microsoft/cpprestsdk/issues/1168
#if defined _WIN32 || defined _WIN64
#define TO_STD_STR(ws_str)     utility::conversions::to_utf8string(ws_str)
#define FROM_STD_STR(utf8str)  utility::conversions::to_string_t(utf8str)
#else
#define TO_STD_STR(ws_str)     ws_str
#define FROM_STD_STR(utf8str)  utf8str
#endif

