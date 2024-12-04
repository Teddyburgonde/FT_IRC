#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Chanel.hpp"
#include "../include/Message.hpp"
#include <algorithm>




void	parsing(int fd, char buffer[1024], std::vector<Chanel> &_chanel) // a remane, contiendra tout les strncmp
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


void Server::analyzeData(int fd, const char* buffer)
{
	Message msg;

	msg = parse_buffer(buffer);
	parsing(fd, (char*)buffer, this->_chanel); //ici sera tout les strncmp

	std::string oldNick;
	if (strncmp(buffer, "NICK ", 5) == 0)
	{
		oldNick = buffer + 5;
		// remove met les caracteres a supprimer a la fin
		// erase coupe la chaine juste avant les caracteres indesirables.
		oldNick.erase(std::remove(oldNick.begin(), oldNick.end(), '\r'), oldNick.end());
		oldNick.erase(std::remove(oldNick.begin(), oldNick.end(), '\n'), oldNick.end());
		if (oldNick.empty())
		{
			std::string response = ERR_NONICKNAMEGIVEN(std::string("Server"), std::string(""));
			// send envoie la reponse au client associé au fd
			send(fd, response.c_str(), response.size(), 0);
			return ;
		}
		for(unsigned long i = 0; i < _clients.size(); i++)
		{
			// si le pseudo est déjà  pris
			if (oldNick == _clients[i].getNickname())
			{
				std::string response = ERR_NICKNAMEINUSE(std::string("Server"), oldNick);
				send(fd, response.c_str(), response.size(), 0);
				return ;
			}
			if (_clients[i].getFd() == fd)
			{
				_clients[i].setNickname(oldNick);
				 std::cout << "Welcome in the server" << std::endl;
				std::cout << "Client FD: " << _clients[i].getFd()
                  << " Nickname: " << _clients[i].getNickname() << std::endl;
				std::string response = RPL_WELCOME(oldNick);
				send(fd, response.c_str(), response.size(), 0);
				break ;
			}
		}
    }
	if (strncmp(buffer, "PRIVMSG ", 8) == 0)
	{
		const char* start = buffer + 8; // Début après "PRIVMSG "

		// Trouver le destinataire
		const char* space = strchr(start, ' '); // Cherche le premier espace

		// Pas de destinataire trouver trouvée
		if (space == NULL)
        	return;
		// fais la meme chose que substr
		// ici notre constructeur std::string::string(const char* s, size_t n);
		// permet de creer une chaine grace a notre pointer
		std::string recipient(start, space - start);
		if (recipient.empty())
		{
			std::string response = ERR_NORECIPIENT(std::string("Server"), std::string(""));
			send(fd, response.c_str(), response.size(), 0);
			return ;
		}
		const char *colon = strchr(space, ':');
		if (colon == NULL)
		{
			std::string response = ERR_NOTEXTTOSEND(std::string("Server"));
			send(fd, response.c_str(), response.size(), 0);
			return ;
		}
		// Extraire le message
		std::string message(colon + 1); // Tout après ":" est le message
		bool recipientFound = false;
		for(unsigned long i = 0; i < _clients.size(); i++)
		{
			// le message est destiné à un utilisateur précis
			if (_clients[i].getNickname() == recipient)
			{
				send(_clients[i].getFd(), message.c_str(), message.size(), 0);
        		recipientFound = true;
        		break;
			}
			// else if // message envoyer a un chanel
			// {

			// }
			if (!recipientFound)
			{
				std::string response = ERR_NOSUCHNICK(std::string ("Server"), recipient);
				send(fd, response.c_str(), response.size(), 0);
			}
		}
	}
}

Message parse_buffer(const char *buffer)
{
	Message msg;
	int	i;
	int	j;

	i = skipSpaces(buffer);
	j = i;
	while (buffer[j] != ' ' && buffer[j])
		j++;
	if (buffer[i] == '\0')
		throw(std::runtime_error("buffer empty"));
	msg.setCommand(std::string (buffer + i, buffer + j));
	std::cout << "la commande : '" << msg.getCommand() << "'"<< std::endl;
	// suite du parsing

	//ce que tu peux faire si tu veux teddy quand je suis pas là demain matin:


	//si on est pas a la fin de buffer (je parle de j vu que j est soit sur l'espace soit sur le \0)
		//skip space
	//sinon erreur car pas d'argument ? Je sais pas si ya des commande sans arg donc a prendre avec des pincettes
	//si suite au skip space on est pas a la fin de buffer
//	{

		//si la où on est c'est un '#':
//		{

			//tout ce qui est directement apres le #, le recuperer et l'ajouter au vecteur '_nameChanel' de la class Message
			//attention, si y'a une virgulep puis un # avec un autre nom, mettre les deux: expemple: #general,#jeux  on doit recup general et jeux sans les #
			//(a verifier, tu peux demander a chat gpt, peut etre que fqut un espqce apres la virgule mmais je crois pas)
//		}
//	}
	//sinon si c'est un '-' et que ya des truc apres, peut etre mettre ca dans option ??? PAs sur du tout faut demander, je sais vraiment pas.
	//sinon si pas de # mais que ya des truc, mettre tout le reste en une string dans '_argument' de la class Message


	//Voilà en gros ce que je vois pour le moment, hesite pas a te rensigné car je suis pas sur.
	//Et apres je crois que c'est pas mal !


	//test:
	if (strncmp(msg.getCommand().c_str(), "LOOL", 4) == 0)
	{
		std::cout << "C'est un bon point" << std::endl;
	}
	else
		std::cout << "AIEIIEIEIEIEIIE" << std::endl;

	return msg;
}



// std::string findCommand()
// {

// }

