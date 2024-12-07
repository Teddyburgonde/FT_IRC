/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 10:09:19 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/07 15:21:08 by gmersch          ###   ########.fr       */
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

//Permet de chercher dans une list de user, un user specifique. Renvoie 1 si le user est trouvé.
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