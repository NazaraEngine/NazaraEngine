// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VIEW_HPP
#define NAZARA_VIEW_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Graphics/AbstractViewer.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Renderer/RenderTarget.hpp>
#include <Nazara/Utility/Node.hpp>

class NAZARA_API NzView : public NzAbstractViewer, public NzNode, NzRenderTarget::Listener
{
	public:
		NzView();
		NzView(const NzVector2f& size);
		~NzView();

		void EnsureFrustumUpdate() const;
		void EnsureProjectionMatrixUpdate() const;
		void EnsureViewMatrixUpdate() const;
		void EnsureViewportUpdate() const;

		float GetAspectRatio() const;
		NzVector3f GetEyePosition() const;
		NzVector3f GetForward() const;
		const NzFrustumf& GetFrustum() const;
		NzVector3f GetGlobalForward() const;
		NzVector3f GetGlobalRight() const;
		NzVector3f GetGlobalUp() const;
		const NzMatrix4f& GetInvViewProjMatrix() const;
		const NzMatrix4f& GetProjectionMatrix() const;
		const NzVector2f& GetSize() const;
		const NzRenderTarget* GetTarget() const;
		const NzRectf& GetTargetRegion() const;
		const NzMatrix4f& GetViewMatrix() const;
		const NzMatrix4f& GetViewProjMatrix() const;
		const NzRecti& GetViewport() const;
		float GetZFar() const;
		float GetZNear() const;

		NzVector2f MapPixelToWorld(const NzVector2i& pixel);
		NzVector2i MapWorldToPixel(const NzVector2f& coords);

		void SetSize(const NzVector2f& size);
		void SetSize(float width, float height);
		void SetTarget(const NzRenderTarget* renderTarget);
		void SetTarget(const NzRenderTarget& renderTarget);
		void SetTargetRegion(const NzRectf& region);
		void SetViewport(const NzRecti& viewport);
		void SetZFar(float zFar);
		void SetZNear(float zNear);

	private:
		void ApplyView() const override;
		void InvalidateNode() override;

		void OnRenderTargetReleased(const NzRenderTarget* renderTarget, void* userdata) override;
		bool OnRenderTargetSizeChange(const NzRenderTarget* renderTarget, void* userdata) override;

		void UpdateFrustum() const;
		void UpdateInvViewProjMatrix() const;
		void UpdateProjectionMatrix() const;
		void UpdateViewMatrix() const;
		void UpdateViewProjMatrix() const;
		void UpdateViewport() const;

		mutable NzFrustumf m_frustum;
		mutable NzMatrix4f m_invViewProjMatrix;
		mutable NzMatrix4f m_projectionMatrix;
		mutable NzMatrix4f m_viewMatrix;
		mutable NzMatrix4f m_viewProjMatrix;
		NzRectf m_targetRegion;
		mutable NzRecti m_viewport;
		NzVector2f m_size;
		const NzRenderTarget* m_target;
		mutable bool m_frustumUpdated;
		mutable bool m_invViewProjMatrixUpdated;
		mutable bool m_projectionMatrixUpdated;
		mutable bool m_viewMatrixUpdated;
		mutable bool m_viewProjMatrixUpdated;
		mutable bool m_viewportUpdated;
		float m_zFar;
		float m_zNear;
};

#endif // NAZARA_VIEW_HPP
