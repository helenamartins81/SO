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


/* cenas dos guioes dos pipes com nome
int main(){
  int fd, n;
  char buf[256];
  if((fd = open("fifo", O_WRONLY)) < 0)
    perror("erro ao abrir o ficheiro");

  while((n = read(0, buf, 256)) > 0)
    write(fd, buf, n);
  return 0;
}
*/
