// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Widgets module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Widgets/BoxLayout.hpp>
#include <Nazara/Core/Log.hpp>
#include <NazaraUtils/StackVector.hpp>
#include <kiwi/kiwi.h>
#include <cassert>
#include <vector>

namespace Nz
{
	struct BoxLayout::State
	{
		std::vector<kiwi::Variable> sizeVar;
		kiwi::Solver solver;
	};

	BoxLayout::BoxLayout(BaseWidget* parent, BoxLayoutOrientation orientation) :
	BaseWidget(parent),
	m_orientation(orientation),
	m_spacing(5.f)
	{
		m_state = std::make_unique<State>();
	}

	BoxLayout::~BoxLayout() = default;

	void BoxLayout::Layout()
	{
		BaseWidget::Layout();

		std::size_t axis = 0;
		std::size_t otherAxis = 1;
		bool reversed = false;

		switch (m_orientation)
		{
			case BoxLayoutOrientation::LeftToRight:
			case BoxLayoutOrientation::RightToLeft:
				axis = 0; //< x
				otherAxis = 1; //< y
				reversed = (m_orientation == BoxLayoutOrientation::RightToLeft);
				break;

			case BoxLayoutOrientation::BottomToTop:
			case BoxLayoutOrientation::TopToBottom:
				axis = 1; //< y
				otherAxis = 0; //< x
				reversed = (m_orientation == BoxLayoutOrientation::TopToBottom);
				break;
		}

		//TODO: Keep solver state when widgets don't change
		std::size_t widgetChildCount = GetVisibleWidgetChildCount();
		if (widgetChildCount == 0)
			return;

		m_state->solver.reset();

		m_state->sizeVar.clear();
		m_state->sizeVar.reserve(widgetChildCount);

		kiwi::Expression sizeSum;

		Nz::Vector2f layoutSize = GetSize();
		float maxLayoutSize = layoutSize[axis];
		float availableSpace = maxLayoutSize - m_spacing * (widgetChildCount - 1);
		float optimalSize = availableSpace / widgetChildCount;

		// Handle size
		ForEachWidgetChild([&](BaseWidget* child)
		{
			float maximumSize = child->GetMaximumSize()[axis];
			float minimumSize = child->GetMinimumSize()[axis];
			float preferredSize = child->GetPreferredSize()[axis];

			m_state->sizeVar.emplace_back();
			auto& sizeVar = m_state->sizeVar.back();

			m_state->solver.addConstraint({ (sizeVar >= minimumSize) | kiwi::strength::required });

			if (maximumSize < std::numeric_limits<float>::infinity() && maximumSize <= minimumSize)
				m_state->solver.addConstraint({ (sizeVar <= maximumSize) | kiwi::strength::required });

			if (preferredSize > 0.f)
			{
				m_state->solver.addConstraint({ (sizeVar >= preferredSize) | kiwi::strength::medium });
				m_state->solver.addConstraint({ (sizeVar >= maxLayoutSize) | kiwi::strength::weak });
			}
			else if (maximumSize < std::numeric_limits<float>::infinity() && maximumSize <= minimumSize)
			{
				m_state->solver.addConstraint({ (sizeVar >= maximumSize) | kiwi::strength::medium });
				m_state->solver.addConstraint({ (sizeVar >= maxLayoutSize) | kiwi::strength::weak });
			}
			else
				m_state->solver.addConstraint({ (sizeVar >= maxLayoutSize) | kiwi::strength::medium });

			m_state->solver.addConstraint({ (sizeVar >= optimalSize) | kiwi::strength::medium });

			sizeSum = sizeSum + sizeVar;
		});

		kiwi::Variable targetSize("LayoutSize");

		m_state->solver.addConstraint((sizeSum <= targetSize) | kiwi::strength::strong);

		m_state->solver.addEditVariable(targetSize, kiwi::strength::strong);
		m_state->solver.suggestValue(targetSize, availableSpace);

		m_state->solver.updateVariables();

		std::size_t varIndex = 0;

		float remainingSize = availableSpace;

		ForEachWidgetChild([&](BaseWidget* child)
		{
			Nz::Vector2f newSize = layoutSize;
			newSize[axis] = static_cast<float>(m_state->sizeVar[varIndex].value());

			child->Resize(newSize);
			remainingSize -= newSize[axis];

			varIndex++;
		});

		float spacing = m_spacing + remainingSize / (widgetChildCount - 1);

		// Handle position
		float cursor = (reversed) ? layoutSize[axis] : 0.f;
		ForEachWidgetChild([&](BaseWidget* child)
		{
			Vector2f childSize = child->GetSize();
			if (reversed)
				cursor -= childSize[axis];

			Nz::Vector2f position(0.f, 0.f);
			position[axis] = cursor;
			position[otherAxis] = layoutSize[otherAxis] / 2.f - childSize[otherAxis] / 2.f;

			child->SetPosition(position);

			if (reversed)
				cursor -= spacing;
			else
				cursor += childSize[axis] + spacing;
		});
	}

	void BoxLayout::OnChildAdded(const BaseWidget* /*child*/)
	{
		RecomputePreferredSize();
		Layout();
	}

	void BoxLayout::OnChildPreferredSizeUpdated(const BaseWidget* /*child*/)
	{
		Layout();
	}

	void BoxLayout::OnChildVisibilityUpdated(const BaseWidget* /*child*/)
	{
		RecomputePreferredSize();
		Layout();
	}

	void BoxLayout::OnChildRemoved(const BaseWidget* /*child*/)
	{
		RecomputePreferredSize();
		Layout();
	}

	void BoxLayout::RecomputePreferredSize()
	{
		std::size_t axis;
		std::size_t otherAxis;
		switch (m_orientation)
		{
			case BoxLayoutOrientation::LeftToRight:
			case BoxLayoutOrientation::RightToLeft:
				axis = 0;
				otherAxis = 1;
				break;

			case BoxLayoutOrientation::BottomToTop:
			case BoxLayoutOrientation::TopToBottom:
				axis = 1;
				otherAxis = 0;
				break;
		}

		float childCount = 0.f;

		Vector2f preferredSize;
		ForEachWidgetChild([&](const BaseWidget* child)
		{
			const Vector2f& childPreferredSize = child->GetPreferredSize();
			preferredSize[axis] += childPreferredSize[axis];
			preferredSize[otherAxis] = std::max(preferredSize[otherAxis], childPreferredSize[otherAxis]);

			childCount += 1.f;
		});

		if (childCount > 1.f)
			preferredSize[axis] += m_spacing * (childCount - 1.f);

		SetPreferredSize(preferredSize);
	}
}
