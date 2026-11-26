#ifndef DATATYPES_H
#define DATATYPES_H

#include <string>

struct RequestResponse
{
	std::string body = "";
	int code = 0;
};

class User
{
	private:
		std::string username_;
		std::string password_;
		std::string firstName_;
		std::string lastName_;
	public:
		User(std::string username, std::string password, std::string firstName, std::string lastName) :
		username_(username), password_(password), firstName_(firstName), lastName_(lastName) {};
		virtual RequestResponse login();
		virtual RequestResponse create();
};

class Admin : public User
{
	private:
		int adminID_;
	public:
		RequestResponse login();
		RequestResponse create();
		RequestResponse addMeme(Meme meme);
		RequestResponse removeMeme(int memeID);
};

class Patron : public User
{
	private:
		int patronID_;
		ShoppingCart cart_;
		PurchaseHistory history_;
	public:
		RequestResponse login();
		RequestResponse create();
		RequestResponse getCart();
		RequestResponse getHistory;
};


#endif
