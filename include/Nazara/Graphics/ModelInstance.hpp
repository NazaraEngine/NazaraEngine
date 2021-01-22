// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MODELINSTANCE_HPP
#define NAZARA_MODELINSTANCE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <memory>

namespace Nz
{
	class AbstractBuffer;
	class MaterialSettings;

	class NAZARA_GRAPHICS_API ModelInstance
	{
		public:
			ModelInstance(const std::shared_ptr<const MaterialSettings>& settings);
			ModelInstance(const ModelInstance&) = delete;
			ModelInstance(ModelInstance&&) noexcept = default;
			~ModelInstance() = default;

			inline std::shared_ptr<AbstractBuffer>& GetInstanceBuffer();
			inline const std::shared_ptr<AbstractBuffer>& GetInstanceBuffer() const;
			inline ShaderBinding& GetShaderBinding();

			ModelInstance& operator=(const ModelInstance&) = delete;
			ModelInstance& operator=(ModelInstance&&) noexcept = default;

		private:
			std::shared_ptr<AbstractBuffer> m_instanceDataBuffer;
			ShaderBindingPtr m_shaderBinding;
	};
}

#include <Nazara/Graphics/ModelInstance.inl>

#endif // NAZARA_MODELINSTANCE_HPP
