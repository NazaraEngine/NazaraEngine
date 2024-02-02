#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <memory>

// Don't use Nz::Color as it's 4 floats and may get us memory-bound
struct PixelColor
{
	Nz::UInt8 r, g, b;
};

struct SceneData
{
	std::size_t imageSize;
	std::unique_ptr<PixelColor[]> pixels;
	std::vector<Nz::Sphered> spheres;
	Nz::Vector3d cameraPos;
	Nz::Vector3d originPos;
	Nz::Vector3d pixelDeltaU;
	Nz::Vector3d pixelDeltaV;
};

void RayCast(SceneData& sceneData, const Nz::Vector2ui& origin, const Nz::Vector2ui& dims);
