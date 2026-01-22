#ifdef _WIN32
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <string.h>
    #include <stdlib.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif
#include <stdio.h>
#include "clinic.h" 

#define PORT "8080"
#define BUFF_SIZE 4096

// APIs to sync with new function names
// ...
// Same server logic, just calling new function names from clinic.h

void send_response(SOCKET client, const char* status, const char* type, const char* body) {
    char header[BUFF_SIZE];
    sprintf(header, 
        "HTTP/1.1 %s\r\n"
        "Content-Type: %s\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Content-Length: %d\r\n"
        "\r\n", status, type, strlen(body));
    send(client, header, strlen(header), 0);
    send(client, body, strlen(body), 0);
}

void parse_body(char* body, char* key, char* value) {
    char* found = strstr(body, key);
    if (found) {
        found += strlen(key) + 1; 
        int i = 0;
        while (found[i] && found[i] != '&') {
            value[i] = found[i];
            i++;
        }
        value[i] = '\0';
    }
}

void handle_request(SOCKET client) {
    char buffer[BUFF_SIZE];
    int bytes = recv(client, buffer, BUFF_SIZE - 1, 0);
    if (bytes <= 0) return;
    buffer[bytes] = '\0';

    char method[10], path[100];
    sscanf(buffer, "%s %s", method, path);
    printf("Req: %s %s\n", method, path);

    char* body = strstr(buffer, "\r\n\r\n");
    if (body) body += 4; else body = "";

    if (strcmp(method, "GET") == 0) {
        if (strcmp(path, "/doctors") == 0) {
             // Manual JSON for doc list
            char buf[4096];
            strcpy(buf, "[");
            for(int i=0; i<doctor_count; i++) {
                char doc_json[256];
                sprintf(doc_json, "{\"name\": \"%s\"}", doctors[i].name);
                strcat(buf, doc_json);
                if(i < doctor_count - 1) strcat(buf, ",");
            }
            strcat(buf, "]");
            send_response(client, "200 OK", "application/json", buf);
        } else if (strstr(path, "/schedule")) {
             char* q = strchr(path, '=');
             if (q) {
                 Doctor* doc = doctor_find(q + 1);
                 if (doc) {
                     char* json = json_get_week(doc);
                     send_response(client, "200 OK", "application/json", json);
                 } else {
                     send_response(client, "404 Not Found", "text/plain", "Doctor not found");
                 }
             }
        } else if (strcmp(path, "/patients") == 0) {
            send_response(client, "200 OK", "application/json", "[\"Alice\", \"Bob\", \"Charlie\", \"David\", \"Eve\"]"); 
        } else {
            send_response(client, "404 Not Found", "text/plain", "Not Found");
        }
    } 
    else if (strcmp(method, "POST") == 0) {
        if (strcmp(path, "/register") == 0) {
            char name[50]={0}, pass[50]={0};
            parse_body(body, "name", name);
            parse_body(body, "passkey", pass);
            doctor_register(name, pass);
            send_response(client, "200 OK", "text/plain", "Registered");
        } 
        else if (strcmp(path, "/login") == 0) {
            char name[50]={0}, pass[50]={0};
            parse_body(body, "name", name);
            parse_body(body, "passkey", pass);
            if (doctor_login(name, pass)) {
                send_response(client, "200 OK", "application/json", "{\"status\":\"ok\"}");
            } else {
                send_response(client, "401 Unauthorized", "application/json", "{\"status\":\"fail\"}");
            }
        } 
        else if (strcmp(path, "/add_event") == 0) {
            char doc_name[50]={0}, p_name[50]={0}, s_day[5]={0}, s_start[10]={0}, s_dur[10]={0}, s_type[5]={0};
            parse_body(body, "doctor", doc_name);
            parse_body(body, "name", p_name);
            parse_body(body, "day", s_day);
            parse_body(body, "start", s_start);
            parse_body(body, "duration", s_dur);
            parse_body(body, "type", s_type);
            
            Doctor* doc = doctor_find(doc_name);
            if (doc) {
                int res = schedule_add(doc, atoi(s_day), p_name, atoi(s_start), atoi(s_dur), (EventType)atoi(s_type));
                if (res == -2) {
                     // Suggestion
                     int alt = avl_suggest_time(doc->week[atoi(s_day)].root, atoi(s_dur), atoi(s_start));
                     char msg[100];
                     sprintf(msg, "{\"error\": \"Collision\", \"suggestion\": %d}", alt);
                     send_response(client, "409 Conflict", "application/json", msg);
                } else if (res == -1) {
                    send_response(client, "400 Bad Request", "application/json", "{\"error\": \"Limit Reached\"}");
                } else {
                    send_response(client, "200 OK", "application/json", "{\"status\": \"added\"}");
                }
            }
        }
        else if (strcmp(path, "/extend") == 0) {
            char doc_name[50]={0}, s_day[5]={0}, s_id[10]={0}, s_extra[10]={0};
            parse_body(body, "doctor", doc_name);
            parse_body(body, "day", s_day);
            parse_body(body, "id", s_id);
            parse_body(body, "extra", s_extra);
            
            Doctor* doc = doctor_find(doc_name);
            if (doc) {
                schedule_extend(doc, atoi(s_day), atoi(s_id), atoi(s_extra));
                send_response(client, "200 OK", "application/json", "{\"status\": \"extended\"}");
            }
        }
    }
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKELONG(2, 2), &wsaData);
#endif

    SOCKET server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Server running on port 8080... (Access http://localhost:8080/)\n");
    
    system_init();
    doctor_register("Dr.Smith", "1234");
    doctor_register("Dr.Jones", "5678");

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        handle_request(client_fd);
        closesocket(client_fd);
    }

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
