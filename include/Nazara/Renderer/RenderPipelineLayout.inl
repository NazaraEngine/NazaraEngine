// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderPipelineLayout.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/String.hpp>
#include <limits>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RenderPipelineLayout::RenderPipelineLayout() :
	m_valid(false)
	{
	}

	inline RenderPipelineLayout::RenderPipelineLayout(RenderPipelineLayoutInfo pipelineLayoutInfo) :
	RenderPipelineLayout()
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);
		Create(std::move(pipelineLayoutInfo));
	}

	inline RenderPipelineLayout::~RenderPipelineLayout()
	{
	}

	inline bool RenderPipelineLayout::Create(RenderPipelineLayoutInfo pipelineLayoutInfo)
	{
		std::sort(pipelineLayoutInfo.bindings.begin(), pipelineLayoutInfo.bindings.end(), [](const auto& lhs, const auto& rhs) { return lhs.index < rhs.index; });

		unsigned int lastBindingIndex = std::numeric_limits<unsigned int>::max();
		for (const auto& binding : pipelineLayoutInfo.bindings)
		{
			if (binding.index == lastBindingIndex)
			{
				NazaraError("Duplicate binding index " + String::Number(binding.index));
				return false;
			}

			lastBindingIndex = binding.index;
		}

		m_layoutInfo = std::move(pipelineLayoutInfo);
		m_valid = true;

		return true;
	}

	inline void RenderPipelineLayout::Destroy()
	{
		m_valid = false;
	}

	inline const RenderPipelineLayoutInfo& RenderPipelineLayout::GetInfo() const
	{
		NazaraAssert(m_valid, "Invalid pipeline info");

		return m_layoutInfo;
	}

	inline bool RenderPipelineLayout::IsValid() const
	{
		return m_valid;
	}

	inline bool RenderPipelineLayout::operator==(const RenderPipelineLayout& rhs) const
	{
		if (m_valid != rhs.m_valid)
			return false;

		if (!m_valid)
			return true; //< Invalid pipeline are equivalent

		if (m_layoutInfo.bindings.size() != rhs.m_layoutInfo.bindings.size())
			return false;

		std::size_t bindingCount = m_layoutInfo.bindings.size();
		for (std::size_t i = 0; i < bindingCount; ++i)
		{
			const auto& lhsBinding = m_layoutInfo.bindings[i];
			const auto& rhsBinding = rhs.m_layoutInfo.bindings[i];

			if (lhsBinding.index != rhsBinding.index ||
			    lhsBinding.shaderStageFlags != rhsBinding.shaderStageFlags ||
			    lhsBinding.type != rhsBinding.type ||
			    lhsBinding.name != rhsBinding.name)
				return false;
		}

		return true;
	}

	inline bool RenderPipelineLayout::operator!=(const RenderPipelineLayout& rhs) const
	{
		return !operator==(rhs);
	}

	template<typename... Args>
	RenderPipelineLayoutRef RenderPipelineLayout::New(Args&&... args)
	{
		std::unique_ptr<RenderPipelineLayout> object(new RenderPipelineLayout(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
