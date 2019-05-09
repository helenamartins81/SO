#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "defs.h"


int fifo_entrada = 0, fifo_saida = 0;
char * myfifo = "fifo-mensagem";

int main(int argc, char *argv[]) {
  pid_t pid;
  char cmd[200], buffer[1000], cliente[200];
  int len, *input_len = (int *)&buffer[0];
  char *input_str[1024]; //&buffer[sizeof(int)];


  int ver = mkfifo(myfifo,0600);

  /*
  if ((fifo_entrada = open("fifo-entrada", O_WRONLY|O_CREAT, 0666)) < 0){
    perror("Erro ao abrir o fifo de entrada!\n");
  }
  */
  pid = getpid();
  printf("pid do cliente:%d\n",pid);
  sprintf(cliente, "%d", pid); // Esta parte aqui acho q é para quando houver filhos
  //mkfifo(cliente, 0660);

  while (fgets(cmd, sizeof(cmd), stdin) > 0) {
    if (strlen(cmd) > 0) {

      if ((fifo_entrada = open(myfifo, O_WRONLY, 0666)) < 0){
        perror("Erro ao abrir o fifo de entrada!\n");
      }

      sprintf(input_str, "%s %s\n", cliente, cmd);
      printf("tas a mandar isto %s\n",input_str);

      *input_len = strlen(input_str) + 1;
      //printf("%s %d",input_str, *input_len);

      printf("ENVIAR (%ls, %s) %d: %s", input_len, input_str, *input_len, input_str);
      write(fifo_entrada, input_str, *input_len);
      close(fifo_entrada);
      close(myfifo);

      /*
      if ((fifo_saida = open(cliente, O_RDONLY)) < 0)
        perror("Erro ao abrir o fifo de saida!\n");
      len = read(fifo_saida, buffer, sizeof(buffer));
      buffer[len] = 0; */

      //close("fifo_saida");
      printf("res (%d) = %s\n", len, buffer);

    }
  }
}
