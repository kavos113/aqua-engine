#ifndef AQUA_DIRECTX_UTIL_H
#define AQUA_DIRECTX_UTIL_H

template <typename T>
void SafeRelease(T** ptr)
{
    if (*ptr)
    {
        (*ptr)->Release();
        *ptr = nullptr;
    }
}

#endif //AQUA_DIRECTX_UTIL_H