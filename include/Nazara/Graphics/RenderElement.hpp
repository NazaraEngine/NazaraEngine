// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERELEMENT_HPP
#define NAZARA_RENDERELEMENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Enums.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class RenderQueueRegistry;

	class RenderElement
	{
		public:
			inline RenderElement(BasicRenderElement elementType);
			inline RenderElement(UInt8 elementType);
			virtual ~RenderElement();

			virtual UInt64 ComputeSortingScore(const RenderQueueRegistry& registry) const = 0;

			inline UInt8 GetElementType() const;

			virtual void Register(RenderQueueRegistry& registry) const = 0;

		private:
			UInt8 m_elementType;
	};
}

#include <Nazara/Graphics/RenderElement.inl>

#endif
