#include <Nazara/Core.hpp>
#include <Nazara/Network.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
	Nz::Application<Nz::Network> app(argc, argv);

	std::unique_ptr<Nz::WebService> webService = Nz::Network::Instance()->InstantiateWebService();

	std::unique_ptr<Nz::WebRequest> webRequest = webService->CreateRequest(Nz::WebRequestMethod::Get, "https://test.digitalpulse.software", [&](const Nz::WebRequestResult& result)
	{
		if (result)
		{
			std::cout << "Got a " << result.GetStatusCode() << " response(" << result.GetDownloadedSize() << " bytes, " << result.GetDownloadSpeed() << " bytes / s" << ")" << std::endl;
			std::cout << result.GetBody() << std::endl;
		}
		else
			std::cout << "Web request failed (code " << result.GetStatusCode() << "): " << result.GetErrorMessage() << std::endl;

		Nz::ApplicationBase::Instance()->Quit();
	});

	webService->QueueRequest(std::move(webRequest));

	app.AddUpdaterFunc([&] { webService->Poll(); });

	return app.Run();
}
