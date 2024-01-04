// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/SignalHandlerAppComponent.hpp>
#include <Nazara/Core/ApplicationBase.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Windows.h>
#elif defined(NAZARA_PLATFORM_POSIX)
#include <signal.h>
#include <string.h>
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	void SignalHandlerAppComponent::InstallSignalHandler()
	{
		bool succeeded = false;

#if defined(NAZARA_PLATFORM_WINDOWS)
		succeeded = ::SetConsoleCtrlHandler([](DWORD ctrlType) -> BOOL
#ifdef NAZARA_COMPILER_MINGW
			WINAPI
#endif
		{
			switch (ctrlType)
			{
				case CTRL_C_EVENT: HandleInterruptSignal("CTRL_C"); break;
				case CTRL_BREAK_EVENT: HandleInterruptSignal("CTRL_BREAK"); break;
				case CTRL_CLOSE_EVENT: HandleInterruptSignal("CTRL_CLOSE"); break;
				case CTRL_LOGOFF_EVENT: HandleInterruptSignal("CTRL_LOGOFF"); break;
				case CTRL_SHUTDOWN_EVENT: HandleInterruptSignal("CTRL_SHUTDOWN"); break;
				default:
				{
					std::string signalName = "<unknown CTRL signal " + std::to_string(ctrlType) + ">";
					HandleInterruptSignal(signalName.c_str());
				}
			}

			return TRUE;
		}, TRUE);
#elif defined(NAZARA_PLATFORM_POSIX)
		struct sigaction action;
		sigemptyset(&action.sa_mask);
		action.sa_flags = 0;
		action.sa_handler = [](int sig)
		{
			HandleInterruptSignal(strsignal(sig));
		};

		if (sigaction(SIGINT, &action, nullptr) == 0)
			succeeded = true;

		if (sigaction(SIGTERM, &action, nullptr) == 0)
			succeeded = true;
#endif

		if (!succeeded)
			NazaraError("failed to install interruption signal handlers");
	}

	void SignalHandlerAppComponent::HandleInterruptSignal(const char* signalName)
	{
		assert(ApplicationBase::Instance());
		NazaraNotice("received interruption signal " + std::string(signalName) + ", exiting...");

		ApplicationBase::Instance()->Quit();
	}
}
