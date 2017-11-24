#include "LogoDemo.hpp"
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Utility.hpp>
#include <NDK/Components.hpp>
#include <NDK/Systems.hpp>
#include <iostream>

namespace
{
	const float duration = 10.f;
	const float maxSpeed = 100.f;
	const float maxMouseForce = 1000.f;
	const float mouseForce = 500.f;
	const float pauseTime = 3.f;
	const float startTime = 2.f;
	const float speed = 3.f;
}

struct ParticleData
{
	Nz::Color color;
	Nz::Vector2f destination;
	Nz::Vector3f position;
	Nz::Vector2f velocity;
};

struct SpriteController : public Nz::ParticleController
{
	void Apply(Nz::ParticleGroup& system, Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId, float elapsedTime) override
	{
		if (!enabled)
			return;

		auto destPtr = mapper.GetComponentPtr<Nz::Vector2f>(Nz::ParticleComponent_Userdata0);
		auto posPtr = mapper.GetComponentPtr<Nz::Vector3f>(Nz::ParticleComponent_Position);
		auto velPtr = mapper.GetComponentPtr<Nz::Vector2f>(Nz::ParticleComponent_Velocity);

		std::uniform_real_distribution<float> dis(-1.f, 1.f);

		for (unsigned int i = startId; i <= endId; ++i)
		{
			Nz::Vector2f newVel = destPtr[i] - Nz::Vector2f(posPtr[i]);
			float length;
			newVel.Normalize(&length);

			float distance = SquaredDistancePointSegment(oldMousePos, actualMousePos, Nz::Vector2f(posPtr[i]));
			if (distance < 250.f)
			{
				Nz::Vector2f mouseLine = actualMousePos - oldMousePos;
				float mouseLength;
				mouseLine.Normalize(&mouseLength);
				if (mouseLength > 5.f)
				{
					velPtr[i] += mouseLine * std::min(mouseLength * mouseForce, maxMouseForce) * elapsedTime;
					velPtr[i] += Nz::Vector2f(dis(randomGen), dis(randomGen)) * std::min(mouseLength, maxMouseForce * 0.1f);
				}
			}

			if (length > 1.f || velPtr[i].GetSquaredLength() > Nz::IntegralPow(30.f, 2))
			{
				newVel *= maxSpeed;

				velPtr[i] = Nz::Lerp(velPtr[i], newVel, 0.4f * elapsedTime);
				posPtr[i] += velPtr[i] * elapsedTime;
			}
			else
			{
				velPtr[i] = Nz::Vector2f::Zero();
				posPtr[i] = destPtr[i];
			}
		}
	}

	static float SquaredDistancePointSegment(const Nz::Vector2f& s0, const Nz::Vector2f& s1, const Nz::Vector2f& point)
	{
		// http://geomalgorithms.com/a02-_lines.html
		Nz::Vector2f v = s1 - s0;
		Nz::Vector2f w = point - s0;

		float c1 = Nz::Vector2f::DotProduct(w, v);
		if (c1 <= 0.f)
			return point.SquaredDistance(s0);

		float c2 = Nz::Vector2f::DotProduct(v, v);
		if (c2 <= c1)
			return point.SquaredDistance(s1);

		float b = c1 / c2;
		Nz::Vector2f projPoint = s0 + b * v;
		return projPoint.SquaredDistance(point);
	}

	std::mt19937 randomGen;
	bool enabled = false;
	float factor = 1000.f;
	Nz::Vector2f actualMousePos;
	Nz::Vector2f oldMousePos;
};


class SpriteRenderer : public Nz::ParticleRenderer
{
	public:
		SpriteRenderer(Nz::MaterialRef mat) :
		m_material(mat)
		{
		}

		void Render(const Nz::ParticleGroup& system, const Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId, Nz::AbstractRenderQueue* renderQueue)
		{
			Nz::Vector2f size(1.f, 1.f);
			Nz::SparsePtr<const Nz::Vector2f> sizePtr(&size, 0);
			Nz::SparsePtr<const Nz::Vector2f> sinCosPtr(nullptr, 0);

			renderQueue->AddBillboards(0, m_material, endId - startId + 1, mapper.GetComponentPtr<const Nz::Vector3f>(Nz::ParticleComponent_Position), sizePtr, sinCosPtr, mapper.GetComponentPtr<const Nz::Color>(Nz::ParticleComponent_Color));
		}

	private:
		Nz::MaterialRef m_material;
};

