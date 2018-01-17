// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GPUQUERY_HPP
#define NAZARA_GPUQUERY_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Enums.hpp>

namespace Nz
{
	class NAZARA_RENDERER_API GpuQuery
	{
		public:
			GpuQuery();
			GpuQuery(const GpuQuery&) = delete;
			GpuQuery(GpuQuery&&) = delete; ///TODO
			~GpuQuery();

			void Begin(GpuQueryMode mode);
			void End();

			unsigned int GetResult() const;

			bool IsResultAvailable() const;

			// Fonctions OpenGL
			unsigned int GetOpenGLID() const;

			GpuQuery& operator=(const GpuQuery&) = delete;
			GpuQuery& operator=(GpuQuery&&) = delete; ///TODO

			static bool IsModeSupported(GpuQueryMode mode);
			static bool IsSupported();

		private:
			GpuQueryMode m_mode;
			unsigned int m_id;
	};
}

#endif // NAZARA_GPUQUERY_HPP
