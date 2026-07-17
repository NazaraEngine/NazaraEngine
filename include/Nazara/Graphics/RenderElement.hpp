// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERELEMENT_HPP
#define NAZARA_GRAPHICS_RENDERELEMENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class RenderQueueRegistry;

	class NAZARA_GRAPHICS_API RenderElement
	{
		public:
			inline RenderElement(BasicRenderElement elementType, UInt32 instanceIndex, Int32 renderLayer, UInt32 renderMask);
			inline RenderElement(UInt8 elementType, UInt32 instanceIndex, Int32 renderLayer, UInt32 renderMask);
			virtual ~RenderElement();

			inline UInt8 GetElementType() const;
			inline UInt32 GetInstanceIndex() const;
			inline Int32 GetRenderLayer() const;
			inline UInt32 GetRenderMask() const;
			inline UInt64 GetSortKey() const;

			virtual void Register(RenderQueueRegistry& registry) const = 0;

			inline void UpdateSortKey(const RenderQueueRegistry& registry);

		protected:
			virtual UInt64 ComputeSortKey(const RenderQueueRegistry& registry) const = 0;

		private:
			Int32 m_renderLayer;
			UInt8 m_elementType;
			UInt32 m_instanceIndex;
			UInt32 m_renderMask;
			UInt64 m_sortKey;
	};
}

#include <Nazara/Graphics/RenderElement.inl>

#endif // NAZARA_GRAPHICS_RENDERELEMENT_HPP
