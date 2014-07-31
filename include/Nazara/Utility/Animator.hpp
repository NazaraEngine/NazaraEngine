// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ANIMATOR_HPP
#define NAZARA_ANIMATOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <Nazara/Utility/Node.hpp>

class NAZARA_API NzAnimator
{
	public:
	    virtual ~NzAnimator() = default;

	    virtual nzAnimatorType GetType() const = 0;

	    virtual void Pause()
	    {
	        if (!m_paused)
                m_paused = true;
            else
                NazaraWarning("Animator is already paused, ignoring...");
	    }

        virtual void Unpause()
	    {
	        if (m_paused)
                m_paused = false;
            else
                NazaraWarning("Animator is not paused, ignoring...");
	    }

		virtual void Update(NzNode* sceneNode, float deltaTime) = 0;

    protected:
        NzAnimator() : m_paused(false) {}

        bool m_paused;
};

class NAZARA_API NzAnimatorCircle : public NzAnimator
{
	public:
	    NzAnimatorCircle(const NzVector3f& center = NzVector3f::Zero(), float radius = 1.f, float timeForOneTurn = 1.f,
            float initialTime = 0.f, const NzVector3f& direction = NzVector3f::UnitY(), float radiusEllipsoid = 0.f) :
            m_center(center), m_radius(radius), m_radiusEllipsoid(radiusEllipsoid), m_speed(timeForOneTurn), m_time(initialTime)
        {
            // If direction is k * UnitX(), the crossproduct is null.
            if ( !NzNumberEquals(direction.y, 0.f) || !NzNumberEquals(direction.z, 0.f) )
                m_vecV = NzVector3f::UnitX().CrossProduct(direction).Normalize();
            else
                m_vecV = NzVector3f::UnitY().CrossProduct(direction).Normalize();

            m_vecU = -m_vecV.CrossProduct(direction).Normalize(); // if direction = UnitY(), m_vecU = UnitX() and m_vecV = unitZ()
        }

        ~NzAnimatorCircle() = default;

	    nzAnimatorType GetType() const
	    {
	        return nzAnimatorType_Circle;
	    }

		void Update(NzNode* node, float deltaTime)
		{
            if (!node || m_paused)
                return;

            m_time += (deltaTime * ((2.f * M_PI) / m_speed));

            if (m_time >= 2.f * M_PI)
                m_time -= 2.f * M_PI;

            float r2 = m_radiusEllipsoid == 0.f ? m_radius : m_radiusEllipsoid;
            node->SetPosition(m_center + (m_radius * std::sin(m_time) * m_vecU) + (r2 * std::cos(m_time) * m_vecV));
		}

    private:
		NzVector3f m_center;
		NzVector3f m_vecU;
		NzVector3f m_vecV;
		float m_radius;
		float m_radiusEllipsoid;
		float m_speed;
        float m_time;
};

class NAZARA_API NzAnimatorLine : public NzAnimator
{
	public:
	    NzAnimatorLine(const NzVector3f& start = NzVector3f::Zero(), const NzVector3f& end = NzVector3f::UnitX(), float timeForOneGo = 1.f,
            float initialTime = 0.f, bool bounce = true) :
            m_end(end), m_start(start), m_bounce(bounce), m_speed(timeForOneGo), m_time(initialTime)
        {
            m_displacement = m_end - m_start;
            m_displacement /= m_speed;
        }

        ~NzAnimatorLine() = default;

	    nzAnimatorType GetType() const
	    {
	        return nzAnimatorType_Line;
	    }

		void Update(NzNode* node, float deltaTime)
		{
            if (!node || m_paused)
                return;

            m_time += deltaTime;

            if (!m_bounce)
            {
                if (m_time >= m_speed)
                    m_time -= m_speed;

                node->SetPosition(m_start + m_time * m_displacement);
            }
            else
            {
                if (m_time >= 2.f * m_speed)
                    m_time -= 2.f * m_speed;

                if (m_time >= m_speed)
                    node->SetPosition(m_end - (m_time - m_speed) * m_displacement);
                else
                    node->SetPosition(m_start + m_time * m_displacement);
            }
		}

    private:
		NzVector3f m_end;
		NzVector3f m_start;
		NzVector3f m_displacement;
        bool m_bounce;
		float m_speed;
        float m_time;
};

#endif // NAZARA_ANIMATOR_HPP