LogoExample::LogoExample(ExampleShared& sharedData) :
ParticleDemo("Logo", sharedData)
{
	Nz::ImageParams params;
	params.loadFormat = Nz::PixelFormatType_RGBA8;

	if (!m_logo.LoadFromFile("resources/Logo.png", params))
		NazaraError("Failed to load logo!");

	unsigned int width = m_logo.GetWidth();
	unsigned int height = m_logo.GetHeight();
	m_pixels.reserve(width * height);

	for (unsigned int x = 0; x < width; ++x)
	{
		for (unsigned int y = 0; y < height; ++y)
		{
			Nz::Color color = m_logo.GetPixelColor(x, y);
			if (color.a == 0)
				continue;

			PixelData data;
			data.pos.Set(x, y);
			data.color = color;

			m_pixels.push_back(data);
		}
	}


	Nz::MaterialRef material = Nz::Material::New();
	material->EnableBlending(true);
	material->EnableDepthWrite(false);
	material->EnableFaceCulling(false);
	material->SetDstBlend(Nz::BlendFunc_InvSrcAlpha);
	material->SetSrcBlend(Nz::BlendFunc_SrcAlpha);

	m_controller = new SpriteController;
	m_renderer = new SpriteRenderer(std::move(material));

	m_declaration = Nz::ParticleDeclaration::New();
	m_declaration->EnableComponent(Nz::ParticleComponent_Color, Nz::ComponentType_Color, NazaraOffsetOf(ParticleData, color));
	m_declaration->EnableComponent(Nz::ParticleComponent_Position, Nz::ComponentType_Float3, NazaraOffsetOf(ParticleData, position));
	m_declaration->EnableComponent(Nz::ParticleComponent_Userdata0, Nz::ComponentType_Float2, NazaraOffsetOf(ParticleData, destination));
	m_declaration->EnableComponent(Nz::ParticleComponent_Velocity, Nz::ComponentType_Float2, NazaraOffsetOf(ParticleData, velocity));
}

void LogoExample::Enter(Ndk::StateMachine& fsm)
{
	ParticleDemo::Enter(fsm);

	m_shared.world3D->GetSystem<Ndk::RenderSystem>().SetDefaultBackground(nullptr);

	Nz::TextureRef backgroundTexture = Nz::Texture::New();
	if (backgroundTexture->LoadFromFile("resources/stars-background.jpg"))
		m_shared.world2D->GetSystem<Ndk::RenderSystem>().SetDefaultBackground(Nz::TextureBackground::New(std::move(backgroundTexture)));

	Ndk::EntityHandle particleGroupEntity = m_shared.world2D->CreateEntity();
	Ndk::ParticleGroupComponent& particleGroup = particleGroupEntity->AddComponent<Ndk::ParticleGroupComponent>(m_pixels.size(), m_declaration);
	RegisterParticleGroup(particleGroupEntity);

	particleGroup.AddController(m_controller);
	particleGroup.SetRenderer(m_renderer);

	m_particles = particleGroup.CreateParticles(m_pixels.size());
	ResetParticles(-duration * (speed / 2.f));

	m_accumulator = pauseTime + duration;
	m_totalAccumulator = 0.f;

	SpriteController* controller = static_cast<SpriteController*>(m_controller.Get());
	controller->actualMousePos = controller->oldMousePos = Nz::Vector2f(Nz::Mouse::GetPosition(*m_shared.target));
}

void LogoExample::Leave(Ndk::StateMachine & fsm)
{
	ParticleDemo::Leave(fsm);
}

bool LogoExample::Update(Ndk::StateMachine& fsm, float elapsedTime)
{
	if (!ParticleDemo::Update(fsm, elapsedTime))
		return false;

	m_totalAccumulator += elapsedTime;
	if (m_totalAccumulator <= startTime)
		return true;

	m_accumulator += elapsedTime;

	SpriteController* controller = static_cast<SpriteController*>(m_controller.Get());
	controller->enabled = (m_accumulator > pauseTime);

	if (m_mouseClock.GetMilliseconds() > 1000/30)
	{
		m_mouseClock.Restart();

		controller->oldMousePos = controller->actualMousePos;
		controller->actualMousePos = Nz::Vector2f(Nz::Mouse::GetPosition(*m_shared.target));
	}

	if (Nz::Mouse::IsButtonPressed(Nz::Mouse::Left))
	{
		if (!m_hasClicked)
		{
			m_hasClicked = true;
			std::uniform_real_distribution<float> dis(50.f, 60.f);

			ParticleData* sprite = static_cast<ParticleData*>(m_particles);
			for (std::size_t i = 0; i < m_pixels.size(); ++i)
			{
				Nz::Vector2f particleToMouse = Nz::Vector2f(sprite[i].position) - controller->actualMousePos;
				float sqDist = particleToMouse.GetSquaredLength();
				if (sqDist < 10000.f)
				{
					float dist = std::sqrt(sqDist);
					particleToMouse /= std::max(dist, 1.f);

					sprite[i].velocity += particleToMouse * dis(m_shared.randomGen);
				}
			}
		}
	}
	else
		m_hasClicked = false;

	return true;
}

void LogoExample::ResetParticles(float elapsed)
{
	Nz::Vector2ui size = m_shared.target->GetSize();

	Nz::Vector2f center = {size.x / 2.f, size.y / 2.f};
	Nz::Vector2f offset = center - Nz::Vector2f(Nz::Vector2ui(m_logo.GetSize()) / 2);

	std::uniform_real_distribution<float> disX(0.f, float(size.x));
	std::uniform_real_distribution<float> disY(-float(size.y) * 0.5f, float(size.y) * 1.5f);

	ParticleData* sprite = static_cast<ParticleData*>(m_particles);
	for (PixelData& data : m_pixels)
	{
		sprite->color = data.color;
		sprite->destination = offset + Nz::Vector2f(data.pos);
		sprite->position.Set(disX(m_shared.randomGen) - float(size.x), disY(m_shared.randomGen), 0.f);
		sprite->velocity = Nz::Vector2f::Zero();
		sprite++;
	}
}
