/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverSignals.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 09:05:42 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/09 20:07:02 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

/*
    The purpose of this function is to say that we have received a signal
*/

bool Server::Signal = false;

// Set the flag to "true" to interrupt the loops
void Server::signalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGQUIT)
    {
        Server::Signal = true;
        std::cout << "\nSignal received: Shutting down the server..." << std::endl;
    }
}