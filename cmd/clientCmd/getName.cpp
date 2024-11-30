#include "include/Client.hpp"
#include "include/Server.hpp"

std::string	Client::getNickname()
{
	return(this->_nickname);
}

std::string	Client::getUsername()
{
	return(this->_username);
}
