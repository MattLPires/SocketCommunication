#undef UNICODE 

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 1024

#define DEFAULT_PORT "27015"

int __cdecl main(void) {
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo hints;
    struct addrinfo *result = NULL;
    

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    printf("Socket TCP/IP - MATHEUS PIRES\n");
    printf("Iniciando biblioteca Winsock\n");

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

    if(iResult != 0) {
        printf("Erro ao iniciar: %d\n", iResult);
        return 1;
    }

    printf("\t\t Lib inicializada!\n");
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    printf("\t-> Resolvendo endereco e porta...\n");
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if(iResult != 0) {
        printf("getaddrinfo falhou com erro: %d\n", iResult);
        WSACleanup();
        return 1;
    } printf("\t\t[+] Sucesso!\n");

    printf("\t-> Criando socket para a conexao...\n");

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(ListenSocket == INVALID_SOCKET) {
        printf("\t\t\t[-] Socket falhou: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    printf("\t\t\t[+] Sucesso!\n");

    printf("\t-> Configurando porta...\n");

    printf("\t-> bind()\n");

    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if(iResult == SOCKET_ERROR) {
        printf("\t\t[-] bind falhou: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("\t\t[+] Sucesso!\n");

    freeaddrinfo(result);

    printf("\t-> Listen()...\n");

    iResult = listen(ListenSocket, SOMAXCONN);
    if(iResult == SOCKET_ERROR) {
        printf("\t\t[-] listen falhou: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("\t\t[+] Sucesso!\n");

    printf("\t->Aguardando conexao...\n");

    ClientSocket = accept(ListenSocket, NULL, NULL);

    if(ClientSocket == INVALID_SOCKET) {
        printf("\t\t[-] Falhou: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("\t\t[+] Sucesso!\n");

    closesocket(ListenSocket);

    printf("\n\t[+] Servidor pronto...\n");

    do {
        printf("\t-> Aguardando...\n");

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if(iResult > 0) {
            printf("\t\t[+] Bytes recebidos: %d\n", iResult);
            printf("\t-> Enviando uma mensagem ao cliente...\n");

            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            if(iSendResult == SOCKET_ERROR) {
                printf("\t\t[-] Error: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }

            printf("\t\t[+] Sucesso! Bytes enviados: %d\n", iSendResult);

        }
        else if(iResult == 0)
                printf("\t\t[-] Conexao fechada!\n");
                else {
                    printf("\t\t[-] recv() falhou: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    WSACleanup();
                    return 1;
                }
    } while (iResult > 0);

    printf("\t-> Encerrando...\n");

    iResult = shutdown(ClientSocket, SD_SEND);
    if(iResult == SOCKET_ERROR) {
        printf("\t\t[-] shutdown falhou: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    printf("\t\t[+] Encerrado!");
    printf("\n\nt### Ate logo!");

    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}