// Copyright (C) 2019 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Widgets/TabBarWidget.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Utility/SimpleTextDrawer.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	namespace
	{
		Nz::Color selectedColor = Nz::Color::White;
		Nz::Color hoveredColor = Nz::Color(192, 192, 192);
		Nz::Color unselectedColor = Nz::Color(80, 80, 80);
	}

	TabBarWidget::TabBarWidget(BaseWidget* parent) :
	BaseWidget(parent),
	m_shouldAutoHide(false),
	m_hoveredTab(NoSelection),
	m_selectedTab(NoSelection),
	m_height(float(Nz::Font::GetDefault()->GetSizeInfo(18).lineHeight))
	{
		m_entity = CreateEntity();
		m_entity->AddComponent<Ndk::GraphicsComponent>();
		m_entity->AddComponent<Ndk::NodeComponent>().SetParent(this);
	}

	std::size_t TabBarWidget::AddTab(const Nz::String& text)
	{
		std::size_t tabIndex = m_tabs.size();

		float cursor = 0.f;
		if (!m_tabs.empty())
		{
			auto& lastTab = m_tabs.back();
			cursor = lastTab.origin + lastTab.backgroundSprite->GetSize().x;
		}

		m_tabs.emplace_back();
		auto& newTab = m_tabs.back(); //< C++17 when
		newTab.origin = cursor;
		newTab.textSprite = Nz::TextSprite::New();
		newTab.textSprite->Update(Nz::SimpleTextDrawer::Draw(text, 18, Nz::TextStyle_Regular, Nz::Color::Black));

		Nz::Boxf textBox = newTab.textSprite->GetBoundingVolume().obb.localBox;

		newTab.backgroundSprite = Nz::Sprite::New();
		newTab.backgroundSprite->SetColor(unselectedColor);
		newTab.backgroundSprite->SetSize({ textBox.width + 10.f, m_height });

		auto& gfxComponent = m_entity->GetComponent<Ndk::GraphicsComponent>();
		gfxComponent.Attach(newTab.backgroundSprite, Nz::Matrix4f::Translate({ cursor, 0.f, 0.f }), 0);
		gfxComponent.Attach(newTab.textSprite, Nz::Matrix4f::Translate({ cursor + 5.f, 0.f, 0.f }), 1);

		return tabIndex;
	}

	void TabBarWidget::EnableAutoHide(bool enable)
	{
		if (m_shouldAutoHide != enable)
		{
			m_shouldAutoHide = enable;

			if (ShouldHide())
			{
				m_entity->Disable();

				SetMinimumSize({0, 0});
				Layout();
			}
			else
			{
				m_entity->Enable();

				SetMinimumSize({ 64, 20 }); //< FIXME
				Layout();
			}
		}
	}

	void TabBarWidget::SelectTab(std::size_t tabIndex)
	{
		assert(tabIndex < m_tabs.size() || m_selectedTab == NoSelection);
		if (m_selectedTab != tabIndex)
		{
			if (m_selectedTab != NoSelection)
				m_tabs[m_selectedTab].backgroundSprite->SetColor(unselectedColor);

			m_selectedTab = tabIndex;
			if (m_selectedTab != NoSelection)
				m_tabs[m_selectedTab].backgroundSprite->SetColor(selectedColor);

			OnTabChanged(this, m_selectedTab);
		}
	}

	std::size_t TabBarWidget::GetHoveredTab(float x, float y) const
	{
		if (y > 0.f && y < m_height)
		{
			for (std::size_t i = 0; i < m_tabs.size(); ++i)
			{
				auto& tab = m_tabs[i];
				if (x > tab.origin)
				{
					float tabWidth = tab.backgroundSprite->GetSize().x;
					if (x < tab.origin + tabWidth)
						return i;
				}
			}
		}

		return NoSelection;
	}

	void TabBarWidget::Layout()
	{
		for (const auto& tabData : m_tabs)
		{

		}

		BaseWidget::Layout();
	}

	void TabBarWidget::OnMouseEnter()
	{
	}

	void TabBarWidget::OnMouseButtonPress(int x, int y, Nz::Mouse::Button button)
	{
		if (button != Nz::Mouse::Left)
			return;
	}

	void TabBarWidget::OnMouseButtonRelease(int x, int y, Nz::Mouse::Button button)
	{
		if (button != Nz::Mouse::Left)
			return;

		std::size_t hoveredTab = GetHoveredTab(float(x), float(y));
		if (hoveredTab != NoSelection)
			SelectTab(hoveredTab);
	}

	void TabBarWidget::OnMouseExit()
	{
	}

	void TabBarWidget::OnMouseMoved(int x, int y, int /*deltaX*/, int /*deltaY*/)
	{
		std::size_t hoveredTab = GetHoveredTab(float(x), float(y));
		if (hoveredTab != m_hoveredTab)
		{
			if (m_hoveredTab != NoSelection)
			{
				if (m_hoveredTab != m_selectedTab)
				{
					auto& hoveredTab = m_tabs[m_hoveredTab];
					hoveredTab.backgroundSprite->SetColor(unselectedColor);
				}
			}

			m_hoveredTab = hoveredTab;
			if (m_hoveredTab != NoSelection)
			{
				if (m_hoveredTab != m_selectedTab)
				{
					auto& hoveredTab = m_tabs[m_hoveredTab];
					hoveredTab.backgroundSprite->SetColor(hoveredColor);
				}
			}
		}
	}
}
