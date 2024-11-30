/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 13:32:33 by tebandam          #+#    #+#             */
/*   Updated: 2024/11/30 09:39:03 by tebandam         ###   ########.fr       */
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
	
	public:
		Client(int fd);
		~Client();
		int getFd() const;
};

#endif