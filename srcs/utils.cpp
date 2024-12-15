/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 10:09:19 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/15 12:47:15 by tebandam         ###   ########.fr       */
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

    while (line[index] && line[index] == ' ')
        index++;
    start = index;
    while (line[start] && line[start] != '\n' && (line[start] != ' ' || full_arg == true)) //en gros si c'est un espace ca s'arrete sauf si full_arg == true
    {
        if (line[index] == ':')
            full_arg = true;
        index++;
    }
    return (std::string(line + start, line + index));
}