//----------------------------------------------------------------------------------------------------------------
// Name         : umurzadi_TCP_server.cpp
// Author       : Adil Umurzakov
// Version      : 1
// Copyright    : Adil Umurzakov student Faculty of Information Technology CTU in Prague (username: umurzadi) 
// Description  : TCP-server
//----------------------------------------------------------------------------------------------------------------
#include <iostream>
using namespace std;

#include <sstream>
#include <cmath>
#include <sys/types.h>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>     // socket(), bind(), socket(), listen()
#include <unistd.h>         // close(), read(), write()
#include <netinet/in.h>     // struct sockaddr_in
#include <string.h>         // bzero()
#include <sys/wait.h>       // waitpid()
#include <netdb.h>
#include <string>
#include <arpa/inet.h>

#define PORT 3999
#define BUFFER_SIZE 10000
#define TIMEOUT 1
#define TIMEOUT_RECHARGING 5

// ERROR--------------------------------------------------------------------------------------------------------
void login_error(int socket){
    send (socket, "300 LOGIN FAILED\a\b", 18, 0);
    cout << "300 LOGIN FAILED\a\b\n";  
    close(socket); 
}

void syntax_error(int socket){
    send (socket, "301 SYNTAX ERROR\a\b", 18, 0);
    cout << "301 SYNTAX ERROR\a\b\n";
    close(socket);
}

void key_error(int socket){
    send (socket, "303 KEY OUT OF RANGE\a\b", 22, 0);
    cout << "303 KEY OUT OF RANGE\a\b\n";
    close(socket);
}
// LOGOUT-------------------------------------------------------------------------------------------------------
void logout(int socket){
    cout << "106 LOGOUT\a\b\n";
    send (socket, "106 LOGOUT\a\b", 12, 0);
    close (socket);
}
// SEGMENTATION-------------------------------------------------------------------------------------------------
string segmentation(char* buffer, int size){
    string message = "";
    for (int i = 0; i < size; i++) {
        message = message + buffer[i];
    }
    return message;
}
// DELIMITER----------------------------------------------------------------------------------------------------
int find_delimiter(string message, int* size){
    for(int i = 0; i < message.size(); i++){
        if(message[i] == '\b' && message[i-1] == '\a'){
            *size = i+1;
            return 1;
        }
    }
    return 0;
}
// RECHARGING---------------------------------------------------------------------------------------------------
int recharging(int socket, string* message, int number_of_letter){
    struct timeval timeout_recharging;
    timeout_recharging.tv_sec = TIMEOUT_RECHARGING;
    timeout_recharging.tv_usec = 0;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout_recharging, sizeof(timeout_recharging));
    char buffer[BUFFER_SIZE];
    while(true){
        if(find_delimiter(*message, &number_of_letter)) break;
        int bytesRead = recv(socket, buffer, BUFFER_SIZE, 0); // <-- recv(reading) function will wait until any character appears
        if (bytesRead <= 0){
            perror("Can't read a socket: ");
            close(socket);
            return -3;
        }
        cout << buffer << endl;
        buffer[bytesRead] = '\0';
        *message += segmentation(buffer, bytesRead);
        if(find_delimiter(*message, &number_of_letter)) break;
        if((*message).size() >= 12) {
            syntax_error(socket);
            return 1;
        }
    }
    if(    (*message)[0] == 'F' 
        && (*message)[1] == 'U' 
        && (*message)[2] == 'L' 
        && (*message)[3] == 'L'  
        && (*message)[4] == ' ' 
        && (*message)[5] == 'P'  
        && (*message)[6] == 'O'  
        && (*message)[7] == 'W'  
        && (*message)[8] == 'E'  
        && (*message)[9] == 'R') {
        (*message).erase(0, number_of_letter);
        struct timeval timeout;
        timeout.tv_sec = TIMEOUT;
        timeout.tv_usec = 0;
        setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
        return 0;
    }
    else {
        send (socket, "302 LOGIC ERROR\a\b", 17, 0);
        cout << "302 LOGIC ERROR\a\b\n";
        close(socket);
        return 1;
    }
}
// LAST MESSAGE-------------------------------------------------------------------------------------------------
int last_message(int socket, string* message){
    int number_of_letter;    
    char buffer[BUFFER_SIZE]; // field of characters
    int res;
    string command;
    while(true){
        if(find_delimiter(*message, &number_of_letter)){
            if((*message)[0] == 'R'){
                command = *message;
                command.erase(number_of_letter - 2, command.size() - (number_of_letter - 2));
                if (command == "RECHARGING"){
                    (*message).erase(0, number_of_letter);
                    recharging(socket, message, number_of_letter);
                }
            }
            else break;
        }
        int bytesRead = recv(socket, buffer, BUFFER_SIZE, 0); // <-- recv(reading) function will wait until any character appears
        if (bytesRead <= 0){
            perror("Can't read a socket: ");
            close(socket);
            return -3;
        }
        buffer[bytesRead] = '\0';
        *message += segmentation(buffer, bytesRead);
        if(find_delimiter(*message, &number_of_letter)){
            if((*message)[0] == 'R'){
                command = *message;
                command.erase(number_of_letter - 2, command.size() - (number_of_letter - 2));
                if (command == "RECHARGING"){
                    (*message).erase(0, number_of_letter);
                    recharging(socket, message, number_of_letter);
                }
                else break;
            }
            else break;
        }
        if((*message).size() >= 100) {
            syntax_error(socket);
            return 1;
        }
    }
    logout(socket);
    return -1;
}

