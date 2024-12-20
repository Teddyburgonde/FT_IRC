/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 10:09:19 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/15 16:40:11 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Message.hpp"
#include "../include/Chanel.hpp"

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

//utils ?? sert a trouver un channel a partir d'un nom
std::vector<Chanel>::iterator find_channel_with_name(std::string &channelName, std::vector<Chanel> &_chanel)
{
	std::vector<Chanel>::iterator it_channel;

	it_channel = _chanel.begin();
	while (it_channel != _chanel.end())
	{
		if ((*it_channel).getName() == channelName)
			return (it_channel);
		it_channel++;
	}
	return (it_channel);
}

std::string get_next_argument(const char *line, int &index)
{
    int        start;
    bool    full_arg = false;

	if (!line)
        return "";
    while (line[index] && line[index] == ' ')
        index++;
    start = index;
    while (line[index] && line[index] != '\n' && (line[index] != ' ' || full_arg == true)) //en gros si c'est un espace ca s'arrete sauf si full_arg == true
    {
        if (line[index] == ':')
            full_arg = true;
        index++;
    }
	if (start == index)
        return "";
    return (std::string(line + start, line + index));
}

void	send_error(std::string error, int fd)
{
	send(fd, error.c_str(), error.size(), 0);
}

int	find_fd_with_nickname(std::string &name, std::vector<Client> &_clients) // amettre dans utils ??
{
	std::vector<Client>::iterator	it = _clients.begin(); //iterator sur client

	while (it != _clients.end()) //on parcour tout les clients existant
	{
		if (name == (*it).getNickname()) //si le nom d'un client est le meme que celui donne en parametre
			return ((*it).getFd()); //on return le fd (int) du client.
		it++;
	}
	return (0);
}

Client	find_it_client_with_fd(int fd, std::vector<Client> &_clients) // amettre dans utils ??
{
	std::vector<Client>::iterator	it = _clients.begin(); //iterator sur client

	while (it != _clients.end()) //on parcour tout les clients existant
	{
		if (fd == (*it).getFd()) //si le nom d'un client est le meme que celui donne en parametre
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
		if (fd == (*it).getFd()) //si le nom d'un client est le meme que celui donne en parametre
			return ((*it).getNickname()); //on return le fd (int) du client.
		it++;
	}
	return ("");
}
