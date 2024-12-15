/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privMsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 12:49:50 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/15 15:30:53 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Chanel.hpp"

void Server::handlePrivMsg(int fd, Message &msg, std::vector<Chanel> &_chanel)
{
	int	index;

	index = 0;
	// Trouver le destinataire
	size_t spacePos = msg.getArgument().find(' ', 8);
	if (spacePos == std::string::npos)
	{
		std::string response = ERR_NORECIPIENT(std::string ("Server"), "");
		send(fd, response.c_str(), response.size(), 0);
		return ;
	}
	if (msg.getArgument().empty())
	{
    	std::cout << "Aucun argument dans le message" << std::endl;
    	return;
	}
	std::string recipient = get_next_argument(msg.getArgument().c_str(), index);
	if (recipient.empty())
	{
		std::string response = ERR_NORECIPIENT(std::string("Server"), "");
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	std::string message = get_next_argument(msg.getArgument().c_str(), index);
	if (recipient[0] == '#')
	{
		std::vector<Chanel>::iterator it_channel_to_send = find_channel_with_name(recipient, _chanel);
		if (it_channel_to_send == _chanel.end())
		{
			std::string response = ERR_NOSUCHCHANNEL(recipient);
			send(fd, response.c_str(), response.size(), 0);
			return ;
		}
		(*it_channel_to_send).sendMessageToChanel(fd, message);
		return ;
	}
	if (message.empty())
	{
		std::string response = ERR_NOTEXTTOSEND(std::string("Server"));
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	// Envoyer le message au destinataire
	bool recipientFound = false;
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getNickname() == recipient)
		{
			send(_clients[i].getFd(), message.c_str(), message.size(), 0);
			recipientFound = true;
			break;
		}
	}
	if (!recipientFound)
	{
		std::string response = ERR_NOSUCHNICK(std::string("Server"), recipient);
		send(fd, response.c_str(), response.size(), 0);
	}
}