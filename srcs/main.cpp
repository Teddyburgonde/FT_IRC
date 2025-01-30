/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 08:10:54 by tebandam          #+#    #+#             */
/*   Updated: 2025/01/30 15:30:25 by gmersch          ###   ########.fr       */
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
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
        return 1;
	}
	
	int port;

	port = std::atoi(argv[1]);
	if (port <= 0 || port > 65535) 
	{
        std::cerr << "Error: Invalid port number. Please provide a valid port (1-65535)." << std::endl;
        return 1;
    }
	std::string password = argv[2];
	try
	{
		Server server(port, password);
		server.serverInit();
		// server.closeFds(); // ???
		signal(SIGINT, Server::signalHandler); //-> catch the signal (ctrl + c)
		signal(SIGQUIT, Server::signalHandler); //-> catch the signal (ctrl + \)
		std::cout << "Server is running. Waiting for connections..." << std::endl;
		while(1)
		{
			
		}

	}
	catch (const std::exception& e)
	{
		// Gérer les erreurs (par exemple, si serverInit lève une exception)
		std::cerr << "Server Error: " << e.what() << std::endl;
		return -1; // il est necessaire le return ?????
	}
	std::cout << "The Server Closed!" << std::endl;
	return (0);
	
}



