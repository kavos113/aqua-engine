#include <gtest/gtest.h>
#include "directx/wrapper/Factory.h"

using namespace AquaEngine;

TEST(Factory, Init)
{
    Factory::Init(false);
    EXPECT_NE(Factory::Get(), nullptr);

    Factory::Shutdown();
}

TEST(Factory, EnableDebugLayer)
{
    Factory::Init(true);
    Factory::EnableDebugLayer();

    EXPECT_NE(Factory::Get(), nullptr);

    Factory::Shutdown();
}