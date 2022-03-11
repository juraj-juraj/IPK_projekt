/**
 * @brief Implementation of first assignment from subject IPK. 
 * @author Juraj Novosád(xnovos13)
 * @file hinfosvc.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include "str_obj.h"

#define DEBUG 0
//I dont know why, it couldnt find it in correct header files. So defined it here
#define SO_REUSEPORT 15

// Defined buffer length for retrieving data from popen
#define BUFF_LEN 4096
#define SA struct sockaddr

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define YIELD_ERROR perror("ERROR");exit(EXIT_FAILURE);

#define MAX_CLIENTS 3
#define TIMEOUT_CONNECT 200
#define HOST_PARAM "/hostname "
#define CPU_NAME_PARAM "/cpu-name "
#define LOAD_PARAM "/load "
#define HTTP_HEAD "HTTP/1.0 200 OK \r\nContent-Type: text/plain\r\n\r\n"
#define BAD_REQUEST "HTTP/1.0 400 Bad Request \r\nContent-Type: text/plain;\r\n\r\nBad request sent"
#define INTERNAL_ERROR "HTTP/1.0 200 OK \r\nContent-Type: text/plain;\r\n\r\nError on server side"

#define HOSTNAME_COMMAND "/etc/hostname"
// retrieve command from cpuinfo file. Line with name and second collumn
#define CPU_NAME_COMMAND "cat /proc/cpuinfo | grep \"model name\" -m 1 | awk -F \":\" \'{print $2}\' | tr \"\n\" -d"
// command to get idle and non idle values in one second difference for computing cpu utilization
#define CPU_LOAD_COMMAND "cat /proc/stat | grep \"cpu \" | awk -F \" \" '{print $5 + $6; print $2 + $3 + $4 + $7 + $8 + $9}' ;\
sleep 1 ;cat /proc/stat | grep \"cpu \" | awk -F \" \" '{print $5 + $6; print $2 + $3 + $4 + $7 + $8 + $9}'"

#define MAX(a, b) (a>b)? a : b

#define STR_PTR(base) &(base.string[base.cursor])
#define STR_SPACE(base) base.length - base.cursor
// Length of string 'GET' which starts the request
#define STR_GET_LEN 3
#define BASE_10 10

typedef enum{
    None,
    Hostname,
    CpuInfo,
    CpuLoad,
    Error
} recvstate_t;

//fsm states for parsing GET word from packet
typedef enum{
    G,
    GE,
    GET,
} dec_state_t;

/**
 * Gets the name of hosting computer. 
 * @return String containing hostname. It needs to be freed after use
 */
char *get_hostname(){
    FILE *host_file = fopen(HOSTNAME_COMMAND, "r");
    
    if(host_file == NULL){
        return NULL;
    }
    //calculate exactly how much space is needed to allocate
    fseek(host_file, 0L, SEEK_END);
    int length = ftell(host_file);
    fseek(host_file, 0L, SEEK_SET);
    char *hostname = (char *) malloc(length);
    if(!hostname){
        fclose(host_file);
        return NULL;
    }
    
    fgets(hostname, length, host_file);
    fclose(host_file);
    return hostname;
}

/**
 * Gets CPU name of hosting computer
 * @return String containing name of computer. It needs to be freed after use
 */
char *get_cpu_name(){
    FILE *cpu_file = popen(CPU_NAME_COMMAND, "r");
    char temp;
    if(!cpu_file){
        return NULL;
    }
    int length = BUFF_LEN;
    char *cpu_name = (char *) malloc(length);
    if(!cpu_name){
        return NULL;
    }
    //skipping leading spaces
    while((temp = fgetc(cpu_file)) == ' '){
        ;
    }
    ungetc(temp, cpu_file);
    fgets(cpu_name, length, cpu_file);
    pclose(cpu_file);
    return cpu_name;
}

/**
 * Gets and calculates cpu utilization. Based on: https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
 * @return String with number reresenting utilization of CPU in %. It needs to be freed after use
 */
char *get_cpu_load(){
    unsigned long int prev_idle, prev_non_idle, idle, non_idle;
    unsigned long int prev_total, total, total_d, work_d, idle_d;
    double percentage;
    FILE *cpu_file = popen(CPU_LOAD_COMMAND, "r");
    if(!cpu_file){
        return NULL;
    }
    int length = BUFF_LEN;
    char *cpu_load = (char *) malloc(length);
    if(!cpu_load){
        return NULL;
    }
    fgets(cpu_load, length, cpu_file);
    prev_idle = strtoul(cpu_load, NULL, BASE_10);
    fgets(cpu_load, length, cpu_file);
    prev_non_idle = strtoul(cpu_load, NULL, BASE_10);
    fgets(cpu_load, length, cpu_file);
    idle = strtoul(cpu_load, NULL, BASE_10);
    fgets(cpu_load, length, cpu_file);
    non_idle = strtoul(cpu_load, NULL, BASE_10);
    
    prev_total = prev_idle + prev_non_idle;
    total = idle + non_idle;
    
    total_d = total - prev_total;
    work_d = non_idle - prev_non_idle;
    percentage = (float) work_d / total_d * 100;
    snprintf(cpu_load, length, "%f%%", percentage);
    pclose(cpu_file);
    return cpu_load;
}

