// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/LuaAPI.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <NDK/LuaBinding.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::LuaAPI
	* \brief NDK class that represents the api used for Lua
	*/

	/*!
	* \brief Gets the internal binding for Lua
	* \return A pointer to the binding
	*/
	LuaBinding* LuaAPI::GetBinding()
	{
		if (!s_binding && !Initialize())
		{
			NazaraError("Failed to initialize binding");
			return nullptr;
		}

		return s_binding;
	}

	/*!
	* \brief Initializes the LuaAPI module
	* \return true if initialization is successful
	*/

	bool LuaAPI::Initialize()
	{
		s_binding = new LuaBinding;
		return true;
	}

	/*!
	* \brief Registers the classes that will be used by the Lua instance
	*
	* \param instance Lua instance that will interact with the engine & SDK
	*/

	void LuaAPI::RegisterClasses(Nz::LuaInstance& instance)
	{
		Nz::ErrorFlags errFlags(Nz::ErrorFlag_ThrowException, true);
		GetBinding()->RegisterClasses(instance);
	}

	/*!
	* \brief Uninitializes the LuaAPI module
	*/

	void LuaAPI::Uninitialize()
	{
		delete s_binding;
	}

	LuaBinding* LuaAPI::s_binding = nullptr;
}
