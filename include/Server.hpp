/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tebandam <tebandam@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 18:58:16 by teddybandam       #+#    #+#             */
/*   Updated: 2024/12/05 15:51:00 by tebandam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <vector>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <sys/poll.h>
#include <iostream>
#include <string.h>


#define RPL_JOIN(nick, channel)                        (":" + nick + " JOIN " + channel + "\r\n")
#define RPL_PART(client, channel)                    (":" + client + " PART " + channel + "\r\n")
#define RPL_MODE(client, channel, mode, name)        (":" + client + " MODE " + channel + " " + mode + " " + name + "\r\n")
#define RPL_KICK(client, channel, target)            (":" + client + " KICK " + channel + " " + target + "\r\n")
#define RPL_INVITERCVR(client, invitee, channel)    (":" + client + " INVITE " + invitee + " " + channel + "\r\n")
#define RPL_NICK(oldNick, newNick)                    (":" + oldNick + " NICK " + newNick + "\r\n")
#define RPL_TOPIC(client, channel, topic)            (":" + client + " TOPIC " + channel + " :" + topic + "\r\n")
#define RPL_WELCOME(client)                            (": 001 " + client + " :Welcome in the IRC world, " + client + "\r\n")
#define RPL_NOTOPIC(client, channel)                (": 331 " + client + " " + channel + " :No topic is set\r\n")
#define RPL_SEETOPIC(client, channel, topic)        (": 332 " + client + " " + channel + " :" + topic + "\r\n")
#define RPL_INVITESNDR(client, invitee, channel)    (": 341 " + client + " " + invitee + " " + channel + "\r\n")
#define RPL_NAMEREPLY(nick, channel, nicknames)        (": 353 " + nick + " = " + channel + " :" + nicknames + "\r\n")


#define ERR_TOOMUCHPARAMS(client, cmd)                (client + " " + cmd + " :Too much parameters\r\n")
#define ERR_USERONCHANNEL(nick, channel)            (": 303 " + nick + " " + channel + " :is already on channel\r\n")
#define ERR_NOSUCHNICK(client, nickname)            (": 401 " + client + " " + nickname + " :No such nickname\r\n")
#define ERR_NOSUCHCHANNEL(channel)                    (": 403 " + channel + " :No such channel\r\n")
#define ERR_CANNOTSENDTOCHAN(client, channel)        (": 404 " + client + " " + channel + " :Cannot send to channel\r\n")
#define ERR_NOTEXTTOSEND(client)                    (": 412 " + client + " :No text to send\r\n")
#define ERR_NONICKNAMEGIVEN(client, nick)            (": 431 " + client + " " + nick + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(client, nickname)        (": 432 " + client + " " + nickname + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(client, nick)                (": 433 * " + client + " " + nick + " :Nickname is already in use\r\n")
#define ERR_NORECIPIENT(client, command) 				(": 411 " + client + " " + command + " :No recipient given\r\n")

#define ERR_USERNOTINCHANNEL(client, nick, channel)    (": 441 " + client + " " + nick + " " + channel + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(client, channel)            (": 442 " + client + " " + channel + " :Not on that channel\r\n")
#define ERR_NOTREGISTERED(client)                    (": 451 " + client + " :You have not registered\r\n")
#define ERR_NEEDMOREPARAMS(client, cmd)                (": 461 " + client + " " + cmd + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED(client)                (": 462 " + client + " :You may not reregister\r\n")
#define ERR_PASSWDMISMATCH(client)                    (": 464 " + client + " :Password incorrect\r\n")
#define ERR_KEYSET(channel)                            (": 467 " + channel + " :Channel key already set\r\n")
#define ERR_CHANNELISFULL(client, channel)            (": 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_INVITEONLYCHAN(client, channel)            (": 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANNELKEY(client, channel)            (": 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_NOCHANMODES(channel)                    (": 477 " + channel + " :Channel doesn't support modes\r\n")
#define ERR_CHANOPRIVSNEEDED(client, channel)        (": 482 " + client + " " + channel + " :You're not channel operator\r\n")


/* Utils */

int	skipSpaces(const char *str);







class Client;

class Chanel;
/*
Dans la class Server, il y a toute les informations  sur le serveur.
Il y a aussi un vecteur de clients qui sont connectes au serveur.
*/
class Server
{
	private:
		int	_fd; // file descriptor du client
		int _port;  // port du serveur
		int _serverSocketFd; // file descriptor du serveur socket
		static bool Signal; // variable pour le signal
		std::vector<Client> _clients; // liste de personnes connectees au serveur via HexChat
		std::vector<struct pollfd> _pollFds; // tableau où sera stocker tous les sockets à surveiller
		std::vector<Chanel> _chanel;
	public:
		Server();
		~Server();
		void serverInit(); // initialisation du serveur
		void createServerSocket(); // creation du serveur socket
		static void signalHandler(int signal); // handler pour le signal
		void closeFds(); // fermeture des file descriptors
		void clearClients(int fd); // effacer les clients
		void acceptNewClient(); // Accepter les nouveaux clients
		void receiveNewData(int fd); // Reception de la data 
		void analyzeData(int fd,  const std::string &buffer);
	public:
		int getFd() const; // getter pour le file descriptor
};


#endif