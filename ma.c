//inserir novos artigos ou modificar atributos de artigos.

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FSTRINGS "strings.txt"
#define FARTIGOS "artigos.txt"

typedef unsigned long Filepos;
typedef unsigned long ArtIndex;

typedef struct {
  Filepos nome;
  double preco;
} Artigo;

Filepos inserir_nome(char * nome) {
  int fd = open(FSTRINGS, O_CREAT | O_WRONLY, 0660);
  Filepos pos = lseek(fd, 0, SEEK_END);
  int tamanho = strlen(nome);
  write(fd, &tamanho, sizeof(tamanho));
  write(fd, nome, tamanho);
  close(fd);
  return pos;
}

char * ler_nome(Filepos nome) {
  int fd = open(FSTRINGS, O_CREAT | O_RDONLY, 0660);
  Filepos pos = lseek(fd, nome, SEEK_SET);
  int tamanho;
  read(fd, &tamanho, sizeof(tamanho));
  char * nome_lido = calloc(tamanho + 1, 1);
  read(fd, nome_lido, tamanho);
  return nome_lido;
}

ArtIndex inserir_artigo(char *nome, double preco) {
  Artigo novo;
  novo.nome = inserir_nome(nome);
  novo.preco = preco;
  int fd = open(FARTIGOS, O_CREAT | O_RDWR, 0660);
  Filepos pos = lseek(fd, 0, SEEK_END);
  write(fd, &novo, sizeof(novo));
  return pos / sizeof(Artigo);
}


void modificar_nome(ArtIndex artigo, char *novonome) {
  Artigo registo;
  int fd = open(FARTIGOS, O_CREAT | O_RDWR, 0660);
  lseek(fd, artigo * sizeof(Artigo), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  registo.nome = inserir_nome(novonome);
  lseek(fd, artigo * sizeof(Artigo), SEEK_SET);
  write(fd, &registo, sizeof(registo));
}

void modificar_preco(ArtIndex artigo, double novopreco) {
  Artigo registo;
  int fd = open(FARTIGOS, O_CREAT | O_RDWR, 0660);
  lseek(fd, artigo * sizeof(Artigo), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  registo.preco = novopreco;
  lseek(fd, artigo * sizeof(Artigo), SEEK_SET);
  write(fd, &registo, sizeof(registo));
}

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
}


void interpretar_linha(char *cmd) {
  switch (cmd[0]) {
    case 'i':
    {
      char nome[100];
      double preco;
      int params = sscanf(cmd, "i %s %lf", nome, &preco);
      Filepos artigo = inserir_artigo(nome, preco);
      printf("artigo %ld\n", artigo);
      break;
    }

    case 'n':
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
    }
    case 'l':
    {
      Filepos artigo;
      int params = sscanf(cmd, "l %lu", &artigo);
      imprimir_artigo(artigo);
      printf("artigo %ld\n", artigo);
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
