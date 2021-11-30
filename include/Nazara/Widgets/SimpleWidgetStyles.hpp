// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WIDGETS_SIMPLEWIDGETSTYLES_HPP
#define NAZARA_WIDGETS_SIMPLEWIDGETSTYLES_HPP

#include <Nazara/Graphics/SlicedSprite.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Widgets/WidgetTheme.hpp>
#include <entt/entt.hpp>

namespace Nz
{
	class NAZARA_WIDGETS_API SimpleButtonWidgetStyle : public ButtonWidgetStyle
	{
		public:
			SimpleButtonWidgetStyle(ButtonWidget* buttonWidget, std::shared_ptr<Material> material, std::shared_ptr<Material> hoveredMaterial = {}, std::shared_ptr<Material> pressedMaterial = {}, std::shared_ptr<Material> pressedHoveredMaterial = {});
			SimpleButtonWidgetStyle(const SimpleButtonWidgetStyle&) = delete;
			SimpleButtonWidgetStyle(SimpleButtonWidgetStyle&&) = default;
			~SimpleButtonWidgetStyle() = default;

			void Layout(const Vector2f& size) override;

			void OnHoverBegin() override;
			void OnHoverEnd() override;
			void OnPress() override;
			void OnRelease() override;

			void UpdateText(const AbstractTextDrawer& drawer) override;

			SimpleButtonWidgetStyle& operator=(const SimpleButtonWidgetStyle&) = delete;
			SimpleButtonWidgetStyle& operator=(SimpleButtonWidgetStyle&&) = default;

		protected:
			virtual void UpdateMaterial(bool hovered, bool pressed);

		private:
			std::shared_ptr<Material> m_hoveredMaterial;
			std::shared_ptr<Material> m_material;
			std::shared_ptr<Material> m_pressedMaterial;
			std::shared_ptr<Material> m_pressedHoveredMaterial;
			std::shared_ptr<SlicedSprite> m_sprite;
			std::shared_ptr<TextSprite> m_textSprite;
			entt::entity m_spriteEntity;
			entt::entity m_textEntity;
			bool m_isHovered;
			bool m_isPressed;
	};

	class NAZARA_WIDGETS_API SimpleLabelWidgetStyle : public LabelWidgetStyle
	{
		public:
			SimpleLabelWidgetStyle(LabelWidget* labelWidget, std::shared_ptr<Material> material, std::shared_ptr<Material> hoveredMaterial = {});
			SimpleLabelWidgetStyle(const SimpleLabelWidgetStyle&) = delete;
			SimpleLabelWidgetStyle(SimpleLabelWidgetStyle&&) = default;
			~SimpleLabelWidgetStyle() = default;

			void Layout(const Vector2f& size) override;

			void OnHoverBegin() override;
			void OnHoverEnd() override;

			void UpdateText(const AbstractTextDrawer& drawer) override;

			SimpleLabelWidgetStyle& operator=(const SimpleLabelWidgetStyle&) = delete;
			SimpleLabelWidgetStyle& operator=(SimpleLabelWidgetStyle&&) = default;

		protected:
			virtual void UpdateMaterial(bool hovered);

		private:
			std::shared_ptr<Material> m_hoveredMaterial;
			std::shared_ptr<Material> m_material;
			std::shared_ptr<TextSprite> m_textSprite;
			entt::entity m_entity;
	};
}

#include <Nazara/Widgets/DefaultWidgetTheme.inl>

#endif // NAZARA_WIDGETS_SIMPLEWIDGETSTYLES_HPP
