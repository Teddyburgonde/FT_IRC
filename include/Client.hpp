/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 13:32:33 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/10 15:53:21 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>


/*
In the Customer class there is all the information about the customer
inside the server.
*/

class Client
{
	private:
		int	_fd;
		std::string IpAddress;
		std::string _nickname;
		std::string _username;

	public:
		Client();
		Client(int fd, const std::string &ip);
		~Client();

	public:
		int getFd() const;
		std::string	getIpAdress();
		std::string	getNickname();
		std::string	getUsername();

		void    setFd(int fd);
		void    setNickname(std::string newNickname);
		void    setIpAddress(const std::string &ip);
		void    setUsername(std::string newUsername);
};




#endif