// CIN (MESSAGE TYPE: OK 'NUM' 'NUM'\A\B)-------------------------------------------------------------------
int assignment(int* x, int* y, string* message, int number_of_letter, int socket);

int get_message(int* x, int* y, int socket, string* message){
    int number_of_letter;    
    char buffer[BUFFER_SIZE]; // field of characters
    while(true){
        if(find_delimiter(*message, &number_of_letter) == 1) break;
        int bytesRead = recv(socket, buffer, BUFFER_SIZE, 0); // <-- recv(reading) function will wait until any character appears
        if (bytesRead <= 0){
            perror("Can't read a socket: ");
            close(socket);
            return -3;
        }
        buffer[bytesRead] = '\0';
        *message += segmentation(buffer, bytesRead);
        if(find_delimiter(*message, &number_of_letter)) break;
        if((*message).size() >= 12) {
            syntax_error(socket);
            return 1;
        }
    }
    for(int i = 0; i < number_of_letter; i++) {
        cout << (*message)[i];
    }
    cout << endl;
    if(assignment(x, y, message, number_of_letter, socket)) return 1;
    cout << *message << endl;
    return 0;
}

int assignment(int* x, int* y, string* message, int number_of_letter, int socket){
    string copy = *message;
    copy.erase(number_of_letter - 2, copy.size() - (number_of_letter - 2)); 
    if(copy[0] == 'O' && copy[1] == 'K'){
        int number_of_spaces = 0;
        for(int i = 0; i < number_of_letter - 2; i++){
            if(copy[i] == ' ')number_of_spaces++;
        }
        stringstream ss(copy);
        int max_words = 3;
        string word[max_words];
        for (int i = 0; i < max_words; i++){
            ss >> word[i];
        } 
        if(number_of_spaces != 2) {
            syntax_error(socket);
            return 1;
        }
        char arr1[word[1].length()];
        strcpy(arr1,word[1].c_str());
        for (int i = 0; i < word[1].length(); i++){
            if(isalpha(arr1[i]) || arr1[i] == '.'){
                syntax_error(socket);
                return -1;
                break;
            } else {
                *x = stoi(word[1]);
            }
        }
        char arr2[word[2].length()];
        strcpy(arr2,word[2].c_str());
        for (int i = 0; i < (word[2].length()); i++){
            if(isalpha(arr2[i]) || arr2[i] == '.'){
                syntax_error(socket);
                return -1;
                break;
            } else {
                *y = stoi(word[2]);
            }
        }
        (*message).erase(0, number_of_letter);
        return 0;
    } 
    else if(copy[0] == 'R'){
        (*message).erase(0, number_of_letter);
        if(recharging(socket, message, number_of_letter)) return 1;
        if(get_message(x, y, socket, message)) return 1;
        return 0;
    }
    else {
        syntax_error(socket);
        return 1;
    }
}

// ROBOT MOVING--------------------------------------------------------------------------------------
class robot{
private:
    int hits = 20;
public:
    int x, y, socket;
    string direction; 
    
    bool compare(int x1, int y1){
        if(x1 == x && y == y1) return true;
        else {
            return false;
        }
    }

