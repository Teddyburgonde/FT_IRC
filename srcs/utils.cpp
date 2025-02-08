/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 10:09:19 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/08 18:08:33 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Message.hpp"
#include "../include/Channel.hpp"

int skipSpaces(const char *str)
{
    int i;
	i = 0;
    while (str[i] && str[i] == ' ')
		i++;
	return (i);
}

//Permet de chercher dans une list de user, un user specifique. Renvoie 1 si le user est trouvé. UNE LIST, PAS CHAN
int	is_user_in_chan(int fd, std::vector<int> userInChannel)
{
	std::vector<int>::iterator it_userInChannelInChannel;

	it_userInChannelInChannel = userInChannel.begin();
	while (it_userInChannelInChannel != userInChannel.end())
	{
		if (*it_userInChannelInChannel == fd) //si le user est déjà dans la liste des gens dans le serveur
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

//utils ?? sert a trouver un channel a partir d'un nom
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
    while (line[index] && line[index] == ' ')
		index++;
	if (line[index] == ':')
	{
        full_arg = true;
		index++;
	}
	start = index;
    while (line[index] && line[index] != '\n' && line[index] != '\r' && (line[index] != ' ' || full_arg == true)) //en gros si c'est un espace ca s'arrete sauf si full_arg == true
		index++;
	if (index == start)
        return ("");
    return (std::string(line + start, line + index));
}

void	betterSend(std::string str, int fd)
{
	send(fd, str.c_str(), str.size(), 0);
}

int	find_fd_with_nickname(std::string &name, std::vector<Client> &_clients) // amettre dans utils ??
{
	std::vector<Client>::iterator	it = _clients.begin(); //iterator sur client

	while (it != _clients.end()) //on parcour tout les clients existant
	{
		if (name == it->getNickname()) //si le nom d'un client est le meme que celui donne en parametre
			return (it->getFd()); //on return le fd (int) du client.
		it++;
	}
	return (0);
}

Client	find_it_client_with_fd(int fd, std::vector<Client> &_clients) // amettre dans utils ??
{
	std::vector<Client>::iterator	it = _clients.begin(); //iterator sur client

	while (it != _clients.end()) //on parcour tout les clients existant
	{
		if (fd == it->getFd()) //si le nom d'un client est le meme que celui donne en parametre
			return (*it); //on return le fd (int) du client.
		it++;
	}
	return (*it);
}

//Sert a trouver un nickname a partir d'un fd donne. est l'inverse de find_fd_with_name
std::string	find_nickname_with_fd(int fd, std::vector<Client> &_clients)
{
	std::vector<Client>::iterator	it = _clients.begin(); //iterator sur client

	while (it != _clients.end()) //on parcour tout les clients existant
	{
		if (fd == it->getFd()) //si le nom d'un client est le meme que celui donne en parametre
			return (it->getNickname()); //on return le fd (int) du client.
		it++;
	}
	return ("");
}

std::string formatIrcMessage(const std::string &nickname, 
	const std::string &username,
	const std::string &host,
	const std::string &command,
	const std::string &target,
	const std::string &message)
{
    if (message.empty())
    {
        return ":" + nickname + "!" + username + "@" + host + " " + command + " " + target + "\r\n";
    }
    return ":" + nickname + "!" + username + "@" + host + " " + command + " " + target + " :" + message + "\r\n";
}
