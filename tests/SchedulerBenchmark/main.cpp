#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Utility/Image.hpp>
#include <Nazara/Utility/Utility.hpp>
#include "task.hpp"
#include <iostream>
#include <mutex>
#include <random>

int main()
{
	Nz::Modules<Nz::Utility> core;

	constexpr unsigned int imageDimensions = 4096; // Will produce imageDimensions² rays
	constexpr unsigned int tileSize = 128;

	std::minstd_rand randEngine(std::random_device{}());
	std::uniform_real_distribution<float> posDis(-1.f, 1.f);
	std::uniform_real_distribution<float> velDis(-10.f, 10.f);

	Nz::TaskScheduler taskScheduler;

	std::cout << "Initializing..." << std::endl;

	// from https://raytracing.github.io/books/RayTracingInOneWeekend.html
	double focalLength = 1.0;
	double viewportHeight = 2.0;
	double viewportWidth = viewportHeight * (static_cast<double>(imageDimensions) / imageDimensions);
	Nz::Vector3d cameraCenter = Nz::Vector3d::Zero();

	Nz::Vector3d viewportU = Nz::Vector3d(viewportWidth, 0, 0);
	Nz::Vector3d viewportV = Nz::Vector3d(0, -viewportHeight, 0);
	Nz::Vector3d viewportUpperLeft = cameraCenter - Nz::Vector3d(0, 0, focalLength) - viewportU / 2 - viewportV / 2;

	SceneData sceneData;
	sceneData.imageSize = imageDimensions;
	sceneData.pixels = std::make_unique<PixelColor[]>(imageDimensions * imageDimensions * 3);
	sceneData.pixelDeltaU = viewportU / imageDimensions;
	sceneData.pixelDeltaV = viewportV / imageDimensions;
	sceneData.originPos = viewportUpperLeft + 0.5 * (sceneData.pixelDeltaU + sceneData.pixelDeltaV);
	sceneData.cameraPos = cameraCenter;
	sceneData.spheres.push_back(Nz::Sphered(Nz::Vector3d(-1.5, 0.5, -10.0), 0.5));
	sceneData.spheres.push_back(Nz::Sphered(Nz::Vector3d(0.0, 0.75, -2.5), 0.75));
	sceneData.spheres.push_back(Nz::Sphered(Nz::Vector3d(1.5, 0.5, -10.0), 0.5));

	std::cout << "Warming up..." << std::endl;
	RayCast(sceneData, Nz::Vector2ui(0, 0), Nz::Vector2ui(imageDimensions, imageDimensions));

	std::cout << "Measuring mono-threaded..." << std::endl;

	Nz::Time t1 = Nz::GetElapsedNanoseconds();
	RayCast(sceneData, Nz::Vector2ui(0, 0), Nz::Vector2ui(imageDimensions, imageDimensions));
	Nz::Time t2 = Nz::GetElapsedNanoseconds();

	std::cout << "Mono-threaded update time: " << (t2 - t1) << std::endl;

	std::cout << "Measuring task-scheduler..." << std::endl;

	unsigned int boxCount = imageDimensions / tileSize;
	if (imageDimensions % tileSize != 0)
		boxCount++;

	std::vector<std::pair<Nz::Vector2ui, Nz::Vector2ui>> boxes(boxCount * boxCount);
	for (unsigned int i = 0; i < boxes.size(); ++i)
	{
		unsigned int x = i % boxCount;
		unsigned int y = i % boxCount;

		Nz::Vector2ui mins(x * tileSize, y * tileSize);
		Nz::Vector2ui maxs = mins + Nz::Vector2ui(tileSize);
		maxs.Minimize(Nz::Vector2ui(imageDimensions));

		Nz::Vector2ui dims = maxs - mins;
		boxes[i] = std::make_pair(mins, dims);
	}

	Nz::Time t3 = Nz::GetElapsedNanoseconds();
	std::atomic_uint64_t acc = 0;
	std::atomic_uint boxCountAcc = 0;
	std::atomic_uint threadCounter = 0;

	for (auto&& [offset, dims] : boxes)
	{
		taskScheduler.AddTask([&, offset = offset, dims = dims]
		{
			thread_local std::size_t threadId = ++threadCounter;

			Nz::Time taskStart = Nz::GetElapsedNanoseconds();
			RayCast(sceneData, offset, dims);
			Nz::Time taskEnd = Nz::GetElapsedNanoseconds();

			acc += (taskEnd - taskStart).AsNanoseconds();
			boxCountAcc++;
		});
	}
	taskScheduler.WaitForTasks();

	Nz::Time t4 = Nz::GetElapsedNanoseconds();

	Nz::Time taskSchedulerTime = t4 - t3;
	std::cout << "task-scheduler update time: " << taskSchedulerTime << std::endl;
	std::cout << "accumulated task time: " << Nz::Time::Nanoseconds(acc) << std::endl;
	std::cout << "difference: " << taskSchedulerTime - Nz::Time::Nanoseconds(acc) << std::endl;
	std::cout << "thread count: " << threadCounter << std::endl;
	std::cout << "box count: " << boxCountAcc << std::endl;

	static_assert(sizeof(PixelColor) == 3 * sizeof(Nz::UInt8));

	Nz::Image image(Nz::ImageType::E2D, Nz::PixelFormat::RGB8, imageDimensions, imageDimensions);
	image.Update(sceneData.pixels.get());
	image.SaveToFile(Nz::Utf8Path("raycast_test.png"));
}
