#include "httplib.h"
#include <iostream>
#include <string>
#include <cstdlib>

int main() 
{
	const char* env_key = std::getenv("GADGET_LITE_API_KEY");
	if (!env_key) 
	{
		std::cerr << "Error: GADGET_LITE_API_KEY environment variable is not set!" << std::endl;
		return 1;
	}
	httplib::Server svr;
	svr.set_mount_point("/", "../../frontend/");

	httplib::Client cli("https://api.gadget-lite.stream");

	httplib::Headers headers = {
		{ "X-API-Key", env_key }
	};

	svr.Post("/api/prompt", [&](const httplib::Request& req, httplib::Response& res) 
	{
		std::cout << "Received prompt: " << req.body << std::endl;
		
		auto backendRes = cli.Post("/api/sql", headers, req.body, "application/json");

		if (backendRes && backendRes->status == 200) 
		{
			std::cout << "Success: " << backendRes->body << std::endl;
		} 
		else 
		{
			std::cout << "Failed with status: " << (backendRes ? backendRes->status : 0) << std::endl;

			if (backendRes) 
			{
				std::cout << "Response body: " << backendRes->body << std::endl;
			}
		}
	});

	std::cout << "Listening on port 8080..." << std::endl;
	svr.listen("0.0.0.0", 8080);

	return 0;
}
