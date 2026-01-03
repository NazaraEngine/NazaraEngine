// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_SIGNALHANDLERAPPCOMPONENT_HPP
#define NAZARA_CORE_SIGNALHANDLERAPPCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>
#include <string_view>

namespace Nz
{
	class NAZARA_CORE_API SignalHandlerAppComponent final : public ApplicationComponent
	{
		public:
			inline SignalHandlerAppComponent(ApplicationBase& app);
			SignalHandlerAppComponent(const SignalHandlerAppComponent&) = delete;
			SignalHandlerAppComponent(SignalHandlerAppComponent&&) = delete;
			~SignalHandlerAppComponent() = default;

			SignalHandlerAppComponent& operator=(const SignalHandlerAppComponent&) = delete;
			SignalHandlerAppComponent& operator=(SignalHandlerAppComponent&&) = delete;

		private:
			void InstallSignalHandler();

			static void HandleInterruptSignal(std::string_view signalName);
	};
}

#include <Nazara/Core/SignalHandlerAppComponent.inl>

#endif // NAZARA_CORE_SIGNALHANDLERAPPCOMPONENT_HPP
