#include "httplib.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

int main() 
{
	httplib::Server svr;
	svr.set_mount_point("/", "../../frontend/");
	
	httplib::Client cli("localhost", 8081);

    svr.Post("/api/prompt", [&](const httplib::Request& req, httplib::Response& res) {
        std::cout << "Received prompt: " << req.body << std::endl;
		
		
		auto backendRes = cli.Post("/query", req.body, "application/json");

        if (backendRes) 
		{
			res.status = backendRes->status;
            res.set_content(backendRes->body, "text/plain");
			std::cout << "[Webserver] Forwarded response from backend with status: " << backendRes->status 
			<< "\nBody: " << backendRes->body  << std::endl;
		} 
		else 
		{
            std::cout << "[Webserver] Error: Failed to reach backend on port 8081. Error Code: " << 
			res.status << std::endl;
            if (res.status == -1)
			{
				res.status = 502;
				res.set_content("Error communicating with backend storage.", "text/plain");
			} 
			else if (res.status == 404)
			{
				res.set_content("Account not found.", "text/plain");
			}
			else
			{
				res.set_content("Unknown error.", "text/plain");
			}
        }
    });

    std::cout << "Listening on port 8080..." << std::endl;
    svr.listen("0.0.0.0", 8080);

	return 0;
}
