// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
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
			NazaraError("Failed to create OpenGL object"); //< TODO: Handle error message
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
	void DeviceObject<C, ObjectType, CreateArgs...>::SetDebugName(const std::string_view& name)
	{
		const Context& context = EnsureDeviceContext();

		if (context.glObjectLabel)
			context.glObjectLabel(ObjectType, m_objectId, name.size(), name.data());
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	const Context& DeviceObject<C, ObjectType, CreateArgs...>::EnsureDeviceContext()
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
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
