#include <Nazara/Core/Clock.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <Nazara/Network/Network.hpp>
#include <Nazara/Network/WebRequest.hpp>
#include <Nazara/Network/WebService.hpp>
#include <catch2/catch_test_macros.hpp>
#include <thread>

SCENARIO("WebService", "[NETWORK][WebService]")
{
	std::unique_ptr<Nz::WebService> webService = Nz::Network::Instance()->InstantiateWebService();
	auto WaitForRequest = [&]
	{
		Nz::MillisecondClock clock;
		while (!webService->Poll())
		{
			if (clock.GetElapsedTime() >= Nz::Time::Seconds(2))
			{
				INFO("WebRequest timed out");
				REQUIRE(false);
				break;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	};

	GIVEN("When performing a GET web request")
	{
		std::unique_ptr<Nz::WebRequest> webRequest = webService->CreateRequest(Nz::WebRequestMethod::Get, "https://test.digitalpulse.software", [&](const Nz::WebRequestResult& result)
		{
			REQUIRE(result);
			CHECK(result.GetStatusCode() == 200);
			CHECK(result.GetDownloadedSize() > 0);
			CHECK(result.GetDownloadSpeed() > 0);
			CHECK(result.GetBody() == "Hello Nazara from web!");
		});
		webService->QueueRequest(std::move(webRequest));

		WaitForRequest();
	}

	GIVEN("When performing a GET web request on a non-existing URL")
	{
		std::unique_ptr<Nz::WebRequest> webRequest = webService->CreateRequest(Nz::WebRequestMethod::Get, "https://test.digitalpulse.software/404", [&](const Nz::WebRequestResult& result)
		{
			REQUIRE(result);
			CHECK(result.GetStatusCode() == 404);
			CHECK(result.GetDownloadedSize() > 0);
			CHECK(result.GetDownloadSpeed() > 0);
		});
		webService->QueueRequest(std::move(webRequest));

		WaitForRequest();
	}

	GIVEN("When performing a POST web request")
	{
		std::unique_ptr<Nz::WebRequest> webRequest = webService->CreateRequest(Nz::WebRequestMethod::Post, "https://test.digitalpulse.software/headers", [&](const Nz::WebRequestResult& result)
		{
			REQUIRE(result);
			CHECK(result.GetStatusCode() == 200);
			CHECK(result.GetDownloadedSize() > 0);
			CHECK(result.GetDownloadSpeed() > 0);

			INFO(result.GetBody());
			CHECK(result.GetBody().find("uthorization: Lynix") != std::string::npos); //< Don't include "A" because some implementation can lowercase headers
		});
		webRequest->SetJSonContent(R"({"value":42})");
		webRequest->SetHeader("Authorization", "Lynix");

		webService->QueueRequest(std::move(webRequest));

		WaitForRequest();
	}
}
