#include "httplib.h"
#include "database.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <cstdlib>

int main() 
{
	// Create an HTTP Web Server via cpp-httplib.
	httplib::Server backend_svr;
	
	// Get DataBase singleton instance.
	DataBase* DB = DataBase::getDataBase();

	// Receieve incoming requests and generate a response in the sub-web domain, query.
	backend_svr.Post("/query", [DB](const httplib::Request &req, httplib::Response &res)
	{
		std::cout << "[Database] Received Query: " << req.body << std::endl;
		
		// Give query to DataBase for handeling and receive a QueryResponse Object.
		QueryResponse QR = DB->handleQuery(req.body);

		// Set response status to the QueryReponse's statusCode.
		res.status = QR.statusCode;
		// Set the reponse body/content to the body of QueryResponse in the text/plain format.
		res.set_content(QR.body, "text/plain");
	});

    std::cout << "Listening on port 8081..." << std::endl;
	// Make Web Server listen on localhost:8081 for responses.
	backend_svr.listen("0.0.0.0", 8081);
	return 0;
}
