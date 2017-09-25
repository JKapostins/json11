#include "EntryPoint.h"
#if defined(GNARLY_SHARED)
#include "Core/Memory/New.inl"
#endif
#include <Core/FileSystem/VirtualFileSystem.h>
#include <Core/FileSystem/NativeFileSystem.h>
#include <Core/Utility/FileSystem.h>
#include <Core/Memory/IMemoryArena.h>
#include <Core/Module/Module.h>
#include <Core/Utility/Assert.h>
#include "gtest/gtest.h"

namespace engine
{
	namespace
	{
		int g_returnCode = 0;
	}

	void Init(int argc, char** argv)
	{
		core::initialize();
		::testing::InitGoogleTest(&argc, argv);
	}

	void Run()
	{
		g_returnCode = RUN_ALL_TESTS();

	}

	int ShutDown()
	{
		core::shutdown();
		gnarly_assert_message(arenaLeakedMemory(GLOBAL_ARENA) == false, "GLOBAL_ARENA leaked memory!");
		return g_returnCode;
	}
}