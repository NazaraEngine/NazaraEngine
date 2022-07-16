// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#if 0

#include <Nazara/Widgets/ScrollAreaWidget.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Widgets/Debug.hpp>

namespace Nz
{
	namespace
	{
		constexpr float scrollbarPadding = 5.f;
	}

	ScrollAreaWidget::ScrollAreaWidget(BaseWidget* parent, BaseWidget* content) :
	BaseWidget(parent),
	m_content(content),
	m_isGrabbed(false),
	m_isScrollbarEnabled(true),
	m_scrollRatio(0.f)
	{
		m_content->SetParent(this);
		m_content->SetPosition(Nz::Vector3f::Zero());

		m_style = GetTheme()->CreateStyle(this);
		SetRenderLayerCount(m_style->GetRenderLayerCount());

		Resize(m_content->GetSize()); //< will automatically layout

		m_scrollbarBackgroundSprite = Nz::Sprite::New();
		m_scrollbarBackgroundSprite->SetColor(Nz::Color(62, 62, 62));

		m_scrollbarBackgroundEntity = CreateEntity();
		m_scrollbarBackgroundEntity->AddComponent<NodeComponent>().SetParent(this);
		m_scrollbarBackgroundEntity->AddComponent<GraphicsComponent>().Attach(m_scrollbarBackgroundSprite, 1);

		m_scrollbarSprite = Nz::Sprite::New();
		m_scrollbarSprite->SetColor(Nz::Color(104, 104, 104));

		m_scrollbarEntity = CreateEntity();
		m_scrollbarEntity->AddComponent<NodeComponent>().SetParent(this);
		m_scrollbarEntity->AddComponent<GraphicsComponent>().Attach(m_scrollbarSprite);
	}

	void ScrollAreaWidget::EnableScrollbar(bool enable)
	{
		if (m_isScrollbarEnabled != enable)
		{
			m_isScrollbarEnabled = enable;

			bool isVisible = IsScrollbarVisible();
			m_scrollbarEntity->Enable(isVisible);
			m_scrollbarBackgroundEntity->Enable(isVisible);
		}
	}

	void ScrollAreaWidget::ScrollToRatio(float ratio)
	{
		m_scrollRatio = Nz::Clamp(ratio, 0.f, 1.f);

		float widgetHeight = GetHeight();
		float maxHeight = widgetHeight - m_scrollbarSprite->GetSize().y - 2.f * scrollbarPadding;

		auto& scrollbarNode = m_scrollbarEntity->GetComponent<NodeComponent>();
		scrollbarNode.SetPosition(Nz::Vector2f(scrollbarNode.GetPosition(Nz::CoordSys_Local).x, scrollbarPadding + m_scrollRatio * maxHeight));

		float contentPosition = m_scrollRatio * (widgetHeight - m_content->GetHeight());

		m_content->SetPosition(0.f, contentPosition);
		m_content->SetRenderingRect(Nz::Rectf(-std::numeric_limits<float>::infinity(), -contentPosition, std::numeric_limits<float>::infinity(), widgetHeight));
	}

	Nz::Rectf ScrollAreaWidget::GetScrollbarRect() const
	{
		Nz::Vector2f scrollBarPosition = Nz::Vector2f(m_scrollbarEntity->GetComponent<NodeComponent>().GetPosition(Nz::CoordSys_Local));
		Nz::Vector2f scrollBarSize = m_scrollbarSprite->GetSize();
		return Nz::Rectf(scrollBarPosition.x, scrollBarPosition.y, scrollBarSize.x, scrollBarSize.y);
	}

