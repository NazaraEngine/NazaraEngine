// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/OpenGLRenderer/Utils.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	template<typename C, GLenum ObjectType, typename... CreateArgs>
	ContextObject<C, ObjectType, CreateArgs...>::ContextObject(const Context& context, CreateArgs... args)
	{
		ErrorFlags errFlags(ErrorMode::ThrowException);
		Create(context, args...);
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	ContextObject<C, ObjectType, CreateArgs...>::~ContextObject()
	{
		Destroy();
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	bool ContextObject<C, ObjectType, CreateArgs...>::Create(const Context& context, CreateArgs... args)
	{
		Destroy();

		m_context = &context;

		EnsureContext();

		m_objectId = C::CreateHelper(*m_context, args...);
		if (m_objectId == InvalidObject)
		{
			NazaraError("failed to create OpenGL object"); //< TODO: Handle error message
			return false;
		}

		return true;
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	void ContextObject<C, ObjectType, CreateArgs...>::Destroy()
	{
		if (IsValid())
		{
			EnsureContext();

			C::DestroyHelper(*m_context, m_objectId);
			m_objectId = InvalidObject;
		}
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	const Context& ContextObject<C, ObjectType, CreateArgs...>::EnsureContext() const
	{
		const Context* activeContext = Context::GetCurrentContext();
		if (activeContext != m_context.Get())
		{
			if (!Context::SetCurrentContext(m_context.Get()))
				throw std::runtime_error("failed to activate context");
		}

		return *m_context;
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	bool ContextObject<C, ObjectType, CreateArgs...>::IsValid() const
	{
		return m_objectId != InvalidObject;
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	const Context* ContextObject<C, ObjectType, CreateArgs...>::GetContext() const
	{
		return m_context.Get();
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	GLuint ContextObject<C, ObjectType, CreateArgs...>::GetObjectId() const
	{
		return m_objectId;
	}

	template<typename C, GLenum ObjectType, typename... CreateArgs>
	void ContextObject<C, ObjectType, CreateArgs...>::SetDebugName(std::string_view name)
	{
		EnsureContext();

		if (m_context->glObjectLabel)
			m_context->glObjectLabel(ObjectType, m_objectId, SafeCast<GLsizei>(name.size()), name.data());
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
