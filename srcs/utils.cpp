/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 10:09:19 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/10 17:04:52 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Message.hpp"
#include "../include/Channel.hpp"


//Allows you to search a list of users for a specific user. Returns 1 if the user is found.
int	is_user_in_chan(int fd, std::vector<int> userInChannel)
{
	std::vector<int>::iterator it_userInChannelInChannel;

	it_userInChannelInChannel = userInChannel.begin();
	while (it_userInChannelInChannel != userInChannel.end())
	{
		if (*it_userInChannelInChannel == fd)
			return (1);
		it_userInChannelInChannel++;
	}
	return (0);
}

std::string find_username_with_fd(int fd, std::vector<Client> &_clients)
{
    for (size_t i = 0; i < _clients.size(); i++)
    {
        if (_clients[i].getFd() == fd)
        {
            return _clients[i].getUsername();
        }
    }
    return "";
}

// Allows you to find a specific channel starting with a channel name. Return an iterator to this channel.
std::vector<Channel>::iterator find_channel_with_name(std::string &channelName, std::vector<Channel> &_channel)
{
	std::vector<Channel>::iterator it_channel;

	it_channel = _channel.begin();
	while (it_channel != _channel.end())
	{
		if (it_channel->getName() == channelName)
			return (it_channel);
		it_channel++;
	}
	return (it_channel);
}

std::string get_next_argument(const char *line, int &index)
{
    bool    full_arg = false;
	int		start;

	if (!line)
        return ("");
	while (line[index] && line [index] == ' ')
		index++;
	if (line[index] == ':')
	{
        full_arg = true;
		index++;
	}
	start = index;
    while (line[index] && line[index] != '\n' && line[index] != '\r' && (line[index] != ' ' || full_arg == true))
		index++;
	if (index == start)
        return ("");
    return (std::string(line + start, line + index));
}

void	betterSend(std::string str, int fd)
{
	send(fd, str.c_str(), str.size(), 0);
}

int	find_fd_with_nickname(std::string &name, std::vector<Client> &_clients)
{
	std::vector<Client>::iterator	it = _clients.begin();

	while (it != _clients.end())
	{
		if (name == it->getNickname())
			return (it->getFd());
		it++;
	}
	return (0);
}

int	find_fd_with_username(std::string &name, std::vector<Client> &_clients)
{
	std::vector<Client>::iterator	it = _clients.begin();

	while (it != _clients.end())
	{
		if (name == it->getUsername())
			return (it->getFd());
		it++;
	}
	return (0);
}

Client	find_it_client_with_fd(int fd, std::vector<Client> &_clients)
{
	std::vector<Client>::iterator	it = _clients.begin();

	while (it != _clients.end())
	{
		if (fd == it->getFd())
			return (*it);
		it++;
	}
	return (*it);
}

// Used to find a nickname from a given fd. is the opposite of find_fd_with_name.
std::string	find_nickname_with_fd(int fd, std::vector<Client> &_clients)
{
	std::vector<Client>::iterator	it = _clients.begin();

	while (it != _clients.end())
	{
		if (fd == it->getFd())
			return (it->getNickname());
		it++;
	}
	return ("");
}