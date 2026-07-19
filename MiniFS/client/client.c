#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "commands.h"

int main(int argc, char** argv) {
    const char* addr="127.0.0.1";
    int port=8080;
    for(int i=1;i<argc;i++) {
        if(strcmp(argv[i],"-a")==0&&i+1<argc) addr=argv[++i];
        else if(strcmp(argv[i],"-p")==0&&i+1<argc) port=atoi(argv[++i]);
    }
    client_context_t client;
    client_init(&client,addr,port);
    if(client_connect(&client)!=0) {
        fprintf(stderr,"Failed to connect\n");
        return 1;
    }
    printf("MiniFS Client - Connected to %s:%d\n",addr,port);
    printf("Commands: UPLOAD <local> <remote>, DOWNLOAD <remote> <local>, DELETE <file>, LIST, INFO <file>, EXIT\n");
    char* line;
    while((line=readline("MiniFS> "))!=NULL) {
        if(strlen(line)==0) { free(line); continue; }
        add_history(line);
        char cmd[32], arg1[256], arg2[256];
        sscanf(line,"%31s %255s %255s",cmd,arg1,arg2);
        for(int i=0;cmd[i];i++) cmd[i]=toupper(cmd[i]);
        if(strcmp(cmd,"UPLOAD")==0) cmd_upload(&client,arg1,arg2);
        else if(strcmp(cmd,"DOWNLOAD")==0) cmd_download(&client,arg1,arg2);
        else if(strcmp(cmd,"DELETE")==0) cmd_delete(&client,arg1);
        else if(strcmp(cmd,"LIST")==0) cmd_list(&client);
        else if(strcmp(cmd,"INFO")==0) cmd_info(&client,arg1);
        else if(strcmp(cmd,"EXIT")==0) { cmd_exit(&client); free(line); break; }
        else printf("Unknown command\n");
        free(line);
    }
    return 0;
}
