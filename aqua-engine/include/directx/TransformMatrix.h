#ifndef TRANSFORMMATRIX_H
#define TRANSFORMMATRIX_H
#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>

#include "buffer/ConstantBufferView.h"
#include "buffer/GPUBuffer.h"
#include "descriptor_heap/DescriptorHeapSegment.h"

namespace AquaEngine
{
    class TransformMatrix
    {
    public:
        void CreateMatrixBuffer(
            std::unique_ptr<D3D12_DESCRIPTOR_RANGE> matrix_range,
            DescriptorHeapSegmentManager &manager
        );

        void CreateMatrixBuffer(const std::shared_ptr<DescriptorHeapSegment> &segment, int offset = 0);

        void SetMatrixBuffer(const DirectX::XMMATRIX &matrix) const;

        void SetToCommand(Command &command) const;

    private:
        struct Matrix
        {
            DirectX::XMMATRIX world;
        };

        GPUBuffer<Matrix> m_matrixBuffer;
        ConstantBufferView m_matrixCBV;
    };
} // AquaEngine

#endif //TRANSFORMMATRIX_H
