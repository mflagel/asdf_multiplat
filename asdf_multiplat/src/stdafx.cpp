// stdafx.cpp : source file that includes just the standard includes
// AsdfMultiplat.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file



void asdf_fail(char const* condition, char const* file, int line, ...) {
    LOG("ASSERT FAILED: %s, %s, %i", condition, file, line)
}