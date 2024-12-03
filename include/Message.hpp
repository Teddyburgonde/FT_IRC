/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/03 14:32:31 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/03 15:56:40 by tebandam         ###   ########.fr       */
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
		std::vector<std::string> _nameChanel;
		std::string _command;
		std::string _option;
		std::string _argument;
	
	public :
		/* Setters */
		void setNameChanel(std::string nameChanel);
		void setCommand(std::string command);
		void setOption(std::string option);

		/* Getters */
		std::string getNameChanel() const;
		std::string getCommand() const;
		std::string getOption() const;	
};


#endif 