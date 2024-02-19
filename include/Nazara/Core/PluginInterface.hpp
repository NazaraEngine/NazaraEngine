// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_PLUGININTERFACE_HPP
#define NAZARA_CORE_PLUGININTERFACE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <NazaraUtils/TypeTraits.hpp>
#include <string>

#ifdef NAZARA_COMPILER_MSVC
#define NazaraPluginPrefix ""
#else
#define NazaraPluginPrefix "lib"
#endif

namespace Nz
{
	class NAZARA_CORE_API PluginInterface
	{
		public:
			PluginInterface() = default;
			PluginInterface(const PluginInterface&) = delete;
			PluginInterface(PluginInterface&&) = delete;
			virtual ~PluginInterface();

			virtual bool Activate() = 0;
			virtual void Deactivate() = 0;

			virtual std::string_view GetDescription() const = 0;
			virtual std::string_view GetName() const = 0;
			virtual UInt32 GetVersion() const = 0;

			PluginInterface& operator=(const PluginInterface&) = delete;
			PluginInterface& operator=(PluginInterface&&) = delete;
	};

#ifdef NAZARA_PLUGINS_STATIC
	template<typename T>
	struct PluginProvider
	{
		static_assert(AlwaysFalse<T>(), "unknown plugin");
	};
#endif
}

#include <Nazara/Core/PluginInterface.inl>

#endif // NAZARA_CORE_PLUGININTERFACE_HPP
