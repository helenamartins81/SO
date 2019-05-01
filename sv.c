//controlar stocks;receber pedidos do cliente de vendas;registar as vendas efectuadas


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


#define SIZE 50
#define SIZE_STRING 50


//registar vendas efectuadas

void registarVenda(int codigo, int quantidade, float montante){
  char buf[SIZE];
  int fd = open("vendas.txt", O_CREAT|O_RDWR, 0777);
  if(fd < 0){
    exit(-1);
  }
  lseek(fd,SIZE*codigo, SEEK_SET);
  sprintf(buf, "artigo: %d, quantidade: %d, montante: %f", codigo, quantidade, montante);
  write(fd, buf, strlen(buf));

  close(fd);
}



void stocks(int codigo, int quantidade){
  char buf[SIZE];
  int fd = open("stocks.txt", O_CREAT|O_RDWR, 0777);
  if(fd < 0){
    exit(-1);
  }
  lseek(fd,SIZE*codigo, SEEK_SET);
  sprintf(buf, "artigo: %d, quantidade: %d", codigo, quantidade);
  write(fd, buf, strlen(buf));

  close(fd);
}

int main(int argc, char const *argv[]) {

  return 0;
}
