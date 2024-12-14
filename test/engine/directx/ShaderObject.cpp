#include <gtest/gtest.h>

#include "AquaEngine.h"

class ShaderObjectTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Factory::Init(true);
        Device::GetAdaptors();
        Device::Init(0);
        GlobalDescriptorHeapManager::Init();
        command = new Command();
    }
    
    void TearDown() override
    {
        delete command;
        Device::Shutdown();
        Factory::Shutdown();
    }
    
    Command* command;
};

TEST_F(ShaderObjectTest, PS)
{
    ShaderObject ps;
    ps.CompileFromFile(L"ps.hlsl", "psMain", "ps_5_0");
    
    ASSERT_NE(ps.Bytecode().pShaderBytecode, nullptr);
    ASSERT_NE(ps.Bytecode().BytecodeLength, 0);
}

TEST_F(ShaderObjectTest, VS)
{
    ShaderObject vs;
    vs.CompileFromFile(L"vs.hlsl", "vsMain", "vs_5_0");
    
    ASSERT_NE(vs.Bytecode().pShaderBytecode, nullptr);
    ASSERT_NE(vs.Bytecode().BytecodeLength, 0);
}