#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <memory>
#pragma comment(lib, "Ws2_32.lib")

using namespace std::chrono;

class Socket {
private:
    SOCKET m_ConnectSocket = INVALID_SOCKET;
    struct addrinfo* m_result = NULL, hints;
    int m_iResult = 0;

    char* m_addr;
public:
    explicit Socket(char* address): m_addr{address}{
        WSADATA wsa;
        m_iResult = WSAStartup(MAKEWORD(2, 2), &wsa);
        if (m_iResult != 0) {
            std::cerr << "|" <<m_addr << "| WSAStartup failed with error: " << m_iResult << std::endl;
            exit(1);
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
    }

    //Connect to target addr, resolves address before connect. eg: localhost -> 127.0.01
    bool Connect(const int &port) {
        std::string portStr = std::to_string(port);
        m_iResult = getaddrinfo(m_addr, portStr.c_str(), &hints, &m_result);
        if (m_iResult != 0) {
            std::cerr << m_addr << ": getaddrinfo failed with error: " << m_iResult << std::endl;
            WSACleanup();
            return false;
        }

        m_ConnectSocket = socket(m_result->ai_family, m_result->ai_socktype, m_result->ai_protocol);
        if (m_ConnectSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
            freeaddrinfo(m_result);
            WSACleanup();
            return false;
        }

        m_iResult = connect(m_ConnectSocket, m_result->ai_addr, (int)m_result->ai_addrlen);
        if (m_iResult == SOCKET_ERROR) {
            std::cerr << m_addr << ": Connect failed: " << WSAGetLastError() << std::endl;
            closesocket(m_ConnectSocket);
            m_ConnectSocket = INVALID_SOCKET;
            freeaddrinfo(m_result);
            WSACleanup();
            return false;
        }

        freeaddrinfo(m_result);
        return true;
    }

    //Sends message to target
    bool Send(const std::string& message) {
        m_iResult = send(m_ConnectSocket, message.c_str(), (int)message.size(), 0);
        if (m_iResult == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            closesocket(m_ConnectSocket);
            WSACleanup();
            return false;
        }
        return true;
    }

    //Recive function. Checks if response contains 200(OK) statues code
    void CheckCode200(int buffSize) {
        std::unique_ptr<char[]> recvbuf = std::make_unique<char[]>(buffSize);

        ZeroMemory(recvbuf.get(), buffSize);
        m_iResult = recv(m_ConnectSocket, recvbuf.get(), buffSize, 0);
        if (m_iResult > 0) {
            std::string m_result(recvbuf.get(), m_iResult);
            std::size_t found = m_result.find("200");

            if(found){
                std::cout << m_addr << ": OK!\n";
            }else{
                std::cout << "There is no OK code(200) it might be failed.\n";
            }
        }
        else if (m_iResult == 0) {
            std::cerr << m_addr << ": Connection closed\n";
        }
        else {
            std::cerr << m_addr << ": recv failed: " << WSAGetLastError() << std::endl;
        }
    }

    ~Socket(){
        if (m_ConnectSocket != INVALID_SOCKET) {
            closesocket(m_ConnectSocket);
        }
        WSACleanup();
     }
};

std::mutex mtx;

void sendRequest(char* url){
    mtx.lock();
    Socket s(url);
    if (s.Connect(80)) {
        s.Send("GET / HTTP/1.1\r\n\r\nContent-Length: 0\r\n\r\n");
        s.CheckCode200(60);
    }
    mtx.unlock();
}

int main(int argc, char* argv[]) {
    if (argc < 2){
        std::cout << "usage: ./<urlChecker.exe> URL URL...\n";
        return 0;
    }

    auto start = high_resolution_clock::now();

    for (int i = 1; i < argc; i++)
    {
        std::thread t(sendRequest, argv[i]);
        t.join();
    }
    

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    std::cout << "Duration to send " << argc - 1 << " request: " << duration.count() << "ms" << "\n";
    return 0;
}


