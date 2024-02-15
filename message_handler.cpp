#include "message_handler.h"

void Peer2Peer::message_handler::setIP(string IP) { this->ipAdress = IP; }

string Peer2Peer::message_handler::getIP() { return this->ipAdress; }

void Peer2Peer::message_handler::setID(int ID) { this->clientID = ID; }

int Peer2Peer::message_handler::getID() { return this->clientID; }

void Peer2Peer::message_handler::setPort(int Port) { this->clientPort = Port; }

int Peer2Peer::message_handler::getPort() { return this->clientPort; }

QJsonObject Peer2Peer::message_handler::readJsonMessage(const string &message)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(QString::fromStdString(message).toUtf8());
    QJsonObject jsonObject;

    if (!jsonDocument.isNull()) {
        jsonObject = jsonDocument.object();

        // Looking for the required objects
        int received_id = jsonObject["id"].toInt();
        QString ip_str = jsonObject["ip"].toString();
        int received_port = jsonObject["port"].toInt();

        // Converting QString to string
        string received_ip = ip_str.toStdString();

        // Setting the variables up to date
        setPort(received_port);
        setID(received_id);
        setIP(received_ip);

    } else {
        // Failed with the readings or parser
        qDebug() << "Error while reading the JSON-Message.";
    }

    return jsonObject;
}

//  // json structure
//     string jsonString = R"({
//         "id": 101,
//         "ip": "192.168.1.100",
//         "port": 8080
//     })";

//     // working with namespace
//     Peer2Peer::message_handler *client_information = new Peer2Peer::message_handler();
//     client_information->readJsonMessage(jsonString);

//     // getters are ready to use for future work
//     cout << "ID: " << client_information->getID() << "\n";
//     cout << "IP: " << client_information->getIP() << "\n";
//     cout << "Port: " << client_information->getPort() << "\n";