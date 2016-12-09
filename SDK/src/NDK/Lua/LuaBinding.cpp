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
		audio = LuaBinding_Base::BindAudio(*this);
		renderer = LuaBinding_Base::BindRenderer(*this);
		graphics = LuaBinding_Base::BindGraphics(*this);
		#endif

		sdk     = LuaBinding_Base::BindSDK(*this);
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the engine & SDK
	*/

	void LuaBinding::RegisterClasses(Nz::LuaInstance& instance)
	{
		core->Register(instance);
		math->Register(instance);
		network->Register(instance);
		sdk->Register(instance);
		utility->Register(instance);

		#ifndef NDK_SERVER
		audio->Register(instance);
		graphics->Register(instance);
		renderer->Register(instance);
		#endif

		// ComponentType (fake enumeration to expose component indexes)
		instance.PushTable(0, m_componentBinding.size());
		{
			for (const ComponentBinding& entry : m_componentBinding)
			{
				if (entry.name.IsEmpty())
					continue;

				instance.PushField(entry.name, entry.index);
			}
		}
		instance.SetGlobal("ComponentType");
	}
}
