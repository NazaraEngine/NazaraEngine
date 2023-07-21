// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RenderFrame::RenderFrame() :
	RenderFrame(nullptr, false, Vector2ui::Zero(), 0)
	{
	}

	inline RenderFrame::RenderFrame(RenderImage* renderImage, bool framebufferInvalidation, const Vector2ui& size, std::size_t framebufferIndex) :
	m_framebufferIndex(framebufferIndex),
	m_image(renderImage),
	m_size(size),
	m_framebufferInvalidation(framebufferInvalidation)
	{
	}

	inline void RenderFrame::Execute(const FunctionRef<void(CommandBufferBuilder& builder)>& callback, QueueTypeFlags queueTypeFlags)
	{
		if NAZARA_UNLIKELY(!m_image)
			throw std::runtime_error("frame is either invalid or has already been presented");

		return m_image->Execute(callback, queueTypeFlags);
	}

	inline std::size_t RenderFrame::GetFramebufferIndex() const
	{
		return m_framebufferIndex;
	}

	inline const Vector2ui& RenderFrame::GetSize() const
	{
		return m_size;
	}

	inline UploadPool& RenderFrame::GetUploadPool()
	{
		if NAZARA_UNLIKELY(!m_image)
			throw std::runtime_error("frame is either invalid or has already been presented");

		return m_image->GetUploadPool();
	}

	inline bool RenderFrame::IsFramebufferInvalidated() const
	{
		return m_framebufferInvalidation;
	}

	template<typename T>
	void RenderFrame::PushForRelease(T&& value)
	{
		if NAZARA_UNLIKELY(!m_image)
			throw std::runtime_error("frame is either invalid or has already been presented");

		m_image->PushForRelease(std::forward<T>(value));
	}

	template<typename F>
	void RenderFrame::PushReleaseCallback(F&& releaseCallback)
	{
		if NAZARA_UNLIKELY(!m_image)
			throw std::runtime_error("frame is either invalid or has already been presented");

		m_image->PushReleaseCallback(std::forward<F>(releaseCallback));
	}

	inline void RenderFrame::Present()
	{
		if NAZARA_UNLIKELY(!m_image)
			throw std::runtime_error("frame is either invalid or has already been presented");

		m_image->Present();
		m_image = nullptr;
	}

	inline void RenderFrame::SubmitCommandBuffer(CommandBuffer* commandBuffer, QueueTypeFlags queueTypeFlags)
	{
		if NAZARA_UNLIKELY(!m_image)
			throw std::runtime_error("frame is either invalid or has already been presented");

		m_image->SubmitCommandBuffer(commandBuffer, queueTypeFlags);
	}


	inline RenderFrame::operator bool()
	{
		return m_image != nullptr;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