	void ScrollAreaWidget::Layout()
	{
		constexpr float scrollBarBackgroundWidth = 20.f;
		constexpr float scrollBarWidth = scrollBarBackgroundWidth - 2.f * scrollbarPadding;

		float areaHeight = GetHeight();
		float contentHeight = m_content->GetHeight();

		if (contentHeight > areaHeight)
		{
			m_hasScrollbar = true;

			Nz::Vector2f contentSize(GetWidth() - scrollBarBackgroundWidth, contentHeight);
			m_content->Resize(contentSize);

			if (m_isScrollbarEnabled)
			{
				m_scrollbarEntity->Enable();
				m_scrollbarBackgroundEntity->Enable();
			}

 			float scrollBarHeight = std::max(std::floor(areaHeight * (areaHeight / contentHeight)), 20.f);

			m_scrollbarBackgroundSprite->SetSize(scrollBarBackgroundWidth, areaHeight);
			m_scrollbarSprite->SetSize(scrollBarWidth, scrollBarHeight);

			m_scrollbarBackgroundEntity->GetComponent<NodeComponent>().SetPosition(contentSize.x, 0.f);
			m_scrollbarEntity->GetComponent<NodeComponent>().SetPosition(contentSize.x + (scrollBarBackgroundWidth - scrollBarWidth) / 2.f, 0.f);

			ScrollToRatio(m_scrollRatio);
		}
		else
		{
			m_hasScrollbar = false;

			m_content->Resize(GetSize());

			m_scrollbarEntity->Disable();
			m_scrollbarBackgroundEntity->Disable();

			ScrollToRatio(0.f);
		}

		BaseWidget::Layout();
	}

	void ScrollAreaWidget::OnMouseButtonPress(int x, int y, Nz::Mouse::Button button)
	{
		if (button != Nz::Mouse::Left)
			return;

		if (!m_isGrabbed)
		{
			m_style->OnGrab();

			auto& scrollbarNode = m_scrollbarEntity->GetComponent<NodeComponent>();

			m_grabbedDelta.Set(x, int(y - scrollbarNode.GetPosition(Nz::CoordSys_Local).y));
		}
	}

	void ScrollAreaWidget::OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button)
	{
		if (button != Nz::Mouse::Left)
			return;

		if (m_scrollbarStatus == ScrollBarStatus::Grabbed)
		{
			Nz::Rectf scrollBarRect = GetScrollbarRect();
			UpdateScrollbarStatus((scrollBarRect.Contains(Nz::Vector2f(float(x), float(y)))) ? ScrollBarStatus::Hovered : ScrollBarStatus::None);
		}
	}

	void ScrollAreaWidget::OnMouseExit()
	{
		//if (m_scrollbarStatus == ScrollBarStatus::Hovered)
			UpdateScrollbarStatus(ScrollBarStatus::None);
	}

	void ScrollAreaWidget::OnMouseMoved(int x, int y, int /*deltaX*/, int /*deltaY*/)
	{
		if (m_scrollbarStatus == ScrollBarStatus::Grabbed)
		{
			float height = GetHeight();
			float maxHeight = height - m_scrollbarSprite->GetSize().y;
			float newHeight = Nz::Clamp(float(y - m_grabbedDelta.y), 0.f, maxHeight);

			ScrollToHeight(newHeight / maxHeight * m_content->GetHeight());
		}
		else
		{
			Nz::Rectf scrollBarRect = GetScrollbarRect();
			UpdateScrollbarStatus((scrollBarRect.Contains(Nz::Vector2f(float(x), float(y)))) ? ScrollBarStatus::Hovered : ScrollBarStatus::None);
		}
	}

	void ScrollAreaWidget::OnMouseWheelMoved(int /*x*/, int /*y*/, float delta)
	{
		constexpr float scrollStep = 100.f;

		ScrollToHeight(GetScrollHeight() - scrollStep * delta);
	}

	void ScrollAreaWidget::UpdateScrollbarStatus(ScrollBarStatus status)
	{
		if (m_scrollbarStatus != status)
		{
			Nz::Color newColor;
			switch (status)
			{
				case ScrollBarStatus::Grabbed: newColor = Nz::Color(235, 235, 235); break;
				case ScrollBarStatus::Hovered: newColor = Nz::Color(152, 152, 152); break;
				case ScrollBarStatus::None:    newColor = Nz::Color(104, 104, 104); break;
			}

			m_scrollbarSprite->SetColor(newColor);
			m_scrollbarStatus = status;
		}
	}
}

#endif
