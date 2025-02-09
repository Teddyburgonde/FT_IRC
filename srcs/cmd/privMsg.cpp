/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privMsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 17:11:46 by gmersch           #+#    #+#             */
/*   Updated: 2025/02/09 17:31:45 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

void Server::handlePrivMsg(int fd, Message &msg, std::vector<Channel> &_channel)
{
	int			index;
	std::string	target;
	std::string response;
	std::string message;
	std::string senderNickname = find_nickname_with_fd(fd, _clients);
	std::string senderUsername = find_username_with_fd(fd, _clients);

	index = 0;
	if (msg.getArgument().empty())
	{
    	betterSend(ERR_NEEDMOREPARAMS(CLIENT(find_nickname_with_fd(fd, this->_clients), find_username_with_fd(fd, this->_clients)), "PRIVMSG"), fd);
    	return;
	}
	target = get_next_argument(msg.getArgument().c_str(), index);
	if (target.empty())
	{
		//!PAS BONNE ERREUR
		response = ERR_NORECIPIENT(std::string("Server"), "");//!A changer par CLIENT
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	message = get_next_argument(msg.getArgument().c_str(), index);
	message += "\n";
	if (message.empty())
	{
		response = ERR_NOTEXTTOSEND(std::string("Server"));//!A changer par CLIENT
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	if (target[0] == '#')
	{
		std::vector<Channel>::iterator it_channel_to_send = find_channel_with_name(target, _channel);
		if (it_channel_to_send == _channel.end())
		{
			response = ERR_NOSUCHCHANNEL(target);
			send(fd, response.c_str(), response.size(), 0);
			return;
		}
		if (!is_user_in_chan(fd, it_channel_to_send->getUserInChannel()))
		{
			betterSend(ERR_NOTONCHANNEL(find_nickname_with_fd(fd, _clients), it_channel_to_send->getName()), fd);//!A changer par CLIENT
			return;
		}
		std::string IRCmessage = PRIVMSG(CLIENT(senderNickname, senderUsername), target, message);
		it_channel_to_send->sendMessageToChannel(fd, IRCmessage);
		return;
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getNickname() == target)
		{
			std::string IRCmessage = PRIVMSG(CLIENT(senderNickname, senderUsername), target, message);
			betterSend(IRCmessage, _clients[i].getFd());
			return;
		}
	}
	response = ERR_NOSUCHNICK(std::string("Server"), target);//!A changer par CLIENT
	send(fd, response.c_str(), response.size(), 0);
}
