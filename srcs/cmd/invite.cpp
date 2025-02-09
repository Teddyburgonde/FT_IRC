/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 17:02:27 by gmersch           #+#    #+#             */
/*   Updated: 2025/02/09 18:32:57 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

void	inviteCommand(int fd, Message &msg, std::vector<Channel> &_channel, std::vector<Client> &_clients)
{
	std::vector<Channel>::iterator	it_channel = _channel.begin();
	std::string						argument;
	std::string						channelName;
	std::string						nick_of_sender;
	std::string						user_of_sender;
	std::string						nameInvite;
	int								fdUserToInvite;
	int								i;

	i = 0;
	argument = msg.getArgument();
	nick_of_sender = find_nickname_with_fd(fd, _clients);
	user_of_sender = find_username_with_fd(fd, _clients);
	channelName = get_next_argument(argument.c_str(), i);
	nameInvite = get_next_argument(argument.c_str(), i);
	fdUserToInvite = find_fd_with_nickname(nameInvite, _clients);
	if (fdUserToInvite == 0)
	{
		betterSend(ERR_NOSUCHNICK(nick_of_sender, nameInvite), fd);
		return;
	}
	while (it_channel != _channel.end() && (*it_channel).getName() != channelName)
		it_channel++;
	if (channelName.empty() || channelName[0] != '#' || it_channel == _channel.end())
	{
		betterSend(ERR_NOSUCHCHANNEL(channelName), fd);
		return;
	}
	if (is_user_in_chan(fd, (*it_channel).getUserInChannel()) == false)
	{
		betterSend(ERR_USERNOTINCHANNEL(nick_of_sender, nick_of_sender, channelName), fd);
		return;
	}
	if (is_user_in_chan(fd, (*it_channel).getOperatorUser()) == false)
	{
		betterSend(ERR_CHANOPRIVSNEEDED(nick_of_sender, channelName), fd);
		return;
	}
	it_channel->setInvitedUser(fdUserToInvite);
	betterSend(RPL_INVITERCVR(CLIENT(nick_of_sender, user_of_sender), nameInvite, channelName), fdUserToInvite);
	betterSend(RPL_INVITESNDR(CLIENT(nick_of_sender, user_of_sender), nameInvite, channelName), fd);
}
