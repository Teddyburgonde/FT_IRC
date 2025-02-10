/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 14:32:31 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/10 09:18:31 by tebandam         ###   ########.fr       */
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
		void parse_buffer(const std::string &buffer, Message& msg);
		void setCommand(std::string command);
		void setArgument(std::string command);

		std::string getCommand() const;
		std::string getArgument() const;
};


#endif 