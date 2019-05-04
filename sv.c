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

int preco_total(ArtIndex artigo, double quantidade) {
  Artigo registo;
  int fd = open(FARTIGOS, O_CREAT | O_RDWR, 0660);
  lseek(fd, artigo * sizeof(Artigo), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  double total = registo.preco * quantidade;
  return total;
}

ArtIndex inserir_venda(ArtIndex codigo, double quantidade){
  Venda novo;
  novo.codigo = codigo;
  novo.quantidade = quantidade;
  novo.total = preco_total(codigo,quantidade);
  novo.tempo = time(&novo.tempo);
  int fd = open(FVENDAS, O_CREAT | O_RDWR, 0660);
  Filepos pos = lseek(fd, 0, SEEK_END);
  write(fd, &novo, sizeof(novo));
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
/*
void modificar_nome(ArtIndex artigo, char *novonome) {
  Artigo registo;
  int fd = open(FARTIGOS, O_CREAT | O_RDWR, 0660);
  lseek(fd, artigo * sizeof(Artigo), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  registo.nome = inserir_nome(novonome);
  lseek(fd, artigo * sizeof(Artigo), SEEK_SET);
  write(fd, &registo, sizeof(registo));
}*/



/*
void imprimir_artigo(ArtIndex artigo) {
  Artigo registo;
  int fd = open(FARTIGOS, O_CREAT | O_RDONLY, 0660);
  Filepos pos = lseek(fd, artigo * sizeof(Artigo), SEEK_SET);
  if (read(fd, &registo, sizeof(registo)) < sizeof(registo))
    printf("esse artigo nao existe\n");
  else {
    char * nome_ptr = ler_nome(registo.nome);
    printf("%ld:%s:%lf", artigo, nome_ptr, registo.preco);
  }
}*/


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

    /*case 'n':
    {
      Filepos codigo;
      char nome[100];
      int params = sscanf(cmd, "n %lu %s", &codigo, nome);
      modificar_nome(codigo, nome);
      break;
    }

    case 'p':
    {
      Filepos codigo;
      double preco;
      int params = sscanf(cmd, "p %lu %lf", &codigo, &preco);
      modificar_preco(codigo, preco);
      printf("artigo %ld\n", codigo);
      break;
    }*/
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
