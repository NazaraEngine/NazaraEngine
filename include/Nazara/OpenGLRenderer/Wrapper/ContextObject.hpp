// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_CONTEXTOBJECT_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_CONTEXTOBJECT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovableValue.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <string>

namespace Nz::GL
{
	template<typename C, GLenum ObjectType, typename... CreateArgs>
	class ContextObject
	{
		public:
			ContextObject() = default;
			ContextObject(const Context& context, CreateArgs... args);
			ContextObject(const ContextObject&) = delete;
			ContextObject(ContextObject&& object) noexcept = default;
			~ContextObject();

			bool Create(const Context& context, CreateArgs... args);
			void Destroy();

			const Context& EnsureContext() const;

			bool IsValid() const;

			const Context* GetContext() const;
			GLuint GetObjectId() const;

			void SetDebugName(const std::string_view& name);

			ContextObject& operator=(const ContextObject&) = delete;
			ContextObject& operator=(ContextObject&& object) noexcept = default;

			static constexpr GLuint InvalidObject = 0;

		protected:
			MovablePtr<const Context> m_context;
			MovableValue<GLuint> m_objectId;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/ContextObject.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_CONTEXTOBJECT_HPP
