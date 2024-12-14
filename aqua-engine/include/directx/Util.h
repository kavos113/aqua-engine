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

inline std::wstring GetWideString(const std::string& str)
{
    int size = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size, 0);
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wstr[0], size);
    return wstr;
}

inline size_t AlignmentSize(size_t size, size_t alignment)
{
    return (size + alignment - 1) & ~(alignment - 1);
}

#endif //AQUA_DIRECTX_UTIL_H