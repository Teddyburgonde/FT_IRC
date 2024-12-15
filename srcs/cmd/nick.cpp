/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/15 16:34:07 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/15 16:34:08 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Chanel.hpp"

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
        if (_clients[i].getNickname() == newNick) {
            std::string response = ERR_NICKNAMEINUSE(std::string("Server"), newNick);
            send(fd, response.c_str(), response.size(), 0);
            return;
        }

        if (_clients[i].getFd() == fd) {
            _clients[i].setNickname(newNick);
            std::string response = RPL_WELCOME(newNick);
            send(fd, response.c_str(), response.size(), 0);
            std::cout << "Client FD: " << fd << " Nickname: " << newNick << std::endl;
            return;
        }
    }
}