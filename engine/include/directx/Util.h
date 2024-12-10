#pragma once

template <typename T>
void SafeRelease(T** ptr)
{
    if (*ptr)
    {
        (*ptr)->Release();
        *ptr = nullptr;
    }
}