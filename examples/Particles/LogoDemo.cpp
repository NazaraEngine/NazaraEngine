#include "LogoDemo.hpp"
#include <Nazara/Graphics.hpp>
#include <Nazara/Utility.hpp>
#include <NDK/Components.hpp>
#include <NDK/Systems.hpp>
#include <iostream>

namespace
{
	const float duration = 10.f;
	const float maxVel = 50.f;
	const float pauseTime = 3.f;
	const float startTime = 2.f;
	const float speed = 3.f;
}

struct SpriteController : public Nz::ParticleController
{
	void Apply(Nz::ParticleGroup& system, Nz::ParticleMapper& mapper, unsigned int startId, unsigned int endId, float elapsedTime) override
	{
		if (!enabled)
			return;

		auto posPtr = mapper.GetComponentPtr<Nz::Vector3f>(Nz::ParticleComponent_Position);
		auto velPtr = mapper.GetComponentPtr<Nz::Vector3f>(Nz::ParticleComponent_Velocity);

		for (unsigned int i = startId; i <= endId; ++i)
			posPtr[i] += velPtr[i] * elapsedTime * factor;
	}

	bool enabled = false;
	float factor = 1.f;
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

	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
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
}

void LogoExample::Enter(Ndk::StateMachine& fsm)
{
	ParticleDemo::Enter(fsm);

	m_shared.world3D->GetSystem<Ndk::RenderSystem>().SetDefaultBackground(nullptr);

	Nz::TextureRef backgroundTexture = Nz::Texture::New();
	if (backgroundTexture->LoadFromFile("resources/stars-background.jpg"))
		m_shared.world2D->GetSystem<Ndk::RenderSystem>().SetDefaultBackground(Nz::TextureBackground::New(std::move(backgroundTexture)));

	Ndk::EntityHandle particleGroupEntity = m_shared.world2D->CreateEntity();
	Ndk::ParticleGroupComponent& particleGroup = particleGroupEntity->AddComponent<Ndk::ParticleGroupComponent>(m_pixels.size(), Nz::ParticleLayout_Sprite);
	RegisterParticleGroup(particleGroupEntity);

	particleGroup.AddController(m_controller);
	particleGroup.SetRenderer(m_renderer);

	m_particles = static_cast<Nz::ParticleStruct_Sprite*>(particleGroup.CreateParticles(m_pixels.size()));
	ResetParticles(-duration * (speed / 2.f));

	m_accumulator = pauseTime + duration;
	m_totalAccumulator = 0.f;
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
	if (m_accumulator > pauseTime + 2.f * duration)
	{
		ResetParticles(0.f);
		m_accumulator = 0.f;
	}

	controller->enabled = (m_accumulator > pauseTime);
	controller->factor = -speed + speed * (m_accumulator - pauseTime) / (duration);

	return true;
}

void LogoExample::ResetParticles(float elapsed)
{
	Nz::Vector2f center = {m_shared.target->GetWidth() / 2.f, m_shared.target->GetHeight() / 2.f};
	Nz::Vector2f offset = center - Nz::Vector2f(Nz::Vector2ui(m_logo.GetSize()) / 2);

	float ratio = float(m_shared.target->GetWidth()) / m_shared.target->GetHeight();
	std::uniform_real_distribution<float> disX(-maxVel * ratio, maxVel * ratio);
	std::uniform_real_distribution<float> disY(-maxVel, maxVel);

	Nz::ParticleStruct_Sprite* sprite = m_particles;
	for (PixelData& data : m_pixels)
	{
		sprite->color = data.color;
		sprite->position = offset + Nz::Vector2f(data.pos);
		sprite->rotation = 0.f;
		sprite->velocity.Set(disX(m_shared.randomGen), disY(m_shared.randomGen), 0.f);
		sprite->position += sprite->velocity * elapsed;
		sprite++;
	}
}
