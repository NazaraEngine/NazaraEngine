// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaAPI.hpp>
#include <NDK/LuaBinding.hpp>

namespace Ndk
{
	bool LuaAPI::Initialize()
	{
		s_binding = new LuaBinding;
		return true;
	}

	void LuaAPI::RegisterClasses(Nz::LuaInstance& instance)
	{
		if (!s_binding && !Initialize())
		{
			NazaraError("Failed to initialize binding");
			return;
		}

		s_binding->RegisterClasses(instance);
	}

	void LuaAPI::Uninitialize()
	{
		delete s_binding;
	}

	LuaBinding* LuaAPI::s_binding = nullptr;
}
