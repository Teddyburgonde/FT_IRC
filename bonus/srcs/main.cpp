/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 08:10:54 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/13 16:51:14 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

int	main(int argc, char **argv)
{
	try 
	{
		if (argc != 3)
			throw std::runtime_error("Usage: ./ircserv <port> <password>");
		int port;
		std::string password;
		if (isValidNumber(argv[1]) == 0) 
		{
			std::cerr << "Error: Invalid port number. Must be a valid integer." << std::endl;
			return 1;
		}
		port = std::atoi(argv[1]);
		if (port <= 0 || port > 65535) 
			throw std::runtime_error("Error: Invalid port number. Please provide a valid port (1-65535).");
		password = argv[2];
		Server server(port, password);

		signal(SIGINT, Server::signalHandler);
		signal(SIGQUIT, Server::signalHandler);
		
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



