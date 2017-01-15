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
#include <Nazara/Utility/Event.hpp>
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

			template<typename T, typename... Args> T* Add(Args&&... args);
			inline void AddChild(std::unique_ptr<BaseWidget>&& widget);

			inline void Center();

			inline void Destroy();

			void EnableBackground(bool enable);

			//virtual BaseWidget* Clone() const = 0;

			inline const Nz::Color& GetBackgroundColor() const;
			inline Canvas* GetCanvas();
			inline const Padding& GetPadding() const;
			inline const Nz::Vector2f& GetContentSize() const;
			inline Nz::Vector2f GetSize() const;

			void GrabKeyboard();

			virtual void ResizeToContent() = 0;

			void SetBackgroundColor(const Nz::Color& color);
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

			virtual void OnKeyPressed(const Nz::WindowEvent::KeyEvent& key);
			virtual void OnKeyReleased(const Nz::WindowEvent::KeyEvent& key);
			virtual void OnMouseEnter();
			virtual void OnMouseMoved(int x, int y, int deltaX, int deltaY);
			virtual void OnMouseButtonPress(int x, int y, Nz::Mouse::Button button);
			virtual void OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button);
			virtual void OnMouseExit();
			virtual void OnParentResized(const Nz::Vector2f& newSize);
			virtual void OnTextEntered(char32_t character, bool repeated);

		private:
			inline BaseWidget();

			inline void DestroyChild(BaseWidget* widget);
			inline void NotifyParentResized(const Nz::Vector2f& newSize);
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
