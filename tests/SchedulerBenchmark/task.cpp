#include "task.hpp"
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Ray.hpp>


bool HitSphere(const Nz::Vector3d& center, double radius, const Nz::Rayd& r)
{
	Nz::Vector3d oc = r.origin - center;
	double a = Nz::Vector3d::DotProduct(r.direction, r.direction);
	double b = 2.0 * Nz::Vector3d::DotProduct(oc, r.direction);
	double c = Nz::Vector3d::DotProduct(oc, oc) - radius * radius;
	double discriminant = b * b - 4 * a * c;
	return (discriminant >= 0);
}

Nz::Color RayColor(const SceneData& sceneData, const Nz::Rayd& r)
{
	double closestHit = std::numeric_limits<double>::infinity();
	const Nz::Sphered* closestSphere = nullptr;
	for (const Nz::Sphered& sphere : sceneData.spheres)
	{
		double t;
		if (r.Intersect(sphere, &t, nullptr) && t < closestHit)
		{
			closestHit = t;
			closestSphere = &sphere;
		}
	}

	if (closestSphere)
	{
		Nz::Vector3d hitPos = r.GetPoint(closestHit);
		Nz::Vector3d hitNormal = Nz::Vector3d::Normalize(hitPos - closestSphere->GetPosition());
		Nz::Vector3d normalAsColor = (hitNormal + Nz::Vector3d(1.0)) / 2.0;
		return Nz::Color((float) normalAsColor.x, (float)normalAsColor.y, (float)normalAsColor.z);
	}
	else
	{
		Nz::Vector3d unitDirection = Nz::Vector3d::Normalize(r.direction);
		float a = static_cast<float>(0.5 * (unitDirection.y + 1.0));
		return (1.f - a) * Nz::Color(1.0, 1.0, 1.0) + a * Nz::Color(0.5, 0.7, 1.0);
	}
}

void RayCast(SceneData& sceneData, const Nz::Vector2ui& origin, const Nz::Vector2ui& dims)
{
	for (std::size_t j = 0; j < dims.y; ++j)
	{
		std::size_t y = origin.y + j;
		for (std::size_t i = 0; i < dims.x; ++i)
		{
			std::size_t x = origin.x + i;
			Nz::Vector3d pixelCenter = sceneData.originPos + (double(x) * sceneData.pixelDeltaU) + (double(y) * sceneData.pixelDeltaV);
			Nz::Vector3d rayDirection = pixelCenter - sceneData.cameraPos;
			Nz::Rayd ray(sceneData.cameraPos, rayDirection);
			Nz::Color color = RayColor(sceneData, ray);

			PixelColor& pixelColor = sceneData.pixels[y * sceneData.imageSize + x];
			color.ToRGB8(color, &pixelColor.r, &pixelColor.g, &pixelColor.b);
		}
	}
}
