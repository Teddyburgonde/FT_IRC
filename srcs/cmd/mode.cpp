/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/09 19:05:16 by gmersch           #+#    #+#             */
/*   Updated: 2025/02/10 14:58:34 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

static void	handle_modeK(std::string argument, int fd, bool is_plus, Channel &it_channel, std::string nickname)
{
	Server server;
	std::string	password;
	int			i;
	
	i = 0;
	if (is_plus)
	{
		password = get_next_argument(argument.c_str(), i);
		if (password.empty())
		{
			betterSend(ERR_NEEDMOREPARAMS(nickname, "MODE"), fd);
			return;
		}
		it_channel.setPassword(argument);
	}
	else
		it_channel.setPassword("");
	it_channel.setModeK(is_plus);
}

static void	handle_modeL(std::string argument, int fd, bool is_plus, Channel &it_channel, std::string nickname)
{
	std::string	nb_user_max;
	Server server;
	int i = 0;
	int number;

	if (is_plus)
	{
		nb_user_max = get_next_argument(argument.c_str(), i);
		if (nb_user_max.empty())
		{
			betterSend(ERR_NEEDMOREPARAMS(nickname, "MODE"), fd);
			return;
		}
		std::stringstream ss(nb_user_max);
		if (!(ss >> number) || !ss.eof())
		{
			betterSend(ERR_TOOMUCHPARAMS(nickname, "MODE"), fd);
			return;
		}
		it_channel.set_nb_user_max(number);
	}
	it_channel.setModeL(is_plus);
}

static void handle_modeO(std::string argument, int fd, bool is_plus, Channel &it_channel, std::string nickname, std::vector<Client> &_clients)
{
	std::string	new_operator_name;
	std::string	mode;
	int			i;
	
	i = 0;
	if (is_plus == true)
		mode = "+o";
	else
		mode = "-o";
	new_operator_name = get_next_argument(argument.c_str(), i);
	if(new_operator_name.empty())
	{
		betterSend(ERR_NEEDMOREPARAMS(nickname, "MODE"), fd);
		return;
	}
	int fd_new_op = find_fd_with_nickname(new_operator_name, _clients);
	if (fd_new_op == 0)
	{
		betterSend(ERR_NOSUCHNICK(nickname, new_operator_name), fd);
		return;
	}
	if (!is_user_in_chan(find_fd_with_nickname(new_operator_name, _clients), it_channel.getUserInChannel()))
	{
		betterSend(ERR_USERNOTINCHANNEL(nickname, new_operator_name, it_channel.getName()), fd);
		return;
	}
	it_channel.addUser(find_fd_with_nickname(new_operator_name, _clients), is_plus);
	std::string username_sender = find_username_with_fd(fd, _clients);
	std::string message = RPL_MODE(CLIENT(nickname, username_sender), it_channel.getName(), mode, new_operator_name);
	it_channel.sendMessageToChannel(-1, message);
}

static void	find_mode(std::string &mode, std::vector<Channel>::iterator it_channel, std::string nickname_of_sender, int fd, std::string &argument, std::vector<Client> &_clients)
{
	bool is_plus;
	Server server;

	if (mode[0] == '-')
		is_plus = false;
	else
		is_plus = true;
	if (mode[1] == 'i') 
		(*it_channel).setModeI(is_plus);
	else if (mode[1] == 't')
		(*it_channel).setModeT(is_plus);
	else if (mode[1] == 'k')
		handle_modeK(argument, fd, is_plus, (*it_channel), nickname_of_sender);
	else if (mode[1] == 'o')
		handle_modeO(argument, fd, is_plus, (*it_channel), nickname_of_sender, _clients);
	else if (mode[1] == 'l')
		handle_modeL(argument, fd, is_plus, (*it_channel), nickname_of_sender);
	else
		betterSend(ERR_UNKNOWNMODE(nickname_of_sender, mode[1]), fd);
}

void	Server::modeCommand(int fd, Message &msg)
{
	std::vector<Channel>::iterator	it_channel = _channel.begin();
	std::string						str_string = msg.getArgument();
	const char						*line = str_string.c_str();
	std::string						nickname_of_sender;
	std::string						argument;
	std::string						channelName;
	std::string						mode;
	int								index;

	
	index = 0;
	nickname_of_sender = find_nickname_with_fd(fd, _clients);
	channelName = get_next_argument(line, index);
	if (channelName.empty() || channelName[0] != '#')
	{
		betterSend(ERR_NOSUCHCHANNEL(channelName), fd);
		return;
	}
	it_channel = find_channel_with_name(channelName, _channel);
	if (it_channel == _channel.end())
	{
		betterSend(ERR_NOSUCHCHANNEL(channelName), fd);
		return;
	}
	if (is_user_in_chan(fd, (*it_channel).getOperatorUser()) == false)
	{
		betterSend(ERR_CHANOPRIVSNEEDED(nickname_of_sender, channelName), fd);
		return;
	}
	mode = get_next_argument(line, index);
	if (mode.empty() || (mode[0] != '+' && mode[0] != '-') || (mode.size() != 2))
	{
		betterSend(ERR_NEEDMOREPARAMS(nickname_of_sender, "MODE"), fd);
		return;
	}
	if (line[index] && line[index] != '\n')
		argument = get_next_argument(line, index);
	find_mode(mode, it_channel, nickname_of_sender, fd, argument, _clients);
}

