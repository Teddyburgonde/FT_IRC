/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 08:10:54 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/03 15:18:07 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdlib>
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Channel.hpp"
#include <algorithm>
#include <cstring>


int	main(int argc, char **argv)
{
	try 
	{
		if (argc != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		int port;

		port = std::atoi(argv[1]);
		if (port <= 0 || port > 65535) 
			throw std::runtime_error("Error: Invalid port number. Please provide a valid port (1-65535).");
		std::string password = argv[2];
		Server server(port, password);

		signal(SIGINT, Server::signalHandler); //-> catch the signal (ctrl + c)
		signal(SIGQUIT, Server::signalHandler); //-> catch the signal (ctrl + \)
		
		server.serverInit();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Server Error: " << e.what() << std::endl;
		return (1);
	}
	std::cout << "The Server Closed!" << std::endl;
	return (0);
}



