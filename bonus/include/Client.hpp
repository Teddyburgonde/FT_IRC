/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 13:32:33 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/11 16:53:56 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>


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


