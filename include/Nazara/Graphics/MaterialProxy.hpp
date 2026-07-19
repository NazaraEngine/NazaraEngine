// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_MATERIALPROXY_HPP
#define NAZARA_GRAPHICS_MATERIALPROXY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/ElementRenderer.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Renderer/ShaderBinding.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <NazaraUtils/Signal.hpp>

namespace Nz
{
	class AbstractViewer;
	class GpuResources;
	struct RenderResourceReferences;
	class SkeletonInstance;

	class NAZARA_GRAPHICS_API MaterialProxy
	{
		public:
			MaterialProxy() = default;
			MaterialProxy(const MaterialProxy&) = default;
			MaterialProxy(MaterialProxy&&) = default;
			virtual ~MaterialProxy();

			virtual void FillRenderResourceReferences(RenderResourceReferences& resourceReferences) const = 0;
			virtual void FillSceneBindings(const ElementRenderer::SceneData& sceneData, std::vector<ShaderBinding::Binding>& bindings) const = 0;
			virtual void FillSkeletonBindings(const SkeletonInstance& skeleton, std::vector<ShaderBinding::Binding>& bindings) const = 0;
			virtual void FillViewerBindings(const AbstractViewer& viewer, std::vector<ShaderBinding::Binding>& bindings) const = 0;

			inline std::size_t GetBindingSetCount() const;
			inline std::size_t GetBindingSetHash(UInt32 setIndex) const;

			virtual const ShaderBinding& GetShaderBinding(GpuResources& renderResources) const = 0;

			MaterialProxy& operator=(const MaterialProxy&) = default;
			MaterialProxy& operator=(MaterialProxy&&) = default;

			NazaraSignal(OnMaterialProxyRelease, MaterialProxy* /*materialProxy*/);

		protected:
			HybridVector<std::size_t, 4> m_bindingSetHashes;
	};
}

#include <Nazara/Graphics/MaterialProxy.inl>

#endif // NAZARA_GRAPHICS_MATERIALPROXY_HPP
