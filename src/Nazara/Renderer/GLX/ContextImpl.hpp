// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CONTEXTIMPL_HPP
#define NAZARA_CONTEXTIMPL_HPP

#include <Nazara/Renderer/OpenGL.hpp>

namespace Nz
{
	struct ContextParameters;

	class ContextImpl
	{
		public:
			ContextImpl();
			~ContextImpl();

			bool Activate();

			bool Create(ContextParameters& parameters);

			void Destroy();

			void EnableVerticalSync(bool enabled);

			void SwapBuffers();

			static bool Desactivate();

		private:
			GLX::Colormap m_colormap;
			GLX::GLXContext m_context;
			GLX::Window m_window;
			bool m_ownsWindow;
	};
}

#endif // NAZARA_CONTEXTIMPL_HPP
