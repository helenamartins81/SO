//inserir novos artigos ou modificar atributos de artigos.

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


#define SIZE 30
#define SIZE_STRING 50

//alterar nome do artigo
// n <código> <novo nome>

void preenche_espacos (char buf[SIZE_STRING]){
      int i = strlen(buf);
      for(;i<SIZE_STRING-1;i++){
        buf[i]='a';
      }
      buf[i]='\n';
}


//mudar para tamanho do nome variavel
int altera_nome(int code, char* nome){
  int fd = open("strings.txt", O_CREAT|O_WRONLY, 0777);
  if(fd<0){
    exit(-1);
  }
  lseek(fd,SIZE_STRING*code, SEEK_SET);
  char buf[SIZE_STRING];
  sprintf(buf, "nome: %s", nome);
  preenche_espacos(buf);
  write(fd, buf,strlen(buf));

  close(fd);
return 0;

}




//alterar preço do artigo
// p <código> <novo preço>
//NAO ESTA A DAR ???

int altera_preco(int code, float preco){
    int fd = open("artigos.txt", O_RDWR, 0777);
    if(fd < 0){
      exit(-1);
    }
    lseek(fd, SIZE*code , SEEK_SET);
    char buffer[SIZE];
    sprintf(buffer, "artigo: %d, preco: %f\n", code, preco);
    write(fd, buffer, strlen(buffer));

    close(fd);
    return 0;
}





int countLines(){
  int fd = open("artigos.txt", O_RDONLY, 0777);
  if(fd<0){
    exit(-1);
  }
  int count= 0;
  char c;
  while(read(fd, &c, 1)==1){
    if(c=='\n'){
      count++;
    }
  }

  close(fd);
  return count;
}


//inserir artigos
// i <nome> <preço>

void insereArtigo(int nome, float preco){
  char buf[SIZE];
  int fd = open("/artigos.txt", O_CREAT|O_APPEND|O_RDONLY, 0777);
  int s = open("/strings.txt", O_CREAT|O_APPEND|O_RDONLY, 0777);
  if(fd < 0)
    exit(-1);
  if(s < 0){
    exit(-1);
  }
  int count = countLines();
  

  close(fd);
  close(s);

}


/*
int main(){

}
*/
