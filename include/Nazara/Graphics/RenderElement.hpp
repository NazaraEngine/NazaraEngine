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
			inline RenderElement(BasicRenderElement elementType, UInt32 renderMask);
			inline RenderElement(UInt8 elementType, UInt32 renderMask);
			virtual ~RenderElement();

			virtual UInt64 ComputeSortKey(const RenderQueueRegistry& registry) const = 0;

			inline UInt8 GetElementType() const;
			inline UInt32 GetRenderMask() const;

			virtual void Register(RenderQueueRegistry& registry) const = 0;

		private:
			UInt8 m_elementType;
			UInt32 m_renderMask;
	};
}

#include <Nazara/Graphics/RenderElement.inl>

#endif // NAZARA_GRAPHICS_RENDERELEMENT_HPP
