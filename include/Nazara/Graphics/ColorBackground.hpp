// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_COLORBACKGROUND_HPP
#define NAZARA_COLORBACKGROUND_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Renderer/UberShader.hpp>

namespace Nz
{
	class ColorBackground;

	using ColorBackgroundConstRef = ObjectRef<const ColorBackground>;
	using ColorBackgroundRef = ObjectRef<ColorBackground>;

	class NAZARA_GRAPHICS_API ColorBackground : public AbstractBackground
	{
		public:
			ColorBackground(const Color& color = Color::Black);
			ColorBackground(const ColorBackground&) = default;
			ColorBackground(ColorBackground&&) = delete;

			void Draw(const AbstractViewer* viewer) const override;

			BackgroundType GetBackgroundType() const override;
			Color GetColor() const;

			void SetColor(const Color& color);

			ColorBackground& operator=(ColorBackground&&) = delete;

			template<typename... Args> static ColorBackgroundRef New(Args&&... args);

		private:
			Color m_color;
			UberShaderConstRef m_uberShader;
			const UberShaderInstance* m_uberShaderInstance;
			int m_materialDiffuseUniform;
			int m_vertexDepthUniform;
	};
}

#include <Nazara/Graphics/ColorBackground.inl>

#endif // NAZARA_COLORBACKGROUND_HPP