    bool hits_down(){
        hits--;
        if(hits == 0){
            close(socket);
            return true;
        }
        return false;
    }

    void define_direction(int x1, int y1){
        if(!((x - x1 == 1 && y1 == y) || (x - x1 == -1 && y1 == y) || (x1 == x && y - y1 == 1) || (x1 == x && y - y1 == -1))) {
            direction = "Error";
            return;
        }
        if(x1 == x){
            if(y1 < y) 
                direction = "UP";
            else
                direction = "down";
        }
        else{
            if(x1 < x)
                direction = "righT";
            else 
                direction = "Left";
        }
        return;
    }

    int turn_x(string* message){
        int check_x;
        int check_y;
        if(x > 0){
            if(direction == "down"){
                cout << "104 TURN RIGHT\a\b" << endl;
                send (socket, "104 TURN RIGHT\a\b", 16, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                direction = "Left";
                return 0;
            }
            else if( direction == "UP"){
                cout << "103 TURN LEFT\a\b" << endl;
                send (socket, "103 TURN LEFT\a\b", 15, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                direction = "Left";
                return 0;
            }
            else if((direction) == "righT"){
                cout << "103 TURN LEFT\a\b" << endl;
                send (socket, "103 TURN LEFT\a\b", 15, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                cout << "103 TURN LEFT\a\b" << endl;
                send (socket, "103 TURN LEFT\a\b", 15, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                direction = "Left";
                return 0;
            }
        }
        else if(x < 0){
            if(direction == "UP"){
                cout << "104 TURN RIGHT\a\b" << endl;
                send (socket, "104 TURN RIGHT\a\b", 16, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                direction = "righT";
                return 0;
            }
            else if(direction == "down"){
               cout << "103 TURN LEFT\a\b" << endl;
                send (socket, "103 TURN LEFT\a\b", 15, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                direction = "righT";
                return 0;
            }
            else if((direction) == "Left"){
                cout << "103 TURN LEFT\a\b" << endl;
                send (socket, "103 TURN LEFT\a\b", 15, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                cout << "103 TURN LEFT\a\b" << endl;
                send (socket, "103 TURN LEFT\a\b", 15, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                direction = "righT";
                return 0;
            }
        }
        return 0;
    }

    int turn_y(string* message){
        int check_x;
        int check_y;
        if(y > 0){
            if(direction == "UP"){
                cout << "103 TURN LEFT\a\b" << endl;
                send (socket, "103 TURN LEFT\a\b", 15, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                cout << "103 TURN LEFT\a\b" << endl;
                send (socket, "103 TURN LEFT\a\b", 15, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                direction = "down";
                return 0;
            }
            else if((direction) == "Left"){
                cout << "103 TURN LEFT\a\b" << endl;
                send (socket, "103 TURN LEFT\a\b", 15, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                direction = "down";
                return 0;
            }
            else if(direction == "righT"){
                cout << "104 TURN RIGHT\a\b" << endl;
                send (socket, "104 TURN RIGHT\a\b", 16, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                direction = "down";
                return 0;
            }   
        }
        else if (y < 0){
            if(direction == "down"){
                cout << "103 TURN LEFT\a\b" << endl;
                send (socket, "103 TURN LEFT\a\b", 15, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                cout << "103 TURN LEFT\a\b" << endl;
                send (socket, "103 TURN LEFT\a\b", 15, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                direction = "UP";
                return 0;
            }
            else if((direction) == "Left"){
                cout << "104 TURN RIGHT\a\b" << endl;
                send (socket, "104 TURN RIGHT\a\b", 16, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                direction = "UP";
                return 0;
            }
            else if(direction == "righT"){
                cout << "103 TURN LEFT\a\b" << endl;
                send (socket, "103 TURN LEFT\a\b", 15, 0);
                if(get_message( &check_x, &check_y, socket, message)) return 1;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                direction = "UP";
                return 0;
            }
        }
        return 0;
    }

    int move_x(string* message){
        int check_x;
        int check_y;
        cout << "102 MOVE\a\b" << endl;
        send (socket, "102 MOVE\a\b", 10, 0);
        if(get_message( &check_x, &check_y, socket, message)) return 1;
        if(direction == "righT"){
            if (compare(check_x, check_y)){
                if(y != 0) {
                    if(turn_y(message)) return 1;
                    return 0;
                }
                else {
                    cout << "103 TURN LEFT\a\b" << endl;
                    send (socket, "103 TURN LEFT\a\b", 15, 0);
                    if(get_message( &check_x, &check_y, socket, message)) return 1;
                    if (!compare(check_x, check_y)){
                        return 1;
                    }
                    direction = "UP";
                }
                move_y(message);
                if(turn_x(message)) return 1;
                if(hits_down()){
                    return 1;
                }
                return 0;
            }
            else{ 
                x++;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                return 0;
            }
        }
        else if(direction == "Left"){
            if (compare(check_x, check_y)){
                if(y != 0) {
                    if(turn_y(message)) return 1;
                    return 0;
                }
                else {
                    cout << "103 TURN LEFT\a\b" << endl;
                    send (socket, "103 TURN LEFT\a\b", 15, 0);
                    if(get_message( &check_x, &check_y, socket, message)) return 1;
                    if (!compare(check_x, check_y)){
                        return 1;
                    }
                    direction = "down";
                }
                move_y(message);
                if(turn_x(message)) return 1;
                if(hits_down()){
                    return 1;
                }
                return 0;
            }
            else{
                x--;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                return 0;
            }
        }
        return 0;
    }

    int move_y(string* message){
        int check_x;
        int check_y;
        cout << "102 MOVE\a\b" << endl;
        send (socket, "102 MOVE\a\b", 10, 0);
        if(get_message( &check_x, &check_y, socket, message)) return 1;
        if(direction == "UP"){
            if(compare(check_x, check_y)){
                if(x != 0) {
                    if(turn_x(message)) return 1;
                }
                else {
                    cout << "103 TURN LEFT\a\b" << endl;
                    send (socket, "103 TURN LEFT\a\b", 15, 0);
                    if(get_message( &check_x, &check_y, socket, message)) return 1;
                    if (!compare(check_x, check_y)){
                        return 1;
                    }
                    direction = "Left";
                }
                move_x(message);
                if(turn_y(message)) return 1;
                if(hits_down()){
                    return 1;
                }
                return 0;
            }
            else{ 
                y++;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                return 0;
            }
        }
        else if(direction == "down"){
            if (compare(check_x, check_y)){
                if(x != 0) {
                    if(turn_x(message)) return 1;
                }
                else {
                    cout << "103 TURN LEFT\a\b" << endl;
                    send (socket, "103 TURN LEFT\a\b", 15, 0);
                    if(get_message( &check_x, &check_y, socket, message)) return 1;
                    if (!compare(check_x, check_y)){
                        return 1;
                    }
                    direction = "righT";
                }
                move_x(message);
                if(turn_y(message)) return 1;
                if(hits_down()){
                    return 1;
                }
                return 0;
            }
            else{
                y--;
                if (!compare(check_x, check_y)){
                    return 1;
                }
                return 0;
            }
        }
        return 0;
    }

};


// MAIN-----------------------------------------------------------------------------------------------
int main () 
{
    // listening socket
    int listening = socket(AF_INET, SOCK_STREAM, 0); // AF_INET(Address Family Internet(IPv4)), SOCK_STREAM = TCP (SOCK_DGRAM = UDP)
    if (listening < 0){
        perror("Can't create a socket!");
        return -1;
    }

    // sockaddr class
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;                   //PF_INET(protocol) = AF_INET(address)
    address.sin_port = htons(PORT);                 // htons = host to network shorts
    address.sin_addr.s_addr = htonl(INADDR_ANY);    // htonl = host to network long // listen everywhere

    if (bind(listening,(struct sockaddr*)&address, sizeof(address)) == -1){
        perror("Can't bind: ");
        close(listening);
        return -1;
    }

    if (listen(listening, 10) < 0){
        perror("Can't listen: ");
        close(listening);
        return -1;
    }

    struct sockaddr_in remote_address;
    socklen_t size;

    while(true){
        int socket = accept(listening, (struct sockaddr*) &remote_address, &size);
        if (socket < 0){
            perror("Accept error!");
            close(listening);
            return -1;
        }

        pid_t pid = fork();         // <--using this means creating a copy of process (one or more copy), which are on a same position as original process(parent process)
        if (pid == 0){              // <--this means we are on a child process
            close(listening);       // <--we close listenning socket because at the when moment we created a child process, we also created a copy of listening socket,
                                    // which we don't need
                                    // for continue we need only socket socket
            fd_set sockets;
            int retval;             // return value
            while(true){
                // DEFAULT TIMEOUT----------------------------------------------------------------------------------
                struct timeval timeout;
                timeout.tv_sec = TIMEOUT;
                timeout.tv_usec = 0;
                if (setsockopt (socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof (timeout)) < 0){
                    perror("setsockopt failed\n");
                }
                if (setsockopt (socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof (timeout)) < 0){
                    perror("setsockopt failed\n");
                }

                // START PROGRAM-------------------------------------------------------------------------------------
                char buffer[BUFFER_SIZE]; // field of characters <<<<<<<<< here must be 22 characters
                int number_of_letter;
                int bytesRead;
                string message = "";
                int res;
                string command;
                // RECIVING MESSAGES(USERNAME)----------------------------------------------------------------------------------
                while(true){
                    bytesRead = recv(socket, buffer, BUFFER_SIZE, 0); // <-- recv(reading) function will wait until any character appears
                    cout << buffer << endl;
                    if (bytesRead <= 0){
                        perror("Can't read a socket: ");
                        close(socket);
                        return -3;
                    }
                    buffer[bytesRead] = '\0';
                    message += segmentation(buffer, bytesRead);
                    cout << message << endl;
                    if(find_delimiter(message, &number_of_letter)){
                        if(message[0] == 'R'){
                            command = message;
                            command.erase(number_of_letter - 2, command.size() - (number_of_letter - 2));
                            if (command == "RECHARGING"){
                                message.erase(0, number_of_letter);
                                recharging(socket, &message, number_of_letter);
                            }
                        }
                        else break;
                    }
                    if (message.size() >= 20){
                        syntax_error(socket);
                        return 1;
                    }
                }
                if (number_of_letter > 20){
                    syntax_error(socket);
                    return 1;
                }
                // CALCULATE HASH-------------------------------------------------------------------------------------------------------
                int hash = 0, a;
                for(int i = 0; i < number_of_letter - 2; i++){
                    a = message[i];
                    hash += a;
                }
                hash = ((hash * 1000) % 65536);
                message.erase(0, number_of_letter);
                cout << message << endl;
                // KEY REQUEST----------------------------------------------------------------------------------------------------------
                cout << "107 KEY REQUEST\a\b" << endl;
                send (socket, "107 KEY REQUEST\a\b", 17, 0);
                int server_key[5] = {23019, 32037, 18789, 16443, 18189}; //server keys
                // RECIVING MESSAGES(KEY REQUEST)----------------------------------------------------------------------------------------------------
                while(true){
                    if(find_delimiter(message, &number_of_letter)){
                        if(message[0] == 'R'){
                            command = message;
                            command.erase(number_of_letter - 2, command.size() - (number_of_letter - 2));
                            if (command == "RECHARGING"){
                                message.erase(0, number_of_letter);
                                recharging(socket, &message, number_of_letter);
                            }
                        }
                        else break;
                    }
                    bytesRead = recv(socket, buffer, BUFFER_SIZE, 0); // <-- recv(reading) function will wait until any character appears
                    cout << buffer << endl;
                    if (bytesRead <= 0){
                        perror("Can't read a socket: ");
                        close(socket);
                        return -3;
                    }
                    buffer[bytesRead] = '\0';
                    message += segmentation(buffer, bytesRead);
                    if(find_delimiter(message, &number_of_letter)){
                        if(message[0] == 'R'){
                            command = message;
                            command.erase(number_of_letter - 2, command.size() - (number_of_letter - 2));
                            if (command == "RECHARGING"){
                                message.erase(0, number_of_letter);
                                recharging(socket, &message, number_of_letter);
                            }
                        }
                        else break;
                    }
                }
                if (number_of_letter > 6){
                    syntax_error(socket);
                    break;
                }
                int key = message[0] - '0';   //convert char to int
                if(key < 0 || key > 9){
                    syntax_error(socket);
                    break;
                } else if(key > 4){
                    key_error(socket);
                    break;
                }
                // CALCULATE SERVER_CONFIRMATION----------------------------------------------------------------------------------------------------
                int server_conf = (hash + server_key[key]) % 65536;
                string something = to_string(server_conf);
                something += "\a\b";
                char send_server_conf[something.length()];
                strcpy(send_server_conf, something.c_str());
                send (socket, send_server_conf, sizeof(send_server_conf), 0);                
                message.erase(0, number_of_letter);
                cout << message << endl;
                // RECIVING MESSAGE(CLIENT_CONFIRMATION)--------------------------------------------------------------------------------------------
                while(true){
                    if(find_delimiter(message, &number_of_letter)){
                        if(message[0] == 'R'){
                            command = message;
                            command.erase(number_of_letter - 2, command.size() - (number_of_letter - 2));
                            if (command == "RECHARGING"){
                                message.erase(0, number_of_letter);
                                recharging(socket, &message, number_of_letter);
                            }
                        }
                        else break;
                    }
                    bytesRead = recv(socket, buffer, BUFFER_SIZE, 0); // <-- recv(reading) function will wait until any character appears
                    cout << buffer << endl;
                    if (bytesRead <= 0){
                        perror("Can't read a socket: ");
                        close(socket);
                        return -3;
                    }
                    buffer[bytesRead] = '\0';
                    message += segmentation(buffer, bytesRead);
                    if(find_delimiter(message, &number_of_letter)){
                        if(message[0] == 'R'){
                            command = message;
                            command.erase(number_of_letter - 2, command.size() - (number_of_letter - 2));
                            if (command == "RECHARGING"){
                                message.erase(0, number_of_letter);
                                recharging(socket, &message, number_of_letter);
                            }
                        }
                        else break;
                    }
                }
                if (number_of_letter > 7){
                    syntax_error(socket);
                    break;
                }
                string numbers = "";
                for (int i = 0; i < (number_of_letter - 2); i++){
                    if(message[i] == ' '){
                        syntax_error(socket);
                        break;
                    }
                    numbers += message[i];
                }
                // CALCULATE CLIENT_CONFIRMATION
                int client_conf = stoi(numbers);
                cout << client_conf << endl;
                int client_key[5] = {32037, 29295, 13603, 29533, 21952}; //client key
                int confirm = (hash + client_key[key]) % 65536;
                if(client_conf != confirm){
                    login_error(socket);
                    break;
                }
                cout << "200 OK\a\b" << endl;
                send (socket, "200 OK\a\b", 8, 0);
                message.erase(0, number_of_letter);
                cout << message << endl;
                // START MOVING------------------------------------------------------------------------------------------------------------
                robot adil;
                int x, y; 
                adil.socket = socket;
                cout << "102 MOVE\a\b" << endl;
                send (socket, "102 MOVE\a\b", 10, 0);
                if(get_message( &x, &y, socket, &message)) break;
                cout << "102 MOVE\a\b" << endl;
                send (socket, "102 MOVE\a\b", 10, 0);
                if(get_message( &adil.x, &adil.y, socket, &message)) break;
                if(adil.compare(x, y)){
                    cout << "104 TURN RIGHT\a\b" << endl;
                    send (socket, "104 TURN RIGHT\a\b", 16, 0);
                    if(get_message( &adil.x, &adil.y, socket, &message)) return 1;
                    if (!adil.compare(x, y)) return 1;
                    cout << "102 MOVE\a\b" << endl;
                    send (socket, "102 MOVE\a\b", 10, 0);
                    if(get_message( &adil.x, &adil.y, socket, &message)) return 1;
                }
                adil.define_direction(x, y);
                if(adil.direction == "Error"){ 
                    break;
                }
                while(!(adil.x == 0 && adil.y == 0)){
                    if(adil.y != 0) if(adil.turn_y(&message)) return 1;
                    while(adil.y != 0 && (adil.direction == "UP" || adil.direction == "down")){
                        if(adil.move_y(&message)) return 1;
                    }
                    if((adil.x) != 0) if(adil.turn_x(&message)) return 1;
                    while(adil.x != 0 && (adil.direction == "righT" || adil.direction == "Left")){
                        if(adil.move_x(&message)) return 1;
                    }
                }
                // LAST MESSAGE------------------------------------------------------------------------------------------------------------------
                cout << "105 GET MESSAGE\a\b" << endl;
                send (socket, "105 GET MESSAGE\a\b", 17, 0);
                last_message(socket, &message);
                break;
            }
            close(socket);
            return 0;
        }
    int status = 0;
    waitpid(0, &status, WNOHANG);
    close(socket);
    }
    return 0;
}
