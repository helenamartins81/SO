#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "defs.h"


int fifo_entrada = 0, fifo_saida = 0;

void abrir_fifo() {
  if ((fifo_entrada = open("fifo-entrada", O_WRONLY|O_CREAT, 0666)) < 0){
    perror("Erro ao abrir o fifo de entrada!\n");
  }
}

int main(int argc, char *argv[]) {
  char cmd[200], buffer[1000], cliente[200];
  int len, *input_len = (int *)&buffer[0];
  char *input_str = &buffer[sizeof(int)];

  sprintf(cliente, "fifo-%d", rand());
  mkfifo(cliente, 0660);
  abrir_fifo();

  while (fgets(cmd, sizeof(cmd), stdin) > 0) {
    if (strlen(cmd) > 0) {
      sprintf(input_str, "%s %s\n", cliente, cmd);
      *input_len = sizeof(int) + strlen(input_str) + 1;
      printf("ENVIAR (%ls, %s) %d: %s", input_len, input_str, *input_len, input_str);
      write(fifo_entrada, buffer, *input_len);

      if ((fifo_saida = open(cliente, O_RDONLY)) < 0)
        perror("Erro ao abrir o fifo de saida!\n");
      len = read(fifo_saida, buffer, sizeof(buffer));
      buffer[len] = 0;
      close(fifo_saida);
      printf("res (%d) = %s\n", len, buffer);
    }
  }
}