/**
 * FSM to find begining of GET part of request from client. 
 * @param string Message from client. Cursor in string structure will be set to beggining of GET
 * @return 1 if GET was resolved, 0 if not
 */
int find_index(string_t *string){
    dec_state_t fsm_state = G;
    int start_index = -1;
    int done = 0;
    for(int i = 0; i < string->length-STR_GET_LEN; i++){
        switch (fsm_state){
        case G:
            if(string->string[i] == 'G'){
                fsm_state = GE;
                start_index = i;
            }
            break;
        case GE:
            if(string->string[i] == 'E'){
                fsm_state = GET;
            }
            else{
                fsm_state = G;
                start_index = -1;
            }
            break;
        case GET:
            if(string->string[i] == 'T'){
                done = 1;
            }   
            else{
                fsm_state = G;
                start_index = -1;
            }
            break;
        }
        if(done == 1){
            break;
        }
    }
    string->cursor = (start_index == -1)? string->cursor : start_index;
    return (start_index == -1) ? EXIT_FAILURE : EXIT_SUCCESS;
}

/**
 * Decodes what is requested after GET
 * @param string Request from client
 * @return Recvstate, type of received request coded as enumeration 
 */
recvstate_t decode_request(string_t *string){
    recvstate_t recvstate = None;
    find_index(string);
    //move to word after GET
    str_nword(string);
    if(str_start_with(string, HOST_PARAM, strlen(HOST_PARAM))){
        recvstate = Hostname;
    }
    else if(str_start_with(string, CPU_NAME_PARAM, strlen(CPU_NAME_PARAM))){
        recvstate = CpuInfo;
    }   
    else if(str_start_with(string, LOAD_PARAM, strlen(LOAD_PARAM))){
        recvstate = CpuLoad;
    }
    else {
        recvstate = Error;
    }
    return recvstate;
}

/**
 * Responds to sent request
 * @param socket to which socket send the data
 * @param type what client requested
 */
void respond(int socket, recvstate_t r_type){
    int retval = 0;
    char *temp = NULL;
    string_t message;
    str_init(&message);
    str_append(&message, HTTP_HEAD, strlen(HTTP_HEAD));  

    switch(r_type){
        case Hostname:
            temp = get_hostname();
            break;
        case CpuInfo:
            temp = get_cpu_name();
            break;
        case CpuLoad:
            temp = get_cpu_load();
            break;
        default:
            write(socket, BAD_REQUEST, strlen(BAD_REQUEST));
            return;
    }
    if(temp == NULL){
        write(socket, INTERNAL_ERROR, strlen(INTERNAL_ERROR));
        return;
    }
    if(DEBUG) printf("premenna v temp: %s \n", temp);
    str_append(&message, temp, strlen(temp));
    write(socket, (char *)message.string, message.cursor);
    free(temp);
    str_destruct(&message);
    return retval;
}

int get_port(int count, char *str_number){
    int port_number = 0;
    if(count != 2){
        if(count < 2) errno = 6;
        else errno = 7;
    }
    else{
        port_number = strtoul(str_number, NULL, BASE_10);
    }
    return port_number;
}

string_t recvstring;
int listenfd;
struct pollfd *pfds;

void process_end(int signal){
    str_destruct(&recvstring);
    close(listenfd);
    free(pfds);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){
    int connfd, n;
    struct sockaddr_in servaddr;
    recvstate_t recv_state = None;

    str_init(&recvstring);

    signal(SIGINT, process_end);

    int server_port = get_port(argc, argv[1]);
    if(!server_port){
        YIELD_ERROR;
    }

    //Set up blank socket
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        YIELD_ERROR;
    }
    int opt = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        YIELD_ERROR;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //listening at any address
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //listening only on selected port
    servaddr.sin_port = htons(server_port);

    if((bind(listenfd, (SA *)&servaddr, sizeof(servaddr))) < 0){
        str_destruct(&recvstring);
        YIELD_ERROR;
    }
    if((listen(listenfd, MAX_CLIENTS)) < 0){
        YIELD_ERROR;
    }
    
    int nfds = 1; 
    pfds = calloc(nfds, sizeof(struct pollfd));
    if(pfds == NULL){
        YIELD_ERROR;
    }

    while(1){;
        if(DEBUG) printf("waiting for connection on port %d \n", server_port);
        connfd = accept(listenfd, (SA *) NULL, NULL);

        pfds[0].fd = connfd;
        pfds[0].events = POLLIN;
        if(!poll(pfds, nfds, TIMEOUT_CONNECT)){
            continue;
        }

        str_clear(&recvstring);
        while((n = read(connfd, STR_PTR(recvstring) , STR_SPACE(recvstring))) > 0){
            if(DEBUG) fprintf(stdout, "-------------\n%s\n", recvstring.string);
            recvstring.cursor += n;
            if(recvstring.cursor + 1 >= recvstring.length){
                str_realloc(&recvstring);
                continue;
            }
            if(DEBUG) printf("last_char: %d\n", (int) recvstring.string[recvstring.cursor-1]);
            if(recvstring.string[recvstring.cursor-1] == '\n'){
                break;
            }
        }
        if(n < 0){
            YIELD_ERROR;
        }
        recv_state = decode_request(&recvstring);
        respond(connfd, recv_state);
        close(connfd);
    }
    return EXIT_SUCCESS;
}


