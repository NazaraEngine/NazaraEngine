// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERPIPELINELAYOUT_HPP
#define NAZARA_RENDERPIPELINELAYOUT_HPP

#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Renderer/Enums.hpp>
#include <string>
#include <vector>

namespace Nz
{
	struct RenderPipelineLayoutInfo
	{
		struct Binding
		{
			std::string name;
			ShaderBindingType type;
			ShaderStageTypeFlags shaderStageFlags;
			unsigned int index;
		};

		std::vector<Binding> bindings;
	};

	class RenderPipelineLayout;

	using RenderPipelineLayoutRef = ObjectRef<RenderPipelineLayout>;

	class RenderPipelineLayout : public RefCounted
	{
		public:
			inline RenderPipelineLayout();
			inline ~RenderPipelineLayout();

			inline bool Create(RenderPipelineLayoutInfo pipelineInfo);
			inline void Destroy();

			inline const RenderPipelineLayoutInfo& GetInfo() const;

			inline bool IsValid() const;

			inline bool operator==(const RenderPipelineLayout& rhs) const;
			inline bool operator!=(const RenderPipelineLayout& rhs) const;

		private:
			RenderPipelineLayoutInfo m_layoutInfo;
			bool m_valid;
	};
}

#include <Nazara/Renderer/RenderPipelineLayout.inl>

#endif // NAZARA_RENDERPIPELINELAYOUT_HPP
