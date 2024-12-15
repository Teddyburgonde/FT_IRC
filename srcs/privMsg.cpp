/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privMsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 12:49:50 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/15 14:02:17 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Client.hpp"

void Server::handlePrivMsg(int fd, const std::string& command)
{
	int	index;

	index = 0;
	// Trouver le destinataire
	size_t spacePos = command.find(' ', 8);
	if (spacePos == std::string::npos)
	{
		std::string response = ERR_NORECIPIENT(std::string ("Server"), "");
		send(fd, response.c_str(), response.size(), 0);
		return ;
	}
	//std::string get_next_argument(const char *line, int &index)
	std::string recipient = command.substr(8, spacePos - 8); // Extrait le destinataire
	//std::string recipient = get_next_argument(command.c_str(), index);
	//std::cout << "Valeur de index: " << index << std::endl;
	std::cout << "Valeur du command: " << command << std::endl;
	if (recipient.empty())
	{
		std::string response = ERR_NORECIPIENT(std::string("Server"), "");
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	if (recipient[0] == '#')
	{
		std::cout << "fonction de Galaad  " << recipient[0] << std::endl;
	}
	// Trouver le message après ":"
	size_t colonPos = command.find(':', spacePos);
	if (colonPos == std::string::npos)
	{
		std::string response = ERR_NOTEXTTOSEND(std::string("Server"));
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	std::string message = command.substr(colonPos + 1); // Tout après ":" est le message
	if (message.empty())
	{
		std::string response = ERR_NOTEXTTOSEND(std::string("Server"));
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	//std::string get_next_argument(const char *line, int &index)
	// std::cout << "Valeur du message: " << message << std::endl;
	// std::cout << "Valeur du message[0]: " << message[0] << std::endl;
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