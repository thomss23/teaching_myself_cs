#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cjson/cJSON.h>


#define LISTEN_PORT 8080
#define MAX_PAIRS 100

// test with curl --http0.9 127.0.0.1:8080
typedef struct Pair {
    char headerName[50];
    char value[50];
} Pair;

int main() {
    struct sockaddr_in addr;
    int server_fd, client_fd;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(LISTEN_PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("bind");
            close(server_fd);
            exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Shout server is listening on %d\n", LISTEN_PORT);
    
    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        char buffer[256];
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            
            char *save;
            char *line = strtok_r(buffer, "\r\n", &save);
            Pair *pair_list = malloc(MAX_PAIRS * sizeof(Pair));
            int entries = 0;


            while (line != NULL) {
                printf("line: %s\n", line);

                char * colon = strchr(line, ':');
                if (colon == NULL) {
                    line = strtok_r(NULL, "\r\n", &save);
                    continue;
                }

                size_t key_len = (size_t)(colon - line);
                char key[sizeof(pair_list[0].headerName)];

                if (key_len >= sizeof(key)) {
                    key_len = sizeof(key) - 1;
                }
                memcpy(key, line, key_len);
                key[key_len] = '\0';

                char *val_ptr = colon + 1;
                while (*val_ptr == ' ') { 
                    val_ptr++; // Skip optional HTTP whitespace after colon
                }
                        
                
                if (entries < MAX_PAIRS) {
                    strncpy(pair_list[entries].headerName, key,
                            sizeof(pair_list[entries].headerName) - 1);
                    pair_list[entries].headerName[sizeof(pair_list[entries].headerName) - 1] = '\0';

                    strncpy(pair_list[entries].value, val_ptr,
                            sizeof(pair_list[entries].value) - 1);
                    pair_list[entries].value[sizeof(pair_list[entries].value) - 1] = '\0';

                    entries++;
                }
                
                line = strtok_r(NULL, "\r\n", &save);
            }
            free(pair_list);

            cJSON *json = cJSON_CreateObject();

            for(int i = 0; i < entries; i++) {
                cJSON_AddStringToObject(json, pair_list[i].headerName, pair_list[i].value);
            }
            
            char *json_str = cJSON_Print(json);

            write(client_fd, json_str, strlen(json_str));
            cJSON_free(json_str);
            cJSON_Delete(json);
        }

        close(client_fd);
    }
}
