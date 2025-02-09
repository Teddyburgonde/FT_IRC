/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 18:58:16 by teddybandam       #+#    #+#             */
/*   Updated: 2025/02/09 17:27:13 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "Message.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <csignal>
#include <sys/poll.h>
#include <iostream>
#include <string.h>
#include <ostream>
#include <sstream>
#include <string>

#define CLIENT(nick, user)							(nick + "!" + user + "@localhost")
#define PRIVMSG(client, target, message)			(":" + client + " PRIVMSG " + target + " :" + message + "\r\n")

#define RPL_JOIN(client, channel)					(":" + client + " JOIN " + channel + "\r\n")
#define RPL_PART(client, channel)					(":" + client + " PART " + channel + "\r\n")
#define RPL_MODE(client, channel, mode, name)		(":" + client + " MODE " + channel + " " + mode + " " + name + "\r\n")
#define RPL_KICK(client, channel, target)			(":" + client + " KICK " + channel + " " + target + "\r\n")
#define RPL_INVITERCVR(client, invitee, channel)	(":" + client + " INVITE " + invitee + " " + channel + "\r\n")
#define RPL_NICK(oldNick, newNick)					(":" + oldNick + " NICK " + newNick + "\r\n")
#define RPL_TOPIC(client, channel, topic)			(":" + client + " TOPIC " + channel + " :" + topic + "\r\n")
#define RPL_WELCOME(client)							(": 001 " + client + " :Welcome in the IRC world, " + client + "\r\n")
#define RPL_NOTOPIC(client, channel)				(": 331 " + client + " " + channel + " :No topic is set\r\n")
#define RPL_SEETOPIC(client, channel, topic)		(": 332 " + client + " " + channel + " :" + topic + "\r\n")
#define RPL_INVITESNDR(client, invitee, channel)	(": 341 " + client + " " + invitee + " " + channel + "\r\n")
#define RPL_NAMEREPLY(nick, channel, nicknames)		(": 353 " + nick + " = " + channel + " :" + nicknames + "\r\n")

#define ERR_TOOMUCHPARAMS(client, cmd)				(client + " " + cmd + " :Too much parameters\r\n")
#define ERR_USERONCHANNEL(nick, channel)			(": 303 " + nick + " " + channel + " :is already on channel\r\n")
#define ERR_NOSUCHNICK(client, nickname)			(": 401 " + client + " " + nickname + " :No such nickname\r\n")
#define ERR_NOSUCHCHANNEL(channel)					(": 403 " + channel + " :No such channel\r\n")
#define ERR_CANNOTSENDTOCHAN(client, channel)		(": 404 " + client + " " + channel + " :Cannot send to channel\r\n")
#define ERR_NOTEXTTOSEND(client)					(": 412 " + client + " :No text to send\r\n")
#define ERR_NONICKNAMEGIVEN(client, nick)			(": 431 " + client + " " + nick + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(client, nickname)		(": 432 " + client + " " + nickname + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(client, nick)				(": 433 * " + client + " " + nick + " :Nickname is already in use\r\n")
#define ERR_NORECIPIENT(client, command)			(": 411 " + client + " " + command + " :No recipient given\r\n")

#define ERR_USERNOTINCHANNEL(client, nick, channel)	(": 441 " + client + " " + nick + " " + channel + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(client, channel)			(": 442 " + client + " " + channel + " :Not on that channel\r\n")
#define ERR_NOTREGISTERED()							(": 451 :You have not registered\r\n")
#define ERR_NEEDMOREPARAMS(client, cmd)				(": 461 " + client + " " + cmd + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTRED(client)				(": 462 " + client + " :You may not reregister\r\n")
#define ERR_PASSWDMISMATCH(client)					(": 464 " + client + " :Password incorrect\r\n")
#define ERR_KEYSET(channel)							(": 467 " + channel + " :Channel key already set\r\n")
#define ERR_CHANNELISFULL(client, channel)			(": 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_UNKNOWNMODE(client, mode)				(": 472 " + client + " " + mode + " :is unknown mode char to me\r\n")
#define ERR_INVITEONLYCHAN(client, channel)			(": 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BADCHANNELKEY(client, channel)			(": 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_NOCHANMODES(channel)					(": 477 " + channel + " :Channel doesn't support modes\r\n")
#define ERR_CHANOPRIVSNEEDED(client, channel)		(": 482 " + client + " " + channel + " :You're not channel operator\r\n")

class Message;
class Client;
class Channel;

/* Utils */

int			skipSpaces(const char *str);
std::string get_next_argument(const char *line, int &index);
void		betterSend(std::string str, int fd);
int			find_fd_with_nickname(std::string &name, std::vector<Client> &_clients);
std::string	find_nickname_with_fd(int fd, std::vector<Client> &_clients);
Client		find_it_client_with_fd(int fd, std::vector<Client> &_clients);
std::string find_username_with_fd(int fd, std::vector<Client> &_clients);
std::string formatIrcMessage(const std::string &nickname, const std::string &username, const std::string &host, const std::string &command, const std::string &target, const std::string &message);


/*
Dans la class Server, il y a toute les informations  sur le serveur.
Il y a aussi un vecteur de clients qui sont connectes au serveur.
*/
class Server
{
	private:
		int	_fd; // file descriptor du client
		int _port;  // port du serveur
		int _fdAccept;
		std::string _password; // password
		int _serverSocketFd; // file descriptor du serveur socket
		static bool Signal; // variable pour le signal
		std::vector<Client> _clients; // liste de personnes connectees au serveur via HexChat
		std::vector<struct pollfd> _pollFds; // tableau où sera stocker tous les sockets à surveiller
		std::vector<Channel> _channel;
	public:
		Server();
		Server(int port, std::string &password);
		~Server();
		Channel* findChannel(const std::string &channelName, std::vector<Channel> &_channel);
		std::map<int, bool> _authenticatedClients;
		static void	signalHandler(int signal);
		void	serverInit();
		void	createServerSocket();
		void	closeFds();
		void	clearClients(int fd);
		void	acceptNewClient();
		void	receiveNewData(int fd);
		void	analyzeData(int fd, std::string &buffer);
		void	handleNick(int fd, const std::string& newNick) ;
		void	handleUser(int fd, const std::string& user);
		void	handlePrivMsg(int fd, Message &msg, std::vector<Channel> &_channel);
		void	handleKick(int fd, Message &msg, std::vector<Channel> &_channel);
		bool	isSenderInChannel(int fd, Channel &channel);
		bool	isSenderOperator(int fd, Channel &channel);
		bool	validateKickArgs(int fd, Message &msg, std::string &channel, std::string &targetUser);
		bool	isTargetInChannel(const std::string &targetUser, Channel &channel, int fd);
		void	notifyKick(Channel &channel, const std::string &sender, const std::string &targetUser, const std::string &reason);
		void	handleTopic(int fd, const Message &msg, std::vector<Channel> &_channel);
		bool	verifyPassword(const std::string& clientPassword) const; 
		bool	authenticatedClients(int fd, std::string &buffer);
		void	socketConfigurationForPoll(int fd);
		int		acceptionConnection(struct sockaddr_in &clientAddr);
		void	addClientToList(int _fdAccept, struct sockaddr_in &clientAddr);
		void	sendEnterPasswordMessage(int fd);
		int		getFd() const;
		int		getFdAccept() const;
};


#endif