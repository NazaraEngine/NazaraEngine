// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_HPP
#define NAZARA_CORE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/HardwareInfo.hpp>
#include <Nazara/Core/ModuleBase.hpp>
#include <Nazara/Core/Modules.hpp>
#include <NazaraUtils/TypeList.hpp>
#include <optional>

namespace Nz
{
	class NAZARA_CORE_API Core : public ModuleBase<Core>
	{
		friend ModuleBase;

		public:
			using Dependencies = TypeList<>;

			struct Config {};

			Core(Config /*config*/);
			~Core();

			inline const HardwareInfo& GetHardwareInfo() const;

		private:
			std::optional<HardwareInfo> m_hardwareInfo;

			static Core* s_instance;
	};
}

#include <Nazara/Core/Core.inl>

#endif // NAZARA_CORE_HPP
