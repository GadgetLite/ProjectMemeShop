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
	
	svr.Post("/api/upload", [&](const httplib::Request& req, httplib::Response& res) 
	{
		std::cout << "Received prompt: " << req.body << std::endl;
		
	});
	svr.Post("/api/login", [&](const httplib::Request& req, httplib::Response& res) 
	{
		std::cout << "Received prompt: " << req.body << std::endl;
		
	});
	svr.Post("/api/signup", [&](const httplib::Request& req, httplib::Response& res) 
	{
		std::cout << "Received prompt: " << req.body << std::endl;
		
	});

	std::cout << "Listening on port 8080..." << std::endl;
	svr.listen("0.0.0.0", 8080);

	return 0;
}
