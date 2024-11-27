#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>

// Classe pour représenter un client connecté
class Client {
public:
    int fd;                  // Descripteur du socket du client
    std::string nickname;    // Pseudo du client
    bool authenticated;      // Indique si le client est authentifié

    Client(int fd);          // Constructeur
    ~Client();               // Destructeur
};

// Classe principale pour gérer le serveur IRC
class Server {
private:
    int server_fd;                             // Descripteur du socket serveur
    std::vector<Client> clients;              // Liste des clients connectés
    std::map<std::string, std::vector<int>> channels; // Canaux (nom -> sockets des membres)
    const int PORT;                           // Port d'écoute
    const int BUFFER_SIZE;                    // Taille maximale des messages

public:
    Server(int port, int buffer_size = 512);  // Constructeur
    ~Server();                                // Destructeur

    void start();                             // Méthode principale pour démarrer le serveur
    void broadcastMessage(const std::string &message, int sender_fd); // Diffuser un message
    void handleCommand(const std::string &command, int client_fd);    // Traiter une commande IRC
    void removeClient(int client_fd);         // Supprimer un client
};

#endif
