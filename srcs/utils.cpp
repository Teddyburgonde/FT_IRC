/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 10:09:19 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/08 12:25:27 by gmersch          ###   ########.fr       */
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
	std::vector<int>::iterator it_userInChannel;

	it_userInChannel = userInChannel.begin();
	while (it_userInChannel != userInChannel.end())
	{
		if (*it_userInChannel == fd) //si le user est déjà dans la liste des gens dans le serveur
			return (1);
		it_userInChannel++;
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