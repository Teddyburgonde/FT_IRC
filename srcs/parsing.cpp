#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Chanel.hpp"

void	parsing(int fd, char buffer[1024], std::vector<Chanel> &_chanel)
{
	//pqrsing ? 
	// /join #rrtnvr
	std::cout << buffer << std::endl;
	if (!strncmp(buffer, "JOIN", 4))
	{
		std::cout << "made join " << std::endl;
		handleJoin(fd, buffer, _chanel);
	}
	if (!strncmp(buffer, "SEND #general", 13)) // a redefinir, marche seulement pour general
	{
		std::string msg = std::string(buffer + 13); //ici je recup le msg apres le SEND #general donc a changer
		_chanel[0].sendMessageToChanel(fd, msg);
		std::cout << "send a message to general" << std::endl;
	}
}

void	handleJoin(int fd, char buffer[1024], std::vector<Chanel> &_chanel)
{
	int i = 0;
	int f = 0;
	std::vector<Chanel>::iterator it = _chanel.begin();
	std::string chanName;

	while (buffer[i] != '#')
		i++;
	f = i;
	while (buffer[f] && buffer[f] != ',')
		f++;
	//stocket dans une variable le bon truc de buffer
	chanName = std::string(buffer + i, buffer + f);
	it = _chanel.begin();
	while (it != _chanel.end())
	{
		if ((*it).getName() == chanName)
			break;
		it++;
	}
	if (it == _chanel.end() && it == _chanel.begin()) //si chan existe pas
	{
		std::cout << "create chan " << chanName << std::endl;
		Chanel newChan;
		newChan.addUser(fd, true); //rejoin en ope psk c'est lui qui l'a cree
		_chanel.push_back(newChan);
	}
	else
	{
		(*it).addUser(fd, true);
	}
}