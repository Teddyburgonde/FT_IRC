/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 17:25:39 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/11 18:05:43 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Channel.hpp"


std::string Server::handleBot(void)
{
	std::string clientChannel;
	std::string reply;
	
	reply = "Insult prohibited in this channel 😡\n";
	std::cout << "detected insult ❗" << std::endl;
	return (reply);
}
