/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSignals.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 09:05:42 by tebandam          #+#    #+#             */
/*   Updated: 2025/01/31 16:05:38 by gmersch          ###   ########.fr       */
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