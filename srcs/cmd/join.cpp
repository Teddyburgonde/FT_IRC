/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:53:57 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/09 20:35:30 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

//This function return 1 if there is an error.
static int	check_active_mode(std::vector<Channel>::iterator &it_ChanExist, int fd, std::vector<Client> &_clients, std::string &arg_after_channel)
{
	if (it_ChanExist->getModeI() == true && !is_user_in_chan(fd, it_ChanExist->getInvitedUser()))
	{
		betterSend(ERR_INVITEONLYCHAN(find_nickname_with_fd(fd, _clients), it_ChanExist->getName()), fd);
		return (1);
	}
	if (it_ChanExist->getModeK() == true && (arg_after_channel.empty() || arg_after_channel != it_ChanExist->getPassword()))
	{
		betterSend(ERR_BADCHANNELKEY(find_nickname_with_fd(fd, _clients), it_ChanExist->getName()), fd);
		return (1);
	}
	if (it_ChanExist->getModeL() == true && it_ChanExist->get_nb_user_in() >= it_ChanExist->get_nb_user_max())
	{
		betterSend(ERR_CHANNELISFULL(find_nickname_with_fd(fd, _clients), it_ChanExist->getName()), fd);
		return (1);
	}
	return (0);
}

static std::string	find_arg_chan(const char *argument, int &index)
{
	bool	full_arg = false;
	int		start = index;

	while (argument[index] && ((argument[index] != ',' && argument[index] != ' ' && argument[index] != '\n' && argument[index] != '\r') || full_arg == true))
	{
		if (argument[index] == ':')
			full_arg = true;
		index++;
	}
	return(std::string(argument + start, argument + index));
}

//This function return all the channel name find.
static std::vector<std::string> create_chanName(const char *argument, int &i, int fd)
{
	std::vector<std::string>	chanName;
	std::vector<std::string>	error;
	int							f;

	while (argument[i] && argument[i] != ' ')
	{
		f = i;
		if ((i == 0 && argument[0] == '#') || (i > 0 && argument[i - 1] == ',' && argument[i] == '#'))
			chanName.push_back(find_arg_chan(argument, f));
		else if (i == 0)
		{
			betterSend(ERR_NOSUCHCHANNEL((std::string)argument), fd);
			return (error);
		}
		i++;
	}
	return (chanName);
}

void handleJoin(int fd, Message &msg, std::vector<Channel> &_channel, std::vector<Client> &_clients)
{
    std::vector<Channel>::iterator 				it_ChanExist;
    std::vector<std::string>::const_iterator	it_chanNew;
	std::string									tmp = msg.getArgument();
    const char 									*argument = tmp.c_str();
    int											i = 0;
    const std::vector<std::string>				&chanName = create_chanName(argument, i, fd);
    std::string									arg_after_channel = get_next_argument(argument, i);
    Client										user_sender = find_it_client_with_fd(fd, _clients);

    if (chanName.empty())
		return;
    it_chanNew = chanName.begin();
    while (it_chanNew != chanName.end())
    {
		it_ChanExist = _channel.begin();
		while (it_ChanExist != _channel.end())
		{
			if (it_ChanExist->getName() == *it_chanNew)
				break;
			++it_ChanExist;
		}
		if (it_ChanExist == _channel.end())
		{
			Channel newChan;
			newChan.setName(*it_chanNew);
			newChan.addUser(fd, true);
			_channel.push_back(newChan);
			betterSend(RPL_JOIN(CLIENT(user_sender.getNickname(), user_sender.getUsername()), newChan.getName()), fd);
		}
		else if (is_user_in_chan(fd, it_ChanExist->getUserInChannel()) == 0 && check_active_mode(it_ChanExist, fd, _clients, arg_after_channel) == 0)
		{
			it_ChanExist->addUser(fd, false);
			std::string message = RPL_JOIN(CLIENT(user_sender.getNickname(), user_sender.getUsername()), it_ChanExist->getName());
			it_ChanExist->sendMessageToChannel(-1, message);
		}
		++it_chanNew;
	}
}
