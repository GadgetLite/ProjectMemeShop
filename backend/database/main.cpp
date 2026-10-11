#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "utils.h"
#include <iostream>

int main() {
	httplib::SSLServer svr("../ssl_certs/cert.pem", "../ssl_certs/key.pem");
	
	if (!svr.is_valid()) 
	{
		std::cerr << "Failed to load SSL certificates!" << std::endl;
		return 1;
	}
	
	potato();
	yo();
	imagineDragons();
	svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
			res.set_content("  O>\n /|\\\n  |\n / \\", "text/plain");
			});

	std::cout << "Listening on https://localhost:8443" << std::endl;
	svr.listen("0.0.0.0", 8443);
	return 0;
}
