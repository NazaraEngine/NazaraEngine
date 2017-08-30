// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/Lua/LuaBinding.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::LuaBinding
	* \brief NDK class that represents the binding between the engine & the SDK with the Lua scripting
	*/

	/*!
	* \brief Binds modules to Lua
	*/

	LuaBinding::LuaBinding()
	{
		core    = LuaBinding_Base::BindCore(*this);
		math    = LuaBinding_Base::BindMath(*this);
		network = LuaBinding_Base::BindNetwork(*this);
		utility = LuaBinding_Base::BindUtility(*this);

		#ifndef NDK_SERVER
		audio    = LuaBinding_Base::BindAudio(*this);
		renderer = LuaBinding_Base::BindRenderer(*this);
		graphics = LuaBinding_Base::BindGraphics(*this);
		platform = LuaBinding_Base::BindPlatform(*this);
		#endif

		sdk     = LuaBinding_Base::BindSDK(*this);
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the engine & SDK
	*/

	void LuaBinding::RegisterClasses(Nz::LuaState& state)
	{
		core->Register(state);
		math->Register(state);
		network->Register(state);
		sdk->Register(state);
		utility->Register(state);

		#ifndef NDK_SERVER
		audio->Register(state);
		graphics->Register(state);
		renderer->Register(state);
		platform->Register(state);
		#endif

		// ComponentType (fake enumeration to expose component indexes)
		state.PushTable(0, m_componentBinding.size());
		{
			for (const ComponentBinding& entry : m_componentBinding)
			{
				if (entry.name.IsEmpty())
					continue;

				state.PushField(entry.name, entry.index);
			}
		}
		state.SetGlobal("ComponentType");
	}
}
