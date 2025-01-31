/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 14:40:30 by tebandam          #+#    #+#             */
/*   Updated: 2025/01/30 15:30:35 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Message.hpp"

Message::Message()
{
	
}

Message::~Message()
{
	
}

Message::Message(Message const &src)
{
	*this = src;
}

Message &Message::operator=(Message const &src)
{
	if (this != &src)
	{
		this->_command = src._command;
	}
	return *this;
}

/* Setters */

// void Message::setNameChannel(std::string nameChannel)
// {
// 	this->_nameChannel.push_back(nameChannel);
// }

void Message::setCommand(std::string command)
{
	this->_command = command;
}

void Message::setArgument(std::string argument)
{
	this->_argument = argument;
}
/* Getters */


std::string Message::getCommand() const
{
	return this->_command;
}

std::string Message::getArgument() const
{
	return this->_argument;
}

