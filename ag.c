#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>


#include "defs.h"




void geraAg(){

}




int main(){
  int in = open(FVENDAS,O_RDONLY,0777);
  if(in < 0){
    perror("esse artigo nao existe\n");
    exit(-1);
  }
  int r = dup2(in, 0);
  if(r == -1){
    perror("não conseguiu copiar\n");
    exit(-1);
  }


	return 0;
}
