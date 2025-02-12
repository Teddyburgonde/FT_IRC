/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 14:40:30 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/10 08:56:53 by tebandam         ###   ########.fr       */
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
	return (*this);
}

void Message::setCommand(std::string command)
{
	this->_command = command;
}

void Message::setArgument(std::string argument)
{
	this->_argument = argument;
}

std::string Message::getCommand() const
{
	return (this->_command);
}

std::string Message::getArgument() const
{
	return (this->_argument);
}
