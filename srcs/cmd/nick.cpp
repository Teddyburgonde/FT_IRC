/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/15 16:34:07 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/04 15:44:32 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

void Server::handleNick(int fd, const std::string& newNick)
{
    if (newNick.empty())
	{
        std::string response = ERR_NONICKNAMEGIVEN(std::string("Server"), "");
        send(fd, response.c_str(), response.size(), 0);
        return;
    }
    
    for (size_t i = 0; i < _clients.size(); ++i)
	{
        if (_clients[i].getNickname() == newNick)
        {
            std::string response = ERR_NICKNAMEINUSE(std::string("Server"), newNick);
            send(fd, response.c_str(), response.size(), 0);
            return;
        }

        if (_clients[i].getFd() == fd)
        {
            _clients[i].setNickname(newNick);
            //!Est ce que on dois faire le messages ci dessous tout le temps ?!
            std::string response = RPL_WELCOME(newNick);
            send(fd, response.c_str(), response.size(), 0);
            /*DEBUG ???*/ std::cout << "Client FD: " << fd << " Nickname: " << newNick << std::endl;
            return;
        }
    }
}