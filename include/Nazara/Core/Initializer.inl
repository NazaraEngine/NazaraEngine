// Copyright (C) 2017 Jérôme Leclercq
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

	/*!
	* \ingroup core
	* \class Nz::Initializer
	* \brief Core class that represents a module initializer
	*/

	/*!
	* \brief Constructs a Initializer object with a boolean
	*
	* \param initialize Initialize the module
	*/
	template<typename... Args>
	Initializer<Args...>::Initializer(bool initialize) :
	m_initialized(false)
	{
		if (initialize)
			Initialize();
	}

	/*!
	* \brief Destructs the object and call Uninitialize
	*
	* \see Uninitialize
	*/
	template<typename... Args>
	Initializer<Args...>::~Initializer()
	{
		Uninitialize();
	}

	/*!
	* \brief Initialize the module
	*
	* \see Uninitialize
	*/
	template<typename... Args>
	bool Initializer<Args...>::Initialize()
	{
		if (!m_initialized)
			m_initialized = Detail::Initializer<Args...>::Init();

		return m_initialized;
	}

	/*!
	* \brief Checks whether the module is initialized
	* \return true if initialized
	*/
	template<typename... Args>
	bool Initializer<Args...>::IsInitialized() const
	{
		return m_initialized;
	}

	/*!
	* \brief Uninitialize the module
	*
	* \see Initialize
	*/
	template<typename... Args>
	void Initializer<Args...>::Uninitialize()
	{
		if (m_initialized)
			Detail::Initializer<Args...>::Uninit();
	}

	/*!
	* \brief Converts the initializer to boolean
	* \return true if initialized
	*/
	template<typename... Args>
	Initializer<Args...>::operator bool() const
	{
		return IsInitialized();
	}
}

#include <Nazara/Core/DebugOff.hpp>
