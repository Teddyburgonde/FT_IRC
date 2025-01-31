/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSignals.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 09:05:42 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/19 16:08:52 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

/*
Le but de cette fonction c'est de dire qu'on a reçu un signal
*/
bool Server::Signal = false; // On initialise la variable Signal a false ici car c'est une variable statique
void Server::signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGQUIT)
    {
        Server::Signal = true; // Mettre le flag à "true" pour interrompre les boucles
        std::cout << "\nSignal received: Shutting down the server..." << std::endl;
    }
}