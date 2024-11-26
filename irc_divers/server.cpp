#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>

// Classe Client pour représenter chaque client connecté
class Client {
public:
    int fd;                  // Descripteur du socket
    std::string nickname;    // Pseudo du client
    bool authenticated;      // Statut d'authentification

    Client(int fd);
    ~Client();
};

// Classe Server pour gérer les connexions et la logique IRC
class Server {
private:
    int server_fd;                             // Socket du serveur
    std::vector<Client> clients;              // Liste des clients connectés
    std::map<std::string, std::vector<int>> channels; // Canaux et leurs membres
    const int PORT;                           // Port du serveur
    const int BUFFER_SIZE;                    // Taille du buffer

public:
    Server(int port, int buffer_size = 512);
    ~Server();

    void start();                             // Démarre le serveur
    void broadcastMessage(const std::string &message, int sender_fd); // Diffuser un message
    void handleCommand(const std::string &command, int client_fd);    // Traiter une commande IRC
    void removeClient(int client_fd);         // Supprimer un client
};

#endif
