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
<br>

# Socket :

<br>

```c
La socket sert de pont entre le client (par exemple HexChat) et ton serveur IRC. 
Elle permet d'établir une communication bidirectionnelle pour que le serveur et
le client puissent échanger des messages.
En d'autre terme chaque client se sert de la socket pour se connecter au serveur IRC. 
```


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
