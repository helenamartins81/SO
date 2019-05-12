
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "defs.h"

int readlinha( char *buf, size_t nbyte,int fildes){
    int i=0;

    while(i <nbyte-1 &&
         read(fildes, buf+i,1)>0 &&
         buf[i] != '\n'){
         i++;
    }
    if(i >= nbyte)
           buf[i] = 0;
    else
           buf[i+1] = 0;

    return i;
}

int fifo_entrada = 0, fifo_saida = 0;
char * myfifo = "fifo-mensagem";
char *myfifo2 = "fifo2";

int main(int argc, char *argv[]) {
  pid_t pid;
  char cmd[200], buffer[1000], cliente[200];
  int len, *input_len = (int *)&buffer[0];
  char input_str[1024]; //&buffer[sizeof(int)];


  int ver = mkfifo(myfifo,0666);


  pid = getpid();
  sprintf(cliente, "%d", pid);

  while (readlinha(cmd, sizeof(cmd), 0) > 0) {
    if (strlen(cmd) > 0) {
      if ((fifo_entrada = open(myfifo, O_WRONLY, 0666)) < 0){
        perror("Erro ao abrir o fifo de entrada!\n");
        exit(-1);
      }

      sprintf(input_str, "%s %s\n", cliente, cmd);

      *input_len = strlen(input_str) + 1;

      if((write(fifo_entrada, input_str, *input_len)) < 0){
        perror("Erro ao abrir o fifo de entrada!\n");
        exit(-1);
      }
      close(fifo_entrada);


      if ((fifo_saida = open(myfifo2, O_RDONLY,0666)) < 0){
        perror("Erro ao abrir o fifo de saida!\n");
        exit(-1);
      }

      char resposta[200];
      if((len = read(fifo_saida, &resposta, sizeof(resposta))) < 0) {
        perror("Erro ao abrir o fifo de saida!\n");
        exit(-1);
      }
      else{
        write(1, &resposta, len);
      }

    }


  }
  return 0;
}
