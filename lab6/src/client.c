#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

#include "common.h"

struct Server {
    char ip[255];
    int port;
};

struct ThreadArgs {
    struct Server server;
    uint64_t begin;
    uint64_t end;
    uint64_t mod;
    uint64_t result;
};

// uint64_t Factorial(const struct FactorialArgs *args) {
//     uint64_t ans = 1;
//     for (uint64_t i = args->begin; i <= args->end; i++) {
//         ans = MultModulo(ans, i, args->mod);
//     }
//     return ans;
// }

void *ServerThread(void *args) {
    struct ThreadArgs *thread_args = (struct ThreadArgs *)args;
    
    struct hostent *hostname = gethostbyname(thread_args->server.ip);
    if (hostname == NULL) {
        fprintf(stderr, "gethostbyname failed with %s\n", thread_args->server.ip);
        pthread_exit(NULL);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(thread_args->server.port);
    
    if (hostname->h_addr_list[0] == NULL) {
        fprintf(stderr, "No address found for %s\n", thread_args->server.ip);
        pthread_exit(NULL);
    }
    server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr_list[0]);

    int sck = socket(AF_INET, SOCK_STREAM, 0);
    if (sck < 0) {
        fprintf(stderr, "Socket creation failed!\n");
        pthread_exit(NULL);
    }

    if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
        fprintf(stderr, "Connection failed\n");
        close(sck);
        pthread_exit(NULL);
    }

    char task[sizeof(uint64_t) * 3];
    memcpy(task, &thread_args->begin, sizeof(uint64_t));
    memcpy(task + sizeof(uint64_t), &thread_args->end, sizeof(uint64_t));
    memcpy(task + 2 * sizeof(uint64_t), &thread_args->mod, sizeof(uint64_t));

    if (send(sck, task, sizeof(task), 0) < 0) {
        fprintf(stderr, "Send failed\n");
        close(sck);
        pthread_exit(NULL);
    }

    char response[sizeof(uint64_t)];
    if (recv(sck, response, sizeof(response), 0) < 0) {
        fprintf(stderr, "Recieve failed\n");
        close(sck);
        pthread_exit(NULL);
    }

    memcpy(&thread_args->result, response, sizeof(uint64_t));
    close(sck);
    
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    uint64_t k = 0;
    uint64_t mod = 0;
    char servers_file[255] = {'\0'};

    while (true) {
        static struct option options[] = {{"k", required_argument, 0, 0},
                                          {"mod", required_argument, 0, 0},
                                          {"servers", required_argument, 0, 0},
                                          {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "", options, &option_index);

        if (c == -1)
            break;

        switch (c) {
        case 0: {
            switch (option_index) {
            case 0:
                ConvertStringToUI64(optarg, &k);
                break;
            case 1:
                ConvertStringToUI64(optarg, &mod);
                break;
            case 2:
                memcpy(servers_file, optarg, strlen(optarg));
                break;
            default:
                printf("Index %d is out of options\n", option_index);
            }
        } break;

        case '?':
            printf("Arguments error\n");
            break;
        default:
            fprintf(stderr, "getopt returned character code 0%o?\n", c);
        }
    }

    if (k == 0 || mod == 0 || !strlen(servers_file)) {
        fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
                argv[0]);
        return 1;
    }

    FILE *file = fopen(servers_file, "r");
    if (!file) {
        fprintf(stderr, "Cannot open servers file: %s\n", servers_file);
        return 1;
    }

    struct Server *servers = NULL;
    unsigned int servers_num = 0;
    char line[255];
    
    while (fgets(line, sizeof(line), file)) {
        servers = realloc(servers, (servers_num + 1) * sizeof(struct Server));
        char *colon = strchr(line, ':');
        if (colon) {
            *colon = '\0';
            strcpy(servers[servers_num].ip, line);
            servers[servers_num].port = atoi(colon + 1);
            servers_num++;
        }
    }
    fclose(file);

    if (servers_num == 0) {
        fprintf(stderr, "No valid servers found in file\n");
        free(servers);
        return 1;
    }

    pthread_t threads[servers_num];
    struct ThreadArgs thread_args[servers_num];

    uint64_t segment = k / servers_num;
    
    for (unsigned int i = 0; i < servers_num; i++) {
        thread_args[i].server = servers[i];
        thread_args[i].begin = i * segment + 1;
        thread_args[i].end = (i == servers_num - 1) ? k : (i + 1) * segment;
        thread_args[i].mod = mod;
        thread_args[i].result = 1;

        if (pthread_create(&threads[i], NULL, ServerThread, (void *)&thread_args[i])) {
            fprintf(stderr, "Error creating thread for server %s:%d\n", 
                    servers[i].ip, servers[i].port);
        }
    }

    uint64_t total = 1;
    for (unsigned int i = 0; i < servers_num; i++) {
        pthread_join(threads[i], NULL);
        total = MultModulo(total, thread_args[i].result, mod);
    }

    printf("Final answer: %" PRIu64 "\n", total);
    free(servers);

    return 0;
}