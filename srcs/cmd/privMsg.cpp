/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privMsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 17:11:46 by gmersch           #+#    #+#             */
/*   Updated: 2025/02/10 16:52:10 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

void Server::handlePrivMsg(int fd, Message &msg)
{
	int			index;
	std::string	target;
	std::string response;
	std::string message;
	std::vector<Channel>::iterator it_channel_to_send;
	std::string clientMsg;
	std::string senderNickname;
	std::string senderUsername;

	senderNickname = find_nickname_with_fd(fd, _clients);
	senderUsername = find_username_with_fd(fd, _clients);
	clientMsg = CLIENT(senderNickname, senderUsername);
	index = 0;
	if (msg.getArgument().empty())
	{
    	betterSend(ERR_NEEDMOREPARAMS(clientMsg, "PRIVMSG"), fd);
    	return;
	}
	target = get_next_argument(msg.getArgument().c_str(), index);
	if (target.empty())
	{
		response = ERR_NEEDMOREPARAMS(clientMsg, "PRIVMSG");
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	message = get_next_argument(msg.getArgument().c_str(), index);
	message += "\n";
	if (message.empty())
	{
		response = ERR_NOTEXTTOSEND(clientMsg);
		send(fd, response.c_str(), response.size(), 0);
		return;
	}
	if (target[0] == '#')
	{
		it_channel_to_send = find_channel_with_name(target, _channel);
		if (it_channel_to_send == _channel.end())
		{
			response = ERR_NOSUCHCHANNEL(target);
			send(fd, response.c_str(), response.size(), 0);
			return;
		}
		if (!is_user_in_chan(fd, it_channel_to_send->getUserInChannel()))
		{
			betterSend(ERR_NOTONCHANNEL(clientMsg, it_channel_to_send->getName()), fd);
			return;
		}
		std::string IRCmessage = PRIVMSG(clientMsg, target, message);
		it_channel_to_send->sendMessageToChannel(fd, IRCmessage);
		return;
	}
	for (size_t i = 0; i < _clients.size(); i++)
	{
		if (_clients[i].getNickname() == target)
		{
			std::string IRCmessage = PRIVMSG(clientMsg, target, message);
			betterSend(IRCmessage, _clients[i].getFd());
			return;
		}
	}
	response = ERR_NOSUCHNICK(clientMsg, target);
	send(fd, response.c_str(), response.size(), 0);
}
