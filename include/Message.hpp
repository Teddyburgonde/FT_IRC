/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 14:32:31 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/05 16:44:37 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <vector>


class Message
{
	public: 
		Message();
		~Message();
		Message(Message const &src);
		Message &operator=(Message const &src);
		

	private:
		std::string _command;
		std::string _argument;

	
	public :
		/* Setters */
		void setCommand(std::string command);
		void setArgument(std::string command);

		/* Getters */
		std::string getCommand() const;
		std::string getArgument() const;
};

void parse_buffer(std::vector <std::string> &buffer, Message& msg);

#endif 