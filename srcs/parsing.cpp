#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Chanel.hpp"

void	parsing(int fd, char buffer[1024], std::vector<Chanel> &_chanel)
{
	//Les espaces après la commande cherché ?? Interressant tant que pas de parsing pour etre sur que la commande est pas JOINOITURE par exemple
	// /join #general
	//std::cout << buffer << std::endl;
	if (!strncmp(buffer, "JOIN ", 5)) //si c'est join la commande, a changer grace au futur parsing ?
	{
		std::cout << "made join " << std::endl; //debug, a retirer
		handleJoin(fd, buffer, _chanel);
	}
	if (!strncmp(buffer, "SEND #general", 13)) // a redefinir, marche seulement pour general
	{
		//besoins du parsing
		std::string msg = std::string(buffer + 13); //ici je recup le msg apres le SEND #general donc a changer
		//dans l'idee faudrais juste envoye le truc après le # grace au parsing ? Au lieu de channel[0], je retrouve le nom du bon salon.
		_chanel[0].sendMessageToChanel(fd, msg); //chanel[0] == que le premier salon. Faut coder le fais d'envoyé dans le salon ou il est le client
		std::cout << "send a message to general" << std::endl;
	}
}

void	handleJoin(int fd, char buffer[1024], std::vector<Chanel> &_chanel)
{
	int i = 0;
	int f = 0;
	std::vector<Chanel>::iterator it = _chanel.begin();
	std::string chanName;

	while (buffer[i] != '#') //Grace au parsing, le code ici sera beaucoup moins long
		i++;
	f = i;
	while (buffer[f] && buffer[f] != ',') //pour le parsing
		f++;
	//stocket dans une variable le bon truc de buffer
	chanName = std::string(buffer + i, buffer + f);
	it = _chanel.begin();
	while (it != _chanel.end()) //on cherche si le chanel existe deja:
	{
		if ((*it).getName() == chanName) //si le nom du chan est le meme qu'un chan existant
			break; //on break pour garder l'iterator sur le bon chan
		it++;
	}
	std::cout << "HERE 1" << std::endl;
	if (it == _chanel.end() && it == _chanel.begin()) //si chan existe pas
	{
		std::cout << "create chan " << chanName << std::endl;
		Chanel newChan;
		newChan.setName(chanName);
		newChan.addUser(fd, true); //rejoin en ope psk c'est lui qui l'a cree
		_chanel.push_back(newChan);
	}
	else
	{
		(*it).addUser(fd, false);
		std::cout << "User: " << fd << "added to " << (*it).getName() << std::endl; //debug aussi, a retirer
	}
	std::cout << "HERE 2" << std::endl;
}