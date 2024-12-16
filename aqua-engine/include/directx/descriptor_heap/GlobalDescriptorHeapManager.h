#ifndef AQUA_GLOBALDESCRIPTORHEAPMANAGER_H
#define AQUA_GLOBALDESCRIPTORHEAPMANAGER_H


#include "GlobalDescriptorHeap.h"
#include "ShaderGlobalDescriptorHeap.h"

namespace AquaEngine
{
    class GlobalDescriptorHeapManager
    {
    public:
        static void Init();

        static DescriptorHeapSegmentManager& CreateShaderManager(const std::string& name, unsigned int size, D3D12_DESCRIPTOR_HEAP_TYPE type);
        static DescriptorHeapSegmentManager& GetCPUHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE type);
        static DescriptorHeapSegmentManager& GetShaderHeapManager(D3D12_DESCRIPTOR_HEAP_TYPE type, const std::string& name);

        static void SetToCommand(Command &command);

        static void Shutdown();
    private:
        static std::array<ShaderGlobalDescriptorHeap, 2> m_shaderHeaps;
        static std::array<GlobalDescriptorHeap, 2> m_heaps;
    };
}


#endif //AQUA_GLOBALDESCRIPTORHEAPMANAGER_H
