// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_SIGNALHANDLERAPPCOMPONENT_HPP
#define NAZARA_CORE_SIGNALHANDLERAPPCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/ApplicationComponent.hpp>

namespace Nz
{
	class NAZARA_CORE_API SignalHandlerAppComponent : public ApplicationComponent
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

			static void HandleInterruptSignal(const char* signalName);
	};
}

#include <Nazara/Core/SignalHandlerAppComponent.inl>

#endif // NAZARA_CORE_SIGNALHANDLERAPPCOMPONENT_HPP
