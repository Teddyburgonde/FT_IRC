/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 08:10:54 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/05 09:08:43 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "Server.hpp"
#include "Client.hpp" 

int	main()
{
	try 
	{
		Server server;
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
        std::cerr << "Error: " << e.what() << std::endl;
        return -1; // il est necessaire le return ?????
    }
	std::cout << "The Server Closed!" << std::endl;
	return (0);
	
}



