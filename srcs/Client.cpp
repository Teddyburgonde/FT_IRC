/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 08:45:08 by tebandam          #+#    #+#             */
/*   Updated: 2024/11/30 09:38:42 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/Client.hpp"

Client::Client(int fd) :_fd(fd)
{
	
}

int Client::getFd() const
{
	return this->_fd;
}

Client::~Client(){}