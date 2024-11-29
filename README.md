# FT_IRC

3/25

-------------------------------------------------
Définitions :

# IRC
```c
IRC (Internet Relay Chat) est un protocole qui permet de dialoguer en temps réel à l'aide d'un logiciel client qui se connecte
à un serveur IRC.
Ce serveur gère les échanges de messages entre les utilisateurs et les canaux (salons de discussion).
Points importants à retenir :
Protocole : IRC est un protocole standardisé pour la communication en temps réel.
   1. Client-Serveur : Les utilisateurs utilisent un client IRC(Un logiciel ) pour se connecter à un serveur IRC.
   2. Serveur IRC : Il joue le rôle de "hub" qui relaye les messages entre les clients connectés.
   3. Canaux : Les discussions se font dans des "channels" (ex : #general), mais des messages privés
entre utilisateurs sont aussi possibles.
```
![architecture(2)](https://github.com/user-attachments/assets/98a103ea-4d4d-4846-a46f-8594743ad073)

<br>

```c
1. Le Message est transmit au logiciel Hexchat.
2. Hexchat se connecte au Server Irc grace a une socket et lui transmet le message.
3. Le serve Irc renvoie le message au bon endroit ( dans le bon canal).
A la fin les deux clients (utilisateurs) voient le message sur Hexchat.
```
<br>

# Socket :

<br>

```c
La socket sert de pont entre le client (par exemple HexChat) et ton serveur IRC. 
Elle permet d'établir une communication bidirectionnelle pour que le serveur et
le client puissent échanger des messages.
En d'autre terme chaque client se sert de la socket pour se connecter au serveur IRC. 
```

------------------------------------------------------------------------------------------------------------------------------------------------------------------------

**Explication des fonctions :**

- signal [✅ ] 

**Définition :**

Associe le signal SIGINT (Ctrl+C) à la méthode statique 
Server::signalHandler.
En d'autre terme , il gere le Ctrl C dans ce cas la. 

**Exemple d'utilisation :** 
signal(SIGINT, Server::signalHandler);

------------------------------------------------------------------------------------------
- sigaction [✅]

**Définition :**

Comme signal(),  elle permet de spécifier une fonction à exécuter lorsque le programme reçoit un signal.
Avec sigaction, tu peux définir des options supplémentaires, comme :
    Bloquer d'autres signaux pendant l'exécution du handler.
    Remplacer ou restaurer un ancien gestionnaire de signal.
    Différencier les signaux capturés.

```c
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```
------------------------------------------------------------------------------------------------
- socket [✅ ]

**Prototype :** 

```c
int socket(int domain, int type, int protocol);
```

**Paramètres de socket() :** 

**Domain :** 
Indique le type d'adresse que la socket utilisera : 
AF_INET : Adresse IPv4 (la plus courante pour un serveur réseau classique). :+1: 
AF_INET6 : Adresse IPv6.
AF_UNIX : Communication locale (entre processus sur la même machine).

**Type :**
Indique le protocole de communication utilisé : 
SOCK_STREAM : Pour une communication orientée connexion.
Utilise le protocole TCP (fiable, ordonné). :+1: 
SOCK_DGRAM : Pour une communication sans connexion.
Utilise le protocole UDP (moins fiable, rapide).
SOCK_RAW : Pour manipuler directement les paquets réseau (nécessite des droits spécifiques).

**Protocole :** 
indique le protocole réseau utilisé. Géneralement, tu peux mettre 0 pour
utiliser le protocole par défaut associé au type : 
Pour SOCK_STREAM, cela utilise TCP. :+1: 
Pour SOCK_DGRAM, cela utilise UDP.

**Définition :**

La fonction socket sert  a creer une socket.
C'est la premiere étape pour établir une communication via un réseau.

**return :** 

Si la création de la socket est réussit elle retun un entier posititf (Le descripteur de fichier ou fd).
En cas d'échec, elle return -1 et errno contient l'erreur.


**Exemple :** 

int sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd == -1) {
    perror("Erreur lors de la création de la socket");
    exit(EXIT_FAILURE);
}

-------------------------------------------------------------------------------------------------

- htons  [ ✅]

**Prototype :** 

```c
#include <arpa/inet.h>
uint16_t htons(uint16_t hostshort);
```

hostshort : Un entier de 16 bits représentant un nombre en ordre d'octets local (host byte order).

**return :** 
La valeur convertie en ordre d'octets réseau (network byte order).

**Que fait la fonction :**
htons() convertit un entier de 16 bits (comme un numéro de port) au format utilisé sur le réseau, afin qu'il soit compris par toutes les machines, quel que soit leur système.

-------------------------------------------------------------------------------------------------

- bind [ ✅ ]

**Prototype :**

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Paramètres de bind() :**

sockfd :
	Le descripteur de fichier de la socket (créé par socket()).
addr :
	Un pointeur vers une structure contenant l’adresse et le port à associer.
	Généralement, c’est une structure sockaddr_in convertie en type générique sockaddr avec un cast.
addrlen :
	La taille de la structure pointée par addr (en octets).
	Pour une structure sockaddr_in, utilise sizeof(struct sockaddr_in).

**Que fais bind() ?**

1. Associe une socket à une adresse réseau (IP et port) :
    - La socket, créée avec socket(), est une "interface vierge".
    - bind() lui attribue une adresse réseau spécifique 
	(définie dans une structure comme sockaddr_in).
2. Prépare la socket pour écouter sur un port spécifique :
    - Sans bind(), une socket ne peut pas recevoir de connexions 
	ou de données pour un port donné.

**Return value :** 

Succès : Retourne 0.
Échec : Retourne -1 et définit errno 
avec une valeur correspondant à l'erreur (par exemple, port déjà utilisé).

-------------------------------------------------------------------------------------------------

- listen [✅ ]

**Prototype :**

```c
int listen(int sockfd, int backlog);
```

**Paramètres de listen()**

sockfd :
	Le descripteur de fichier de la socket (retourné par socket()).
	Ce doit être une socket déjà configurée avec bind().
backlog :
	Nombre maximum de connexions en attente dans la file d'attente.
	Si la file est pleine, les nouvelles connexions peuvent être rejetées ou ignorées.

**Que fait listen() ?**

Prépare une socket pour accepter les connexions :
- Après avoir associé la socket à une adresse et un port avec bind(), 
tu dois appeler listen() pour indiquer que cette socket est prête à recevoir des connexions entrantes.
- Crée une file d'attente pour les connexions entrantes :
Les connexions entrantes sont mises en file d'attente 
jusqu'à ce qu'elles soient acceptées avec accept().

**valeur de return :**
Succès : Retourne 0.
Échec : Retourne -1 et définit errno avec
une valeur correspondant à l'erreur.

-------------------------------------------------------------------------------------------------

setsockopt [❌]

**Fonctions a comprendre :**

- htonl [ ❌]
-  ntohs [ ❌] 
- ntohl [❌ ]
- inet [ ❌]
- addr [❌ ]
- inet_ntoa [ ❌]
- send [❌ ]
- recv[ ❌]
- lseek [❌ ]
- fstat [❌ ]
- fcntl [❌ ]
- poll [ ❌]
- close [❌ ]
- getsockname [❌]
- getprotobyname [❌]
- gethostbyname [❌]
- getaddrinfo [❌]
- freeaddrinfo [❌]
- connect [❌]
- accept [❌]

 ------------------------------------------------------------------------------------------------

**Commands ::**


◦ KICK - Ejecter un client du channel [❌]
◦ INVITE - Inviter un client au channel [❌]
◦ TOPIC - Modifier ou afficher le thème du channel [❌]
◦ MODE - Changer le mode du channel : [❌]
— i : Définir/supprimer le canal sur invitation uniquement. [❌]
— t : Définir/supprimer les restrictions de la commande TOPIC pour les opé-
rateurs de canaux [❌]
— k : Définir/supprimer la clé du canal (mot de passe) [❌ ]
— o : Donner/retirer le privilège de l’opérateur de canal [ ❌]
— l : Définir/supprimer la limite d’utilisateurs pour le canal [ ❌]


# Sources : 

Videos : 

C'est quoi irc ?
<br>
https://www.youtube.com/watch?v=1jshSVJBm7Q

Installer un server IRC
<br>
https://www.youtube.com/watch?v=_ONwR6CWvE8

Live chat server
<br>
https://www.youtube.com/watch?v=0kU9ql3TWPg

Building a web server in c++ part 1
Ps: C'est long est pas tres agreable a regarder. 
<br>
https://www.youtube.com/watch?v=Kc1kwm1WyVM&list=PLPUbh_UILtZW1spXkwszDBN9YhtB0tFKC

Building a web server in c++ part 2
Ps: C'est long est pas tres agreable a regarder. 
<br>
https://www.youtube.com/watch?v=YqEqjODUkWY&list=PLPUbh_UILtZW1spXkwszDBN9YhtB0tFKC&index=2


Blogs :
<br>
<br>
Creer un server
<br>
https://www.tutorialspoint.com/unix_sockets/what_is_socket.htm

Hexchat pour le client
<br>
https://www.rfc-editor.org/rfc/rfc2812#section-3.2.4
<br>
ecriture pour les clients irc

Medium (tres bonne sources pour setup le projet d'apres moi) <br>
https://medium.com/@afatir.ahmedfatir/small-irc-server-ft-irc-42-network-7cee848de6f9

reactiveso(guide irc) <br>
https://reactive.so/post/42-a-comprehensive-guide-to-ft_irc/
