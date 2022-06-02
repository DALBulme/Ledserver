#include "ledserver.h"
#include "config.h"



LedServer::LedServer(quint16 port, QObject *parent) : QObject(parent), m_port(port)
{
    m_gpio = new Gpio(this);
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &LedServer::myNewConnection);
}

void LedServer::start()
{
    m_server->listen(QHostAddress::Any, PORT);
}

// Client hat Verbindung zum Server aufgebaut
void LedServer::myNewConnection()
{
    m_socket = m_server->nextPendingConnection();

    connect(m_socket, &QTcpSocket::readyRead, this, &LedServer::myServerRead);              // verbindet mit myServerRead
    connect(m_socket, &QTcpSocket::disconnected, this, &LedServer::myClientDisconnect);     // Trennt die Verbindung

    m_socket->write("Bitte geben Sie eine Zahl zwischen 0 und 15 ein: ");                   // Anweisung an Benutzer
    qDebug() << "Verbindung mit Benutzer hergestellt.";
}

// Client hat Verbindung zum Server getrennt
void LedServer::myClientDisconnect()
{
    for (int i = 0; i < 4; i++)                                                             // LEDs auschchalten
    {
        m_gpio->set(LEDS[i], 0);
    }
    qDebug() << "Verbindung wurde vom Benutzer getrennt.";
    m_socket->close();                                                                      // trennt verbindung

}

// Client hat eine Zahl (0...15) zum Server gesendet
void LedServer::myServerRead()
   {
    while (m_socket->bytesAvailable()) {
        QString message = m_socket->readAll();                                              // Input von Benutzer
        int c = message.toInt();
        qDebug() << "Von Benutzer gewählte Zahl: " + QString::number(c);
        if (c >= 0 && c <= 15)
        {
            qDebug() << "Zahl wird angezeigt.";
            m_socket->write("Zahl wird angezeigt.\n");                                      // Input an LEDs anzeigen
            m_gpio->set(c);
        }
        else
        {
            m_socket->write("Input kann nicht angezeigt werden!\n");
        }
    }
        m_socket->write("Bitte geben Sie eine Zahl zwischen 0 und 15 ein: ");

   }

