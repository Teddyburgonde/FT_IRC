#include "include/Client.hpp"
#include "include/Server.hpp"

void	Client::setNickname(std::string newNickname)
{
	this->_nickname = newNickname;
}

void	Client::setUsername(std::string newUsername)
{
	this->_username = newUsername;
}

