// Copyright (C) 2025 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Android/AndroidActivity.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <chrono>

namespace Nz
{
	AndroidActivity::AndroidActivity(ANativeActivity* activity, void* savedState, std::size_t savedStateSize) :
	m_pendingCallback(nullptr),
	m_inputQueue(nullptr),
	m_activity(activity),
	m_window(nullptr)
	{
		NazaraAssert(s_instance == nullptr, "only one instance of AndroidActivity can exist at a given time");
		s_instance = this;

		activity->callbacks->onContentRectChanged = [](ANativeActivity* /*activity*/, const ARect* rect)
		{
			AndroidActivity* activity = s_instance;
			activity->WaitForAppCallback([=]
			{
				activity->OnContentRectChanged(activity, rect);
			}, "onContentRectChanged");
			NazaraDebug("size: bottom=" + std::to_string(rect->bottom) + ",top=" + std::to_string(rect->top) + ",left=" + std::to_string(rect->left) + ",right=" + std::to_string(rect->right));
		};

		activity->callbacks->onDestroy = [](ANativeActivity* /*activity*/)
		{
			AndroidActivity* activity = s_instance;
			activity->WaitForAppCallback([=]
			{
				activity->OnDestroy(activity);
			}, "onDestroy");

			if (activity->m_appThread.joinable())
				activity->m_appThread.join();
		};

		activity->callbacks->onInputQueueCreated = [](ANativeActivity* /*activity*/, AInputQueue* queue)
		{
			AndroidActivity* activity = s_instance;
			activity->WaitForAppCallback([=]
			{
				activity->m_inputQueue = queue;
				activity->OnInputQueueCreated(activity, queue);
			}, "onInputQueueCreated");
		};

		activity->callbacks->onInputQueueDestroyed = [](ANativeActivity* /*activity*/, AInputQueue* queue)
		{
			AndroidActivity* activity = s_instance;
			activity->WaitForAppCallback([=]
			{
				activity->OnInputQueueDestroyed(activity, queue);
				activity->m_inputQueue = nullptr;
			}, "onInputQueueDestroyed");
		};

		activity->callbacks->onLowMemory = [](ANativeActivity* /*activity*/)
		{
			AndroidActivity* activity = s_instance;
			activity->WaitForAppCallback([=]
			{
				activity->OnLowMemory(activity);
			}, "onLowMemory");
		};

		activity->callbacks->onNativeWindowCreated = [](ANativeActivity* /*activity*/, ANativeWindow* window)
		{
			AndroidActivity* activity = s_instance;
			activity->WaitForAppCallback([=]
			{
				activity->m_window = window;
				activity->OnWindowCreated(activity, window);
			}, "onNativeWindowCreated");
		};

		activity->callbacks->onNativeWindowDestroyed = [](ANativeActivity* /*activity*/, ANativeWindow* window)
		{
			AndroidActivity* activity = s_instance;
			activity->WaitForAppCallback([=]
			{
				activity->OnWindowDestroyed(activity, window);
				activity->m_window = nullptr;
			}, "onNativeWindowDestroyed");
		};

		activity->callbacks->onNativeWindowResized = [](ANativeActivity* /*activity*/, ANativeWindow* window)
		{
			AndroidActivity* activity = s_instance;
			activity->WaitForAppCallback([=]
			{
				activity->OnWindowResized(activity, window);
			}, "onNativeWindowResized");
		};
	}

	AndroidActivity::~AndroidActivity()
	{
		s_instance = nullptr;
	}

	void AndroidActivity::Poll()
	{
		std::unique_lock lock(m_pendingCallbackMutex);
		if (m_pendingCallback)
		{
			(*m_pendingCallback)();
			m_pendingCallback = nullptr;

			lock.unlock();
			m_pendingCallbackCV.notify_all();
		}
	}

	void AndroidActivity::PostMain()
	{
		ANativeActivity_finish(m_activity);
	}

	void AndroidActivity::PreMain()
	{
		m_config = AConfiguration_new();
		AConfiguration_fromAssetManager(m_config, m_activity->assetManager);
	}

	void AndroidActivity::WaitForAppCallback(FunctionRef<void()> callback, std::string_view eventName)
	{
		using namespace std::chrono_literals;

		NazaraDebug("received Android event " + std::string(eventName));

		std::unique_lock lock(m_pendingCallbackMutex);
		m_pendingCallback = &callback;
		bool eventHandled = m_pendingCallbackCV.wait_for(lock, 3'000ms, [&]{ return m_pendingCallback == nullptr; });
		if (!eventHandled)
			NazaraError("Application lost an Android event! (" + std::string(eventName) + ")");
	}

	AndroidActivity* AndroidActivity::s_instance = nullptr;
}
