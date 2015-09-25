// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	namespace Detail
	{
		template<typename...> struct Initializer;

		template<typename T, typename... Rest>
		struct Initializer<T, Rest...>
		{
			static bool Init()
			{
				if (T::Initialize())
				{
					if (Initializer<Rest...>::Init())
						return true;
					else
						T::Uninitialize();
				}

				return false;
			}

			static void Uninit()
			{
				Initializer<Rest...>::Uninit();
				T::Uninitialize();
			}
		};

		template<>
		struct Initializer<>
		{
			static bool Init()
			{
				return true;
			}

			static void Uninit()
			{
			}
		};
	}


	template<typename... Args>
	Initializer<Args...>::Initializer(bool initialize) :
	m_initialized(false)
	{
		if (initialize)
			Initialize();
	}

	template<typename... Args>
	Initializer<Args...>::~Initializer()
	{
		Uninitialize();
	}

	template<typename... Args>
	bool Initializer<Args...>::Initialize()
	{
		if (!m_initialized)
			m_initialized = Detail::Initializer<Args...>::Init();

		return m_initialized;
	}

	template<typename... Args>
	bool Initializer<Args...>::IsInitialized() const
	{
		return m_initialized;
	}

	template<typename... Args>
	void Initializer<Args...>::Uninitialize()
	{
		if (m_initialized)
			Detail::Initializer<Args...>::Uninit();
	}

	template<typename... Args>
	Initializer<Args...>::operator bool() const
	{
		return IsInitialized();
	}
}

#include <Nazara/Core/DebugOff.hpp>
