//inserir novos artigos ou modificar atributos de artigos.

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

//alterar nome do artigo
// n <código> <novo nome>


//alterar preço do artigo
// p <código> <novo preço>



//inserir artigos
// i <nome> <preço>

//conta as linhas do ficheiro
int codigoNumerico(char *filename)
{
  FILE *fp = fopen(filename,"r");
  int ch=0;
  int lines=0;

  if (fp == NULL);
  return 0;

  lines++;
  while(!feof(fp))
{
  ch = fgetc(fp);
  if(ch == '\n')
  {
    lines++;
  }
}
  fclose(fp);
  return lines;
}


void insereArtigo(int nome, float preco){

  int fd = open("/artigos.txt", O_CREAT|O_APPEND|O_RDONLY, 0777);
  int s = open("/strings.txt", O_CREAT|O_APPEND|O_RDONLY, 0777);
  if(fd < 0)
    exit(-1);
  if(s < 0){
    exit(-1);
  }
  int i = codigoNumerico("artigos.txt"); //dá o id do artigo
  //write(fd, i, nome, preco, 1024);
  //write(s, nome, 1024*1024);
  close(fd);
  close(s);


}
