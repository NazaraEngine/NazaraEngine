// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_BASEWIDGET_HPP
#define NDK_BASEWIDGET_HPP

#include <NDK/Prerequesites.hpp>
#include <NDK/Entity.hpp>
#include <NDK/EntityOwner.hpp>
#include <NDK/World.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Utility/Mouse.hpp>
#include <Nazara/Utility/Node.hpp>

namespace Ndk
{
	class Canvas;

	class NDK_API BaseWidget : public Nz::Node
	{
		friend Canvas;

		public:
			struct Padding;

			BaseWidget(BaseWidget* parent);
			BaseWidget(const BaseWidget&) = delete;
			BaseWidget(BaseWidget&&) = default;
			virtual ~BaseWidget();

			inline void AddChild(std::unique_ptr<BaseWidget>&& widget);

			void EnableBackground(bool enable);

			//virtual BaseWidget* Clone() const = 0;

			inline Canvas* GetCanvas();
			inline const Padding& GetPadding() const;
			inline const Nz::Vector2f& GetContentSize() const;
			inline Nz::Vector2f GetSize() const;

			virtual void ResizeToContent() = 0;

			inline void SetContentSize(const Nz::Vector2f& size);
			inline void SetPadding(float left, float top, float right, float bottom);
			void SetSize(const Nz::Vector2f& size);

			BaseWidget& operator=(const BaseWidget&) = delete;
			BaseWidget& operator=(BaseWidget&&) = default;

			struct Padding
			{
				float left;
				float top;
				float right;
				float bottom;
			};

		protected:
			EntityHandle CreateEntity();
			void DestroyEntity(Entity* entity);
			virtual void Layout();
			void InvalidateNode() override;

			virtual void OnMouseEnter();
			virtual void OnMouseMoved(int x, int y, int deltaX, int deltaY);
			virtual void OnMouseButtonPress(int x, int y, Nz::Mouse::Button button);
			virtual void OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button);
			virtual void OnMouseExit();

		private:
			inline BaseWidget();

			inline void UpdateCanvasIndex(std::size_t index);

			std::size_t m_canvasIndex;
			std::vector<EntityOwner> m_entities;
			std::vector<std::unique_ptr<BaseWidget>> m_children;
			Canvas* m_canvas;
			EntityOwner m_backgroundEntity;
			Padding m_padding;
			WorldHandle m_world;
			Nz::Color m_backgroundColor;
			Nz::SpriteRef m_backgroundSprite;
			Nz::Vector2f m_contentSize;
			BaseWidget* m_widgetParent;
	};
}

#include <NDK/BaseWidget.inl>

#endif // NDK_BASEWIDGET_HPP
