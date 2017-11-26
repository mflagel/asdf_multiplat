#pragma once

///based on SDL2 begin_code.h
#undef ASDF_API

#ifdef _MSC_VER
    #ifdef ASDFM_DLL
        #ifdef ASDFM_EXPORTS
            #define ASDFM_API __declspec(dllexport)
        #else
            #define ASDFM_API __declspec(dllimport)
        #endif
    #else
        #define ASDFM_API
    #endif
#endif
