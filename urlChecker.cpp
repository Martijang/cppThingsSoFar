#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#pragma comment(lib, "Ws2_32.lib")

using namespace std::chrono;

class Socket {
private:
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, hints;
    int iResult = 0;

public:
    explicit Socket(){
        WSADATA wsa;
        iResult = WSAStartup(MAKEWORD(2, 2), &wsa);
        if (iResult != 0) {
            std::cout << "WSAStartup failed with error: " << iResult << "\n";
            exit(1);
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
    }

    //Connect to target addr, resolves address before connect. eg: localhost -> 127.0.01
    bool Connect(char* addr, const int &port) {
        std::string portStr = std::to_string(port);
        iResult = getaddrinfo(addr, std::move(portStr.c_str()), &hints, &result);
        if (iResult != 0) {
            std::cout << addr << ": getaddrinfo failed with error: " << iResult << "\n";
            WSACleanup();
            return false;
        }

        ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cout << "Socket creation failed: " << WSAGetLastError() << "\n";
            freeaddrinfo(result);
            WSACleanup();
            return false;
        }

        iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            std::cout << addr << ": Connect failed: " << WSAGetLastError() << "\n";
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            freeaddrinfo(result);
            WSACleanup();
            return false;
        }

        freeaddrinfo(result);
        return true;
    }

    //Sends message to target
    bool Send(const std::string& message) {
        iResult = send(ConnectSocket, message.c_str(), (int)message.size(), 0);
        if (iResult == SOCKET_ERROR) {
            std::cout << "Send failed: " << WSAGetLastError() << "\n";
            closesocket(ConnectSocket);
            WSACleanup();
            return false;
        }
        return true;
    }

    //Recive function. Checks if response contains 200(OK) statues code
    void CheckCode200(int buffSize, char* addr) {
        char* recvbuf = new char[buffSize];

        ZeroMemory(recvbuf, buffSize);

        iResult = recv(ConnectSocket, recvbuf, buffSize, 0);
        if (iResult > 0) {
            std::string result(std::move(recvbuf), iResult);
            std::size_t found = result.find("200");

            if(!found){
                std::cout << "There is no OK code(200) it might be failed.\n";
            }else{
                std::cout << addr << ": OK!\n";
            }
        }
        else if (iResult == 0) {
            std::cout << addr << ": Connection closed\n";
        }
        else {
            std::cout << addr << ": recv failed: " << WSAGetLastError() << "\n";
        }
        delete[] recvbuf;
    }

    ~Socket(){
        if (ConnectSocket != INVALID_SOCKET) {
            closesocket(ConnectSocket);
        }
        WSACleanup();
     }
};

std::mutex mtx;
/*without mutex, error occurs
error happened cuz of e.com affected google.com url 
eg: ./urlChecker.exe www.google.com e.com
e.com: getaddrinfo failed with error: 11001
www.google.com: Connect failed: 10038
Duration to send 2 request: 55ms
*/

//with mutex threads cannot share resources.

void send_request(char* url){
    mtx.lock();
    Socket s;
    //s.SockInit();
    if (s.Connect(url, 80)) {
        s.Send("GET / HTTP/1.1\r\n\r\nContent-Length: 0\r\n\r\n");
        s.CheckCode200(60, url); // 60 byte is enough to find http code 200 from response
    }
    mtx.unlock();
}

int main(int argc, char* argv[]) {
    if (argc < 2){
        std::cout << "usage: ./<name.exe> URL URL...\n";
        return 0;
    }

    auto start = high_resolution_clock::now();

    std::vector<std::thread> Tvector;
    for (size_t i = 1; i < argc; i++){
        std::thread t1(std::move(send_request), argv[i]);
        Tvector.push_back(std::move(t1));
    }
    
    for (std::thread &v: Tvector){
        v.join();
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    std::cout << "Duration to send " << argc - 1 << " request: " << duration.count() << "ms" << "\n";
    return 0;
}
