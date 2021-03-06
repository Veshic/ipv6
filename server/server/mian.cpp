// server.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma  comment(lib,"ws2_32.lib")

#include "../../common/utility/DebugLog.h"

class WSA
{
public:
    static void WSAStartup()
    {
        WSADATA wsaData;
        if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
        {
            Debug::Log("WSAStartup failed");
            return;
        }
    }

    static void WSACleanup()
    {
        if (::WSACleanup())
        {
            Debug::Log("WSACleanup failed");
            return;
        }
    }
};

int main()
{
    WSA::WSAStartup();

    SOCKET sockListen;
    SOCKET sockClient;
    struct addrinfo hints = {};
    struct addrinfo* res = nullptr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6; //AF_UNSPEC
    hints.ai_socktype = SOCK_STREAM;

    hints.ai_flags = AI_PASSIVE;
    /*
    AI_PASSIVE      ソケットアドレスをbind()する場合に指定。
    AI_CANONNAME    ホストの正式名を要求する。
    AI_NUMERICHOST  nodenameは数値形式のネットワークアドレスに限る。ホスト名の名前解決は行わない。
    AI_NUMERICSERV  servnameはポート番号の文字列に限る。サービス名の解決は行わない.※Windows では, この定数はない。
    AI_V4MAPPED     ai_familyがAF_INET6でIPv6アドレスが見つからなかった場合は、IPv4 - mapped IPv6アドレスを返す。ai_familyがAF_INET6でない場合、AI_V4MAPPEDは無視される。
    AI_ALL          AI_V4MAPPEDフラグとともに指定された場合、getaddrinfo()はIPv4, IPv6アドレスの両方を返す。AI_V4MAPPEDフラグなしに指定した場合、AI_ALLフラグは無視される。
    AI_ADDRCONFIG   ローカルシステムがIPv4アドレスを持つ場合に限りIPv4アドレスを取得し、ローカルシステムがIPv6アドレスを持つ場合に限りIPv6アドレスを取得する。
    */

    //ポート番号かサービス名 httpとか
    const char* port = "12345";
    const char* hostname = "localhost";
    //const char* port = "http";
    //const char* hostname = "www.v6pc.jp";
    {
        int ret = getaddrinfo(hostname, port, &hints, &res);
        if (ret != 0)
        {
            Debug::Log("getaddrinfo failed: ", gai_strerror(ret));
            return -1;
        }
    }
    Debug::Log("%s",res->ai_addr->sa_data);

    sockListen = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockListen < 0)
    {
        Debug::Log("create socket failed");
        return -1;
    }

    {
        int ret = bind(sockListen, res->ai_addr, (int)res->ai_addrlen);
        if (ret != 0)
        {
            Debug::Log("getaddrinfo failed: ", gai_strerror(ret));
            return -1;
        }
    }
    freeaddrinfo(res);

    {
        struct sockaddr_storage client;
        socklen_t len = sizeof(client);;
        int ret = listen(sockListen, 3);

        Debug::Log("begin accept");
        sockClient = accept(sockListen, (struct sockaddr*)&client, &len);
        Debug::Log("success accept");
    }

    send(sockClient, "HELLO\n", 6, 0);
    char buf[1024] = {};
    while (true)
    {
        int ret = recv(sockClient, buf, sizeof(buf), 0);
        if (ret == -1)
        {
            Debug::Log("recv failed ");
            break;
        }
        if (strcmp(buf, "end") == 0)break;
        int size = ret;
        Debug::Log("recv: %s", buf);
        ret = send(sockClient, buf, size, 0);
        ZeroMemory(&buf, sizeof(buf));
    }
    Debug::Log("Connection end");

    closesocket(sockClient);
    closesocket(sockListen);

    WSA::WSACleanup();
    getchar();
    return 0;
}