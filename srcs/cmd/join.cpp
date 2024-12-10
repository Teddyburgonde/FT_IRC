/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:53:57 by tebandam          #+#    #+#             */
/*   Updated: 2024/12/10 17:17:57 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Chanel.hpp"

//La fonction ci-dessous sert à recuperer tout les channels dont on parle, donc tout ce qui est apres un #
std::vector<std::string> create_chanName(const char *argument)
{
	int i = 0;
	int	f = 0;
	std::vector<std::string>	chanName;
	//le while ci-dessous sert à recuperer tout les channels dont on parle, donc tout ce qui est apres un #
	//si je fais ##general ca va faire de la merde ??
	while (argument[i] && argument[i] != ' ') //on stop quand espace ou fin de ligne //ATTENTION, PROBABLEMENT \r ou \t je sais plus
	{
		if ((i == 0 && argument[0] == '#') || (i > 0 && argument[i - 1] == ',' && argument[i] == '#')) //si le premier carractere est un #, ou sinon faut que le #soit juste apres une ','.
		{
			f = i;
			while (argument[f] && argument[f] != ',' && argument[f] != ' ' && argument[f] != '\n') //on set f au bout de la chaine qu'on veut recup
				f++;
			chanName.push_back(std::string(argument + i, argument + f)); //on recupere une chaine qui debute à i et qui fini à f et on l'ajoute au vecteur chanName;
		}
		else if (i == 0)
		{
			f = 0;
			while (argument[f] && argument[f] != ',' && argument[f] != ' ' && argument[f] != '\n') //on set f au bout de la chaine qu'on veut recup
				f++;
			std::cout << std::string(argument + i, argument + f) << " :Invalid channel name" << std::endl;
		}
		i++; //on incremente i pour que si 'i' vallais '#', il se décale pour en chercher un autre
	} //On boucle afin de recuperer tout les differents channel dont on parle
	return (chanName);
}

//DEBUG, PERMET DE PRINT TOUT LES USER DE TOUT LES CHANNEL, A GARDER DE COTE
void	print_userInchan(std::vector<Chanel> &_chanel)
{
	std::vector<Chanel>::iterator	it = _chanel.begin();
	std::vector<int> id_userInChannel;

	while (it != _chanel.end())
	{
		std::cout << "Dans le channel " << (*it).getName();;
		id_userInChannel = (*it).getUserInChannel();
		std::vector<int>::iterator	it_id = id_userInChannel.begin();
		while (it_id != id_userInChannel.end())
		{
			std::cout << "	dans channel: " << *it_id << std::endl;
			it_id++;
		}
		it++;
		std::cout << std::endl;
	}
}

void	handleJoin(int fd, Message &msg, std::vector<Chanel> &_chanel, std::vector<Client> &_clients)
{
	std::vector<Chanel>::iterator 		it_ChanExist; //channel existant
	std::vector<std::string>::const_iterator	it_chanNew;
	std::string argumentStr = msg.getArgument();
	const char	*argument = argumentStr.c_str(); //arguement  est egale a la string stocker dans la class msg._argument
	const std::vector<std::string> 			&chanName = create_chanName(argument);

	if (chanName.empty()) //si y'a pas de channel valide dans la commande reçu
	{
		std::cout << "No channel joined. Try JOIN #<channel>" << std::endl;
		return;
	}
	//ici on va rechercher si les noms stocké dans chanName sont déjà des noms de channels existant, afin que si ils n'existent pas, on les créés !
	it_chanNew = chanName.begin(); //init de l'iterator des differents nom de chan qu'on cherche
	while (it_chanNew != chanName.end()) //Tant que je trouve pas 'general' par exemple dans la liste des channels existantes
	{
		it_ChanExist = _chanel.begin(); //on repart du debut de la list de channel existant afin de la parcourir en entier et de bien chercher partout
		while (it_ChanExist != _chanel.end()) //tant qu'on a pas chercher dans tout ceux existant
		{
			if (!strcmp(((*it_ChanExist).getName()).c_str(), (*it_chanNew).c_str())) //si le nom du chan est le meme qu'un chan existant
				break; //on break pour garder l'iterator sur le bon chan
			it_ChanExist++;
		}
		if (it_ChanExist == _chanel.end()) //si chan existe pas, (donc 'it' est a la fin car on a tout parcouru sans trouver)
		{
			std::cout << "Create chan: " << *it_chanNew << std::endl; //!debug, à retirer!
			Chanel newChan; //On créé un nouveau channel
			newChan.setName(*it_chanNew); //on set le nom du channel qu'on viens de cree
			newChan.addUser(fd, true); // il rejoint en operateur psk c'est lui qui l'a créé
			_chanel.push_back(newChan); //on ajoute le nouveau channel a la list de channel existant
		}
		else if (is_user_in_chan(fd, (*it_ChanExist).getUserInChannel()) == 0)//sinon, donc le channel existais deja
		{
			//QUELQUE PQRT ICI, FAIRE UN CHECK SI K POUR MDP EST ACTIVE !!
			if ((*it_ChanExist).getModeI() == true && is_user_in_chan(fd, (*it_ChanExist).getInvitedUser())) //Si invite only et pas invité
			{
				std::string error = /*nomduserv*/ "473 " + find_nickname_with_fd(fd, _clients) + " " + (*it_ChanExist).getName() + " :Cannot join channel (+i)";
				send(fd, error.c_str(), error.size(), 0);
			}
			else 
				(*it_ChanExist).addUser(fd, false); //on ajoute la personne qui a fais la commande join à la liste des personnes qui sont dans ce channel.
		}
		it_chanNew++; //On passe au prochain channel que la personne veut rejoindre
	}
}
