// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/JoltPhysics3D.hpp>
#include <Nazara/JoltPhysics3D/Config.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <cstdarg>
#include <iostream>
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
	std::cout << buffer << std::endl;
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

		m_threadPool = std::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, Core::Instance()->GetHardwareInfo().GetCpuThreadCount());
	}

	JoltPhysics3D::~JoltPhysics3D()
	{
		m_threadPool.reset();
		// FIXME: Uncomment when next version of Jolt gets released
		//JPH::UnregisterTypes();

		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}

	JPH::JobSystem& JoltPhysics3D::GetThreadPool()
	{
		return *m_threadPool;
	}

	JoltPhysics3D* JoltPhysics3D::s_instance;
}
