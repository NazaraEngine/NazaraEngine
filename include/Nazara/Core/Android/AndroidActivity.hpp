// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ANDROID_ANDROIDACTIVITY_HPP
#define NAZARA_CORE_ANDROID_ANDROIDACTIVITY_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Utils/FunctionRef.hpp>
#include <Nazara/Utils/Signal.hpp>
#include <android/configuration.h>
#include <android/native_activity.h>
#include <android/window.h>
#include <thread>

namespace Nz
{
	class NAZARA_CORE_API AndroidActivity
	{
		public:
			AndroidActivity(ANativeActivity* activity, void* savedState, std::size_t savedStateSize);
			AndroidActivity(const AndroidActivity&) = delete;
			AndroidActivity(AndroidActivity&&) = delete;
			~AndroidActivity();

			inline AConfiguration* GetConfiguration();
			inline const AConfiguration* GetConfiguration() const;

			inline AInputQueue* GetInputQueue();
			inline const AInputQueue* GetInputQueue() const;

			inline ANativeActivity* GetNativeActivity();
			inline const ANativeActivity* GetNativeActivity() const;
			
			inline ANativeWindow* GetNativeWindow();
			inline const ANativeWindow* GetNativeWindow() const;

			void Poll();

			template<typename F, typename... Args> void Start(F&& f, Args&&... args);

			AndroidActivity& operator=(const AndroidActivity&) = delete;
			AndroidActivity& operator=(AndroidActivity&&) = delete;

			static inline AndroidActivity* Instance();

			NazaraSignal(OnContentRectChanged, AndroidActivity* /*activity*/, const ARect* /*rect*/);
			NazaraSignal(OnDestroy, AndroidActivity* /*activity*/);
			NazaraSignal(OnInputQueueCreated, AndroidActivity* /*activity*/, AInputQueue* /*inputQueue*/);
			NazaraSignal(OnInputQueueDestroyed, AndroidActivity* /*activity*/, AInputQueue* /*inputQueue*/);
			NazaraSignal(OnLowMemory, AndroidActivity* /*activity*/);
			NazaraSignal(OnWindowCreated, AndroidActivity* /*activity*/, ANativeWindow* /*window*/);
			NazaraSignal(OnWindowDestroyed, AndroidActivity* /*activity*/, ANativeWindow* /*window*/);
			NazaraSignal(OnWindowResized, AndroidActivity* /*activity*/, ANativeWindow* /*window*/);

		private:
			void PostMain();
			void PreMain();
			void WaitForAppCallback(FunctionRef<void()> callback, std::string_view eventName);

			FunctionRef<void()>* m_pendingCallback;
			std::condition_variable m_pendingCallbackCV;
			std::mutex m_pendingCallbackMutex;
			std::thread m_appThread;
			AConfiguration* m_config;
			AInputQueue* m_inputQueue;
			ANativeActivity* m_activity;
			ANativeWindow* m_window;

			static AndroidActivity* s_instance;
	};
}

#include <Nazara/Core/Android/AndroidActivity.inl>

#endif // NAZARA_CORE_ANDROID_ANDROIDACTIVITY_HPP
