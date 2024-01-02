// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/JoltPhysics3D.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/JoltPhysics3D/Config.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <cstdarg>
#include <Nazara/JoltPhysics3D/Debug.hpp>

// Callback for traces, connect this to your own trace function if you have one
static void TraceImpl(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	// Print to the TTY
	NazaraError(buffer);
}

namespace Nz
{
	JoltPhysics3D::JoltPhysics3D(Config /*config*/) :
	ModuleBase("JoltPhysics3D", this)
	{
		JPH::RegisterDefaultAllocator();
		JPH::Trace = TraceImpl;
		JPH::Factory::sInstance = new JPH::Factory;
		JPH::RegisterTypes();

		int threadCount = -1; //< system CPU core count
#ifdef NAZARA_PLATFORM_WEB
		threadCount = 0; // no thread on web for now
#endif

		m_threadPool = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, threadCount);
	}

	JoltPhysics3D::~JoltPhysics3D()
	{
		m_threadPool.reset();
		JPH::UnregisterTypes();

		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}

	JPH::JobSystem& JoltPhysics3D::GetThreadPool()
	{
		return *m_threadPool;
	}

	JoltPhysics3D* JoltPhysics3D::s_instance;
}
