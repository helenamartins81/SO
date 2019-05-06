//inserir novos artigos ou modificar atributos de artigos.

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "defs.h"

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
  close(fd);
  return nome_lido;
}

Filepos inicializa_stock( Filepos codigo){
  Stock novo;
  novo.quantidade = 0;
  int fd = open(FSTOCKS, O_CREAT | O_RDWR, 0660);
  Filepos pos = lseek(fd, 0, SEEK_END);
  write(fd, &novo, sizeof(novo));
  close(fd);
  return pos / sizeof(Stock);
}

ArtIndex inserir_artigo(char *nome, double preco) {
  Artigo novo;
  novo.nome = inserir_nome(nome);
  novo.preco = preco;
  int fd = open(FARTIGOS, O_CREAT | O_RDWR, 0660);
  Filepos pos = lseek(fd, 0, SEEK_END);
  write(fd, &novo, sizeof(novo));
  inicializa_stock(pos);
  close(fd);
  return 1 + pos / sizeof(Artigo);
}

void modificar_nome(ArtIndex artigo, char *novonome) {
  Artigo registo;
  int fd = open(FARTIGOS, O_CREAT | O_RDWR, 0660);
  lseek(fd, (artigo - 1) * sizeof(Artigo), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  registo.nome = inserir_nome(novonome);
  lseek(fd, (artigo - 1) * sizeof(Artigo), SEEK_SET);
  write(fd, &registo, sizeof(registo));
  close(fd);
}

void modificar_preco(ArtIndex artigo, double novopreco) {
  Artigo registo;
  int fd = open(FARTIGOS, O_CREAT | O_RDWR, 0660);
  lseek(fd, (artigo - 1) * sizeof(Artigo), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  registo.preco = novopreco;
  lseek(fd, (artigo - 1) * sizeof(Artigo), SEEK_SET);
  write(fd, &registo, sizeof(registo));
  close(fd);
}

void imprimir_artigo(ArtIndex artigo) {
  Artigo registo;
  int fd = open(FARTIGOS, O_CREAT | O_RDONLY, 0660);
  Filepos pos = lseek(fd, (artigo - 1) * sizeof(Artigo), SEEK_SET);
  if (read(fd, &registo, sizeof(registo)) < sizeof(registo))
    printf("esse artigo nao existe\n");
  else {
    char * nome_ptr = ler_nome(registo.nome);
    printf("id: %ld\nnome:%s\npreço:%lf\n", artigo, nome_ptr, registo.preco);
  }
  close(fd);
}

void imprimir_stock(ArtIndex stock) {
  Stock registo;
  int fd = open(FSTOCKS, O_CREAT | O_RDONLY, 0660);
  Filepos pos = lseek(fd, (stock - 1) * sizeof(Stock), SEEK_SET);
  if (read(fd, &registo, sizeof(registo)) < sizeof(registo))
    printf("esse artigo nao existe\n");
  else {
    printf("id: %ld\nquantidade:%lf\n", stock, registo.quantidade);
  }
  close(fd);
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
      break;
    }
    case 's':
    {
      Filepos stock;
      int params = sscanf(cmd, "s %lu", &stock);
      imprimir_stock(stock);
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
