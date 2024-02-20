#include "userinterface.h"
#include "Json.h"
#include "tcpclient.h"
#include <cstdlib>
#include <QCoreApplication>
#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>
#include <QInputDialog>
#include <QApplication>
#include <QtWidgets>

Userinterface::Userinterface(TcpClient * client) : Client(client) {

    // Create main window
    window.setWindowTitle("P2P Chat");
    //window.setFixedSize(800, 500);

    // Create widgets
    QLabel* outputLabel = new QLabel("Enter message:");
    QLineEdit* inputLineEdit = new QLineEdit();
    QPushButton* sendButton = new QPushButton("Send");
    QLabel* inputLabel = new QLabel("Chat:");
    QTextBrowser* receivedTextBrowser = new QTextBrowser();
    receivedTextBrowser->setReadOnly(true);
    receivedTextBrowser->document()->setMaximumBlockCount(100); // Set maximum block count using QTextDocument
    QLabel* debugLabel = new QLabel("Debug messages:");
    QPlainTextEdit* debugTextEdit = new QPlainTextEdit();
    debugTextEdit->setReadOnly(true);
    debugTextEdit->setMaximumHeight(110);
    debugTextEdit->document()->setMaximumBlockCount(100); // Set maximum block count using QTextDocument
    debugTextEdit->setStyleSheet("color: red;");

    // Create layout
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(outputLabel);
    layout->addWidget(inputLineEdit);
    layout->addWidget(sendButton);
    layout->addWidget(inputLabel);
    layout->addWidget(receivedTextBrowser);
    layout->addWidget(debugLabel);
    layout->addWidget(debugTextEdit);

    window.setLayout(layout);



    QObject::connect(client, &TcpClient::newConnection, [client, debugTextEdit](QTcpSocket *socket)
                     {
                         qDebug() << "New connection from: " << socket->peerAddress().toString();
                         debugTextEdit->appendPlainText("New connection from: " + socket->peerAddress().toString());
                         QObject::connect(socket, &QTcpSocket::readyRead, client, &TcpClient::readFromAll);
                     });

    QObject::connect(client, &TcpClient::newMessageReceived, [receivedTextBrowser](QString message)
                     {

                         qDebug() << "New message received: " << message;
        receivedTextBrowser->append(JSONtoMessage(message));
                     });


    auto messageProcessingFunc = [this, debugTextEdit, receivedTextBrowser, inputLineEdit]
    {
        QString message = inputLineEdit->text();
        if (message.isEmpty())
        {
            // Show message box with error
            QMessageBox::warning(&window, "Error", "Message cannot be empty!");
            return;
        }

        //intercept for json
        message = createJSON(Client->getNickName(), Client->getIP(), Client->getPort(), message );
        receivedTextBrowser->append(JSONtoMessage(message));
        qDebug() << "Message sent: " << message;
        debugTextEdit->appendPlainText("Message sent: " + message);

        Client->sendToAll(message);
        inputLineEdit->clear();
    };

    QObject::connect(sendButton, &QPushButton::clicked, messageProcessingFunc);
    QObject::connect(inputLineEdit, &QLineEdit::returnPressed, messageProcessingFunc);
    // Show window
    window.show();


}

void Userinterface::QuitError(QString Error){
    qWarning() << "ERROR: "<< Error;
    QMessageBox msgBox;
    msgBox.setWindowTitle("Error");
    msgBox.setText(Error);
    msgBox.setIcon(QMessageBox::Critical);
    QAbstractButton* pButtonExit = msgBox.addButton("Exit", QMessageBox::YesRole);
    msgBox.exec();

    if (msgBox.clickedButton()==pButtonExit) {
        std::exit(1);
    }

}
