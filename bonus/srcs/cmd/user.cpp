/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 17:55:54 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/11 17:03:58 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Server.hpp"
#include "../../include/Client.hpp"


// Validate the format of username
static bool isValidUsername(const std::string& user) 
{
	if (user.empty() || user.size() > 10)
		return (false);
	if (!std::isalpha(user[0]))
		return (false);
	for (size_t i = 0; i < user.size(); ++i) 
	{
		if (!std::isalnum(user[i]) && user[i] != '_')
			return (false);
	}
	return (true);
}

void Server::handleUser(int fd, std::string& user)
{
	std::string response;
	std::string clientMsg;

	clientMsg = CLIENT(find_nickname_with_fd(fd, this->_clients), find_username_with_fd(fd, this->_clients));
	if (!isValidUsername(user))
	{
		response = ERR_ERRONEUSUSERNAME(clientMsg, user);
		betterSend(response, fd);
		return;
	}
	if (find_fd_with_username(user, _clients))
	{
		betterSend(ERR_USERNAMEINUSE(clientMsg, user), fd);
		return;
	}
	for (size_t i = 0; i < this->_clients.size(); ++i) 
	{
		if (this->_clients[i].getFd() == fd && ! this->_clients[i].getUsername().empty())
		{
			response = ERR_ALREADYREGISTRED(clientMsg);
			betterSend(response, fd);
			return;
		}
	}
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i].getFd() == fd) 
		{
			_clients[i].setUsername(user);
			break;
		}
	}
	response = RPL_WELCOME(find_nickname_with_fd(fd, _clients));
	betterSend(response, fd);
}


