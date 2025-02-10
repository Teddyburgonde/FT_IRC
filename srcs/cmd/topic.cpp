/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 12:36:39 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/10 16:53:07 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

Channel* Server::findChannel(const std::string &ChannelName)
{
	for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end(); ++it)
	{
		if (it->getName() == ChannelName)
			return &(*it);
	}
	return (NULL);
}

void Server::handleTopic(int fd, const Message &msg)
{
	int			index;
	std::string channel = get_next_argument(msg.getArgument().c_str(), index);
	std::string newTopic;
	std::string notification;
	std::string	clientMsg;

	clientMsg = CLIENT(find_nickname_with_fd(fd, this->_clients), find_username_with_fd(fd, this->_clients));
	if (channel.empty())
	{
		betterSend(ERR_NEEDMOREPARAMS(clientMsg, "TOPIC"), fd);
		return;
	}
	Channel* targetChannel = findChannel(channel);
	if (!targetChannel)
	{
		betterSend(ERR_NOSUCHCHANNEL(channel), fd);
		return;
	}
	if (!isSenderInChannel(fd, *targetChannel))
	{
		betterSend(ERR_NOTONCHANNEL(clientMsg, channel), fd);
		return;
	}
	newTopic = get_next_argument(msg.getArgument().c_str(), index);
	if (newTopic.empty())
	{
		if (!targetChannel->getTopic().empty())
			betterSend(RPL_SEETOPIC(clientMsg, targetChannel->getName(), targetChannel->getTopic()), fd);
		else
			betterSend(RPL_NOTOPIC(clientMsg, targetChannel->getName()), fd);
		return;
	}
	if (!is_user_in_chan(fd, targetChannel->getOperatorUser()) && targetChannel->getModeT())
	{
		betterSend(ERR_CHANOPRIVSNEEDED(clientMsg, targetChannel->getName()), fd);
		return;
	}
	targetChannel->setTopic(newTopic);
	notification = RPL_SEETOPIC(clientMsg, targetChannel->getName(), targetChannel->getTopic());
	targetChannel->sendMessageToChannel(-1, notification);
}