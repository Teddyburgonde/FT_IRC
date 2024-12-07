/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 13:32:33 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/07 18:53:26 by gmersch          ###   ########.fr       */
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
Dans la class Client il y a toute les informations sur le client
a l'interieur du serveur.
*/

class Client
{
	private:
		int	_fd; // file descriptor du client
		std::string IpAddress;  // adresse ip du client
		std::string _nickname;
		std::string _username;

	public:
		Client();
		Client(int fd, const std::string &ip);
		~Client();

	public:
		/* Getters */
		int getFd() const;
		std::string    getNickname();
		std::string    getUsername();

		/* Setters */
		void    setNickname(std::string newNickname);
		void    setUsername(std::string newUsername);
		void    setFd(int fd); // setter pour le file descriptor
		void    setIpAddress(const std::string &ip); // setter pour l'adresse ip
};

int	find_fd_with_nickname(std::string &name, std::vector<Client> &_clients); // amettre dans utils ??
std::string	find_nickname_with_fd(int fd, std::vector<Client> &_clients);


#endif


