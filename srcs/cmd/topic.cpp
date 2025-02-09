/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/14 12:36:39 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/09 18:58:46 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

Channel* Server::findChannel(const std::string &ChannelName, std::vector<Channel> &_Channel)
{
	for (std::vector<Channel>::iterator it = _Channel.begin(); it != _Channel.end(); ++it)
	{
		if (it->getName() == ChannelName)
			return &(*it);
	}
	return (NULL);
}

void Server::handleTopic(int fd, const Message &msg, std::vector<Channel> &_Channel)
{
	int			index;
	std::string channel = get_next_argument(msg.getArgument().c_str(), index);
	std::string newTopic;
	std::string notification;

	if (channel.empty())
	{
		betterSend(ERR_NEEDMOREPARAMS(std::string("Server"), "TOPIC"), fd);//!A changer par CLIENT
		return;
	}
	Channel* targetChannel = findChannel(channel, _Channel);
	if (!targetChannel)
	{
		betterSend(ERR_NOSUCHCHANNEL(channel), fd);
		return;
	}
	if (!isSenderInChannel(fd, *targetChannel))
	{
		betterSend(ERR_NOTONCHANNEL(std::string("Server"), channel), fd);//!A changer par CLIENT
		return;
	}
	newTopic = get_next_argument(msg.getArgument().c_str(), index);
	if (newTopic.empty())
	{
		if (!targetChannel->getTopic().empty())
			betterSend(RPL_SEETOPIC(std::string("Server"), targetChannel->getName(), targetChannel->getTopic()), fd);//!A changer par CLIENT
		else
			betterSend(RPL_NOTOPIC(std::string("Server"), targetChannel->getName()), fd);//!A changer par CLIENT
		return;
	}
	if (!is_user_in_chan(fd, targetChannel->getOperatorUser()) && targetChannel->getModeT())
	{
		betterSend(ERR_CHANOPRIVSNEEDED(find_nickname_with_fd(fd, this->_clients), targetChannel->getName()), fd);//!A changer par CLIENT
		return;
	}
	targetChannel->setTopic(newTopic);
	//!on envoie a tout le monde le nouveau topic. Peux etre pas le bon message !
	notification = RPL_SEETOPIC(CLIENT(find_nickname_with_fd(fd, this->_clients), find_username_with_fd(fd, this->_clients)), targetChannel->getName(), targetChannel->getTopic());
	targetChannel->sendMessageToChannel(-1, notification);
}