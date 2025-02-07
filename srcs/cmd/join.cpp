/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmersch <gmersch@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:53:57 by tebandam          #+#    #+#             */
/*   Updated: 2025/02/07 16:32:24 by gmersch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/Client.hpp"
#include "../../include/Server.hpp"
#include "../../include/Message.hpp"
#include "../../include/Channel.hpp"

static std::string	find_arg_chan(const char *argument, int &index)
{
	bool	full_arg = false;
	int		start = index;

	while (argument[index] && ((argument[index] != ',' && argument[index] != ' ' && argument[index] != '\n') || full_arg == true)) //on set f au bout de la chaine qu'on veut recup
	{
		if (argument[index] == ':')
			full_arg = true;
		index++;
	}
	return(std::string(argument + start, argument + index));
}

//La fonction ci-dessous sert à recuperer tout les channels dont on parle, donc tout ce qui est apres un #
static std::vector<std::string> create_chanName(const char *argument, int &i, int fd)
{
	int	f;
	std::vector<std::string>	chanName;
	std::vector<std::string>	error;
	//le while ci-dessous sert à recuperer tout les channels dont on parle, donc tout ce qui est apres un #
	//si je fais ##general ca va faire de la merde ??
	while (argument[i] && argument[i] != ' ') //on stop quand espace ou fin de ligne //ATTENTION, PROBABLEMENT \r ou \t je sais plus
	{
		f = i;
		if ((i == 0 && argument[0] == '#') || (i > 0 && argument[i - 1] == ',' && argument[i] == '#')) //si le premier carractere est un #, ou sinon faut que le #soit juste apres une ','.
			chanName.push_back(find_arg_chan(argument, f)); //on recupere une chaine qui debute à i et qui fini à f et on l'ajoute au vecteur chanName;
		else if (i == 0)
		{
			betterSend(ERR_NOSUCHCHANNEL((std::string)argument), fd);
			return (error);
		}
			//std::cout << find_arg_chan(argument, f) << " :Invalid channel name" << std::endl;
		i++; //on incremente i pour que si 'i' vallais '#', il se décale pour en chercher un autre
	} //On boucle afin de recuperer tout les differents channel dont on parle
	return (chanName);
}

//DEBUG, PERMET DE PRINT TOUT LES USER DE TOUT LES CHANNEL, A GARDER DE COTE
void	print_userInchan(std::vector<Channel> &_channel)
{
	std::vector<Channel>::iterator	it = _channel.begin();
	std::vector<int> id_userInChannel;

	while (it != _channel.end())
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

//return 1 if error
int	check_active_mode(std::vector<Channel>::iterator	&it_ChanExist, int fd, std::vector<Client> &_clients, std::string &arg_after_channel)
{
	//std::cout << "mode i = " << (*it_ChanExist).getModeI() << std::endl; //!debug
	//EST CE QUE PLUSIEUR ERREURDOIVENT S'QFFICHER SI ERREUR ?? OU JUSTE UNE SEUL ET FAIRE DES ELSE IF
	if ((*it_ChanExist).getModeI() == true && !is_user_in_chan(fd, (*it_ChanExist).getInvitedUser())) //Si invite only et pas invité
	{
		betterSend(ERR_INVITEONLYCHAN(find_nickname_with_fd(fd, _clients), (*it_ChanExist).getName()), fd);
		return (1);
	}
	if ((*it_ChanExist).getModeK() == true && (arg_after_channel.empty() || arg_after_channel != (*it_ChanExist).getPassword()))
	{
		betterSend(ERR_BADCHANNELKEY(find_nickname_with_fd(fd, _clients), (*it_ChanExist).getName()), fd);
		return (1);
	}
	if ((*it_ChanExist).getModeL() == true && (*it_ChanExist).get_nb_user_in() >= (*it_ChanExist).get_nb_user_max()) // nb limite de personne
	{
		betterSend(ERR_CHANNELISFULL(find_nickname_with_fd(fd, _clients), (*it_ChanExist).getName()), fd);
		return (1);
	}
	return (0);
}

static std::string trim(const std::string &str)
{
	size_t start = str.find_first_not_of(" \t\n\r");
	size_t end = str.find_last_not_of(" \t\n\r");
	return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

void handleJoin(int fd, Message &msg, std::vector<Channel> &_channel, std::vector<Client> &_clients)
{

    std::vector<Channel>::iterator it_ChanExist;
    std::vector<std::string>::const_iterator it_chanNew;
    int i = 0;

    std::string argumentStr = msg.getArgument();
    const char *argument = argumentStr.c_str();
    const std::vector<std::string> &chanName = create_chanName(argument, i, fd);
    std::string arg_after_channel = get_next_argument(argument, i);
    Client user_sender = find_it_client_with_fd(fd, _clients);

    if (chanName.empty())
    {
        std::cout << "Erreur : Aucun canal fourni. Utilisateur : " << user_sender.getNickname() << std::endl;
        return;
    }
    for (it_chanNew = chanName.begin(); it_chanNew != chanName.end(); ++it_chanNew)
    {
        std::cout << *it_chanNew << " ";
    }
    std::cout << std::endl;

    it_chanNew = chanName.begin();
    while (it_chanNew != chanName.end())
    {
		std::string trimmedChanName = trim(*it_chanNew);  // Trim du nom du canal à comparer
        for (std::vector<Channel>::iterator it = _channel.begin(); it != _channel.end(); ++it)
			std::cout << it->getName() << " ";
		std::cout << std::endl;

		// Recherche du canal existant
		it_ChanExist = _channel.begin();
		while (it_ChanExist != _channel.end())
		{
			std::string trimmedExistingName = trim(it_ChanExist->getName());  // Trim du nom du canal existant
			if (trimmedExistingName == trimmedChanName)
				break;
			++it_ChanExist;
		}

		if (it_ChanExist == _channel.end())
		{
			Channel newChan;
			newChan.setName(trimmedChanName);
			newChan.addUser(fd, true);
			_channel.push_back(newChan);
			std::string joinMessage = formatIrcMessage(user_sender.getNickname(), user_sender.getUsername(), "localhost", "JOIN", newChan.getName(), "");
			send(fd, joinMessage.c_str(), joinMessage.size(), 0);
		}
		else if (is_user_in_chan(fd, (*it_ChanExist).getUserInChannel()) == 0 && check_active_mode(it_ChanExist, fd, _clients, arg_after_channel) == 0)
		{
			it_ChanExist->addUser(fd, false);

			std::string joinMessage = formatIrcMessage(user_sender.getNickname(), user_sender.getUsername(), "localhost", "JOIN", (*it_ChanExist).getName(), "");
			it_ChanExist->sendMessageToChannel(fd, joinMessage);
		}
		++it_chanNew;
	}
}
