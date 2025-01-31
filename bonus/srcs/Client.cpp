/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 08:45:08 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/21 13:28:33 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Chanel.hpp"

Client::Client(){}


Client::Client(int fd, const std::string &ip) :_fd(fd), IpAddress(ip)
{
	
}

int Client::getFd() const
{
	return this->_fd;
}

std::string	Client::getIpAdress()
{
	return (this->IpAddress);
}


std::string Client::getNickname()
{
	return this->_nickname;
}

std::string Client::getUsername()
{
	return this->_username;
}

void Client::setNickname(std::string newNickname)
{
	this->_nickname = newNickname;
}

void Client::setUsername(std::string newUsername)
{
	this->_username = newUsername;
}

void Client::setFd(int fd)
{
	this->_fd = fd;
}
void Client::setIpAddress(const std::string &ip)
{
	this->IpAddress = ip;
}

Client::~Client(){}
