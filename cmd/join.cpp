#include "include/Client.hpp"
#include "include/Server.hpp"
#include "include/Message.hpp"
#include "include/Chanel.hpp"

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
			while (argument[f] && argument[f] != ',' && argument[f] != ' ') //on set f au bout de la chaine qu'on veut recup
				f++;
			chanName.push_back(std::string(argument + i, argument + f)); //on recupere une chaine qui debute à i et qui fini à f et on l'ajoute au vecteur chanName;
		}
		else if (i == 0)
		{
			f = 0;
			while (argument[f] && argument[f] != ',' && argument[f] != ' ') //on set f au bout de la chaine qu'on veut recup
				f++;
			std::cout << std::string(argument + i, argument + f) << " :Invalid channel name" << std::endl;
		}
		i++; //on incremente i pour que si 'i' vallais '#', il se décale pour en chercher un autre
	} //On boucle afin de recuperer tout les differents channel dont on parle
	return (chanName);
}

void	handleJoin(int fd, Message &msg, std::vector<Chanel> &_chanel)
{
	std::vector<Chanel>::iterator 		it = _chanel.begin();
	std::vector<std::string>::const_iterator	it_chanName;
	const char 							*argument = (msg.getArgument()).c_str(); //arguement  est egale a la string stocker dans la class msg._argument
	const std::vector<std::string> 			&chanName = create_chanName(argument);

	if (chanName.empty()) //si y'a pas de channel valide dans la commande reçu
	{
		std::cout << "No channel joined. Try /join #<channel>" << std::endl;
		return;
	}
	//ici on va rechercher si les noms stocké dans chanName sont déjà des noms de channels existant, afin que si ils n'existent pas, on les créés !
	it_chanName = chanName.begin(); //init de l'iterator des differents nom de chan qu'on cherche
	while (it_chanName != chanName.end()) //Tant que on a pas chercher les differents nom de channel dont on parle
	{
		it = _chanel.begin(); //on repart du debut de la list de channel existant afin de la parcourir en entier et de bien chercher partout
		while (it != _chanel.end()) //tant qu'on a pas chercher dans tout ceux existant
		{
			if ((*it).getName() == *it_chanName) //si le nom du chan est le meme qu'un chan existant
				break; //on break pour garder l'iterator sur le bon chan
			it++;
		}
		if (it == _chanel.end() || it == _chanel.begin()) //si chan existe pas, (donc 'it' n'est pas a la fin) // ATTENTION< IT == CHANNEL BEGIN ?????? PAS SUUUUUUUR
		{
			std::cout << "create chan " << *it_chanName << std::endl; //!debug, à retirer!
			Chanel newChan; //On créé un nouveau channel
			newChan.setName(*it_chanName); //on set le nom du channel qu'on viens de cree
			newChan.addUser(fd, true); // il rejoint en operateur psk c'est lui qui l'a créé
			_chanel.push_back(newChan); //on ajoute le nouveau channel a la list de channel existant
		}
		else //sinon, donc le channel existais deja
		{
			(*it).addUser(fd, false); //on ajoute la personne qui a fais la commande join à la liste des personnes qui sont dans ce channel.
			std::cout << "User: " << fd << "added to " << (*it).getName() << std::endl; //debug aussi, a retirer
		}
		it_chanName++; //On passe au prochain channel que la personne veut rejoindre
	}
}