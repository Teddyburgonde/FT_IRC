/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 14:40:30 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/03 14:40:58 by tebandam         ###   ########.fr       */
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
		this->_nameChanel = src._nameChanel;
		this->_command = src._command;
		this->_option = src._option;
	}
	return *this;
}

/* Setters */

void Message::setNameChanel(std::string nameChanel)
{
	this->_nameChanel.push_back(nameChanel);
}

void Message::setCommand(std::string command)
{
	this->_command = command;
}

void Message::setOption(std::string option)
{
	this->_option = option;
}

/* Getters */

std::string Message::getNameChanel() const
{
	return this->_nameChanel[0];
}

std::string Message::getCommand() const
{
	return this->_command;
}

std::string Message::getOption() const
{
	return this->_option;
}

