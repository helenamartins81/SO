//inserir novos artigos ou modificar atributos de artigos.

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>


#define FVENDAS "vendas.txt"
#define FSTOCKS "stocks.txt"
#define FARTIGOS "artigos.txt"


typedef unsigned long Filepos;
typedef unsigned long ArtIndex;

typedef struct {
  Filepos nome;
  double preco;
} Artigo;

typedef struct {
  ArtIndex codigo;
  double quantidade;
  double total;
  time_t tempo;
} Venda;

typedef struct{
  Filepos codigo;
  double quantidade;
} Stock;

double preco_total(ArtIndex artigo, double quantidade) {
  Artigo registo;
  int fd = open(FARTIGOS, O_CREAT | O_RDWR, 0660);
  lseek(fd, artigo * sizeof(Artigo), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  double total = registo.preco * quantidade;
  return total;
}

int modifica_stock(ArtIndex artigo, double quantidade) {
  Stock registo;
  int fd = open(FSTOCKS, O_CREAT | O_RDWR, 0660);
  lseek(fd, artigo * sizeof(Stock), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  if (registo.quantidade-quantidade < 0){
    printf("não tem stock disponivel\n" );
    return 0;}
  else registo.quantidade = registo.quantidade-quantidade;
  lseek(fd, artigo * sizeof(Stock), SEEK_SET);
  write(fd, &registo, sizeof(registo));
  close(fd);
  return 1;
}

ArtIndex inserir_venda(ArtIndex codigo, double quantidade){
  Venda novo;
  novo.codigo = codigo;
  novo.quantidade = quantidade;
  novo.total = preco_total(codigo,quantidade);
  novo.tempo = time(&novo.tempo);
  int fd = open(FVENDAS, O_CREAT | O_RDWR, 0660);
  Filepos pos = lseek(fd, 0, SEEK_END);
  if (modifica_stock(codigo,quantidade) == 1){
  write(fd, &novo, sizeof(novo));
  }
  else {
    printf("Venda invalida por falta de stock\n" );
  }
  return pos / sizeof(Venda);
}


void imprimir_venda(ArtIndex venda) {
  Venda registo;
  int fd = open(FVENDAS, O_CREAT | O_RDONLY, 0660);
  Filepos pos = lseek(fd, venda * sizeof(Venda), SEEK_SET);
  if (read(fd, &registo, sizeof(registo)) < sizeof(registo))
    printf("esse artigo nao existe\n");
  else {
    printf("codigo %ld\nquantidade %lf\npreço total %lf\ntempo:%s\n", registo.codigo, registo.quantidade, registo.total, ctime(&registo.tempo));
  }
}

void imprimir_stock(ArtIndex stock) {
  Stock registo;
  int fd = open(FSTOCKS, O_CREAT | O_RDONLY, 0660);
  Filepos pos = lseek(fd, stock * sizeof(Stock), SEEK_SET);
  if (read(fd, &registo, sizeof(registo)) < sizeof(registo))
    printf("esse artigo nao existe\n");
  else {
    printf("codigo %ld\nquantidade %lf\n", registo.codigo, registo.quantidade);
  }
}

void interpretar_linha(char *cmd) {
  switch (cmd[0]) {
    case 'v':
    {
      ArtIndex codigo;
      double quantidade;
      double total;
      int params = sscanf(cmd, "v %lu %lf %lf", &codigo, &quantidade, &total);
      Filepos venda = inserir_venda(codigo, quantidade);
      printf("Venda %ld\n", venda);
      break;
    }
    case 's':
    {
      Filepos stock;
      int params = sscanf(cmd, "l %lu", &stock);
      imprimir_stock(stock);
      printf("stock %ld\n", stock);
      break;
    }
    case 'l':
    {
      Filepos venda;
      int params = sscanf(cmd, "l %lu", &venda);
      imprimir_venda(venda);
      printf("venda %ld\n", venda);
      break;
    }
    default:
      printf("operacao invalida: %s\n", cmd);

  }
}


int main() {
  char cmd[200];
  while (fgets(cmd, sizeof(cmd), stdin) > 0) {
    if (strlen(cmd) > 0)
      interpretar_linha(cmd);
  }
}
