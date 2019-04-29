//inserir novos artigos ou modificar atributos de artigos.

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


#define BUFFER_SIZE 1024

//alterar nome do artigo
// n <código> <novo nome>


void altera_string(int c, char nome[10]);

void altera_nome(int code, char nome[10]){

  int fd = open("/artigos.txt", O_RDWR, 0600);
  if(fd < 0)
    exit(-1);
  int i = 0;
  char linha[BUFFER_SIZE];
  while(read(fd, linha, sizeof linha) >0){
    if(i == code){
      int c = linha[2];
      altera_string(c, nome);
    }
    else i++;
  }
  close(fd);

}


void altera_string(int c, char nome[10]){
  int fd = open("/artigos.txt", O_RDWR, 0600);
  if(fd < 0)
    exit(-1);
  int i = 0;
  char linha[BUFFER_SIZE];
  while(read(fd, linha, sizeof linha) >0){
    if(i == c){
      lseek(fd, -sizeof(linha[2]),SEEK_END);
      write(fd, nome, strlen(nome));
    }
    else i++;
  }
  close(fd);
}




//alterar preço do artigo
// p <código> <novo preço>
//NAO FAÇO IDEIA SE ISTO FAZ ALGUMA COISA DECENTE
void alteraPreco(int code, float preco){
    char buf[10];
    gcvt(preco, 6, buf);
    int fd = open("/artigos.txt", O_RDWR, 0600);
    if(fd < 0)
      exit(-1);
    int i = 0;
    char linha[BUFFER_SIZE];
    while(read(fd, linha, sizeof linha) >0){
      if(i == code){
        lseek(fd, -sizeof(float),SEEK_END);
        write(fd, buf, sizeof(float));
      }
      else i++;
    }
    close(fd);
}



//inserir artigos
// i <nome> <preço>

//conta as linhas do ficheiro
int codigoNumerico(char *filename){
  FILE *fp = fopen(filename,"r");
  int ch=0;
  int lines=0;

  if (fp == NULL);
  return 0;

  lines++;
  while(!feof(fp)){
    ch = fgetc(fp);
    if(ch == '\n'){
      lines++;
    }
  }
  fclose(fp);
  return lines;
}


void insereArtigo(int nome, float preco){
  char buf[BUFFER_SIZE];
  int fd = open("/artigos.txt", O_CREAT|O_APPEND|O_RDONLY, 0777);
  int s = open("/strings.txt", O_CREAT|O_APPEND|O_RDONLY, 0777);
  if(fd < 0)
    exit(-1);
  if(s < 0){
    exit(-1);
  }
  int i = codigoNumerico("artigos.txt"); //dá o id do artigo
  sprintf(buf, "%d %d %f", i, nome, preco);
  write(fd, buf, 40);
  //write(s, nome, 1024*1024); -> escrever no ficheiro strings
  close(fd);
  close(s);


}
