// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_IMAGEBUTTONWIDGET_HPP
#define NAZARA_WIDGETS_IMAGEBUTTONWIDGET_HPP

#include <Nazara/Core/Color.hpp>
#include <Nazara/Graphics/SlicedSprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/BaseWidget.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>

namespace Nz
{
	class AbstractTextDrawer;
	class MaterialPass;

	class NAZARA_WIDGETS_API ImageButtonWidget : public BaseWidget
	{
		public:
			inline ImageButtonWidget(BaseWidget* parent, std::shared_ptr<Material> material);
			inline ImageButtonWidget(BaseWidget* parent, std::shared_ptr<Material> material, float cornerSize, float cornerTexCoords);
			ImageButtonWidget(BaseWidget* parent, std::shared_ptr<Material> material, std::shared_ptr<Material> hoveredMaterial, std::shared_ptr<Material> pressedMaterial, float cornerSize, float cornerTexCoords);
			ImageButtonWidget(const ImageButtonWidget&) = delete;
			ImageButtonWidget(ImageButtonWidget&&) = default;
			~ImageButtonWidget() = default;

			inline const Color& GetColor() const;
			inline float GetCornerSize() const;
			inline float GetCornerTexCoords() const;
			inline const std::shared_ptr<Material>& GetHoveredMaterial() const;
			inline const std::shared_ptr<Material>& GetMaterial() const;
			inline const std::shared_ptr<Material>& GetPressedMaterial() const;
			inline const Rectf& GetTextureCoords() const;

			inline void SetColor(const Color& color);
			inline void SetCorner(float size, float texcoords);
			inline void SetHoveredMaterial(std::shared_ptr<Material> material);
			inline void SetMaterial(std::shared_ptr<Material> material);
			inline void SetPressedMaterial(std::shared_ptr<Material> material);
			inline void SetTextureCoords(const Rectf& coords);

			ImageButtonWidget& operator=(const ImageButtonWidget&) = delete;
			ImageButtonWidget& operator=(ImageButtonWidget&&) = default;

			NazaraSignal(OnButtonTrigger, const ImageButtonWidget* /*button*/);

		private:
			void Layout() override;

			void OnMouseEnter() override;
			void OnMouseButtonPress(int x, int y, Mouse::Button button) override;
			void OnMouseButtonRelease(int x, int y, Mouse::Button button) override;
			void OnMouseExit() override;

			void OnRenderLayerUpdated(int baseRenderLayer) override;

			void UpdatePreferredSize();

			std::unique_ptr<ImageButtonWidgetStyle> m_style;
			std::shared_ptr<Material> m_hoveredMaterial;
			std::shared_ptr<Material> m_material;
			std::shared_ptr<Material> m_pressedMaterial;
			Color m_color;
			Rectf m_textureCoords;
			float m_cornerSize;
			float m_cornerTexCoords;
	};
}

#include <Nazara/Widgets/ImageButtonWidget.inl>

#endif // NAZARA_WIDGETS_IMAGEBUTTONWIDGET_HPP
