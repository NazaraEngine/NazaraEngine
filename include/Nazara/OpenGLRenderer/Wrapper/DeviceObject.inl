// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	template<typename C, GLenum ObjectType, typename... CreateArgs>
	DeviceObject<C, ObjectType, CreateArgs...>::DeviceObject(OpenGLDevice& device, CreateArgs... args)
	{
		ErrorFlags errFlags(ErrorMode::ThrowException);
		Create(device, args...);
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	DeviceObject<C, ObjectType, CreateArgs...>::~DeviceObject()
	{
		Destroy();
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	bool DeviceObject<C, ObjectType, CreateArgs...>::Create(OpenGLDevice& device, CreateArgs... args)
	{
		Destroy();

		m_device = &device;

		const Context& context = EnsureDeviceContext();

		m_objectId = C::CreateHelper(*m_device, context, args...);
		if (m_objectId == InvalidObject)
		{
			NazaraError("failed to create OpenGL object"); //< TODO: Handle error message
			return false;
		}

		return true;
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	void DeviceObject<C, ObjectType, CreateArgs...>::Destroy()
	{
		if (IsValid())
		{
			const Context& context = EnsureDeviceContext();

			C::DestroyHelper(*m_device, context, m_objectId);
			m_objectId = InvalidObject;
		}
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	const Context& DeviceObject<C, ObjectType, CreateArgs...>::EnsureDeviceContext() const
	{
		assert(m_device);

		const Context* activeContext = Context::GetCurrentContext();
		if (!activeContext || activeContext->GetDevice() != m_device)
		{
			const Context& referenceContext = m_device->GetReferenceContext();
			if (!Context::SetCurrentContext(&referenceContext))
				throw std::runtime_error("failed to activate context");

			return referenceContext;
		}

		return *activeContext;
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	bool DeviceObject<C, ObjectType, CreateArgs...>::IsValid() const
	{
		return m_objectId != InvalidObject;
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	OpenGLDevice* DeviceObject<C, ObjectType, CreateArgs...>::GetDevice() const
	{
		return m_device;
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	GLuint DeviceObject<C, ObjectType, CreateArgs...>::GetObjectId() const
	{
		return m_objectId;
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	void DeviceObject<C, ObjectType, CreateArgs...>::SetDebugName(std::string_view name)
	{
		const Context& context = EnsureDeviceContext();

		if (context.glObjectLabel)
			context.glObjectLabel(ObjectType, m_objectId, SafeCaster(name.size()), name.data());
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
