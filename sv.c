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


void print(int output, char * str) {
  write(output, str, strlen(str));
  write(output, "\n", strlen("\n"));
}

//dá o montante total de uma determinada quantidade de um artigo
double preco_total(ArtIndex artigo, double quantidade) {
  Artigo registo;
  int fd = open(FARTIGOS, O_CREAT | O_RDWR, 0660);
  lseek(fd, (artigo - 1) * sizeof(Artigo), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  double total = registo.preco * quantidade;
  close(fd);
  return total;
}


//decrementa o stock quando há vendas
int modifica_stock(ArtIndex artigo, double quantidade, int output) {
  Stock registo;
  int fd = open(FSTOCKS, O_CREAT | O_RDWR, 0660);
  lseek(fd, artigo * sizeof(Stock), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  if (registo.quantidade-quantidade < 0){
    print(output, "não tem stock disponivel\n" );
    return 0;}
  else registo.quantidade -= quantidade;
  lseek(fd, artigo * sizeof(Stock), SEEK_SET);
  write(fd, &registo, sizeof(registo));
  close(fd);
  return 1;
}


//incrementa o stock quando são adicionados artigos
int atualiza_stock(Filepos codigo, int quantidade){
  Stock registo;
  int fd = open(FSTOCKS, O_CREAT | O_RDWR, 0660);
  lseek(fd, codigo * sizeof(Stock), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  registo.quantidade += quantidade;
  printf("quantidade %f", registo.quantidade);
  lseek(fd, codigo * sizeof(Stock), SEEK_SET);
  write(fd, &registo, sizeof(registo));
  close(fd);
  return 0;
}


//insere uma venda no ficheiro vendas
ArtIndex inserir_venda(ArtIndex codigo, double quantidade,int output){
  Venda novo;
  novo.codigo = codigo;
  novo.quantidade = -1 * quantidade;
  novo.total = preco_total(codigo,quantidade);
  novo.tempo = time(&novo.tempo);
  int fd = open(FVENDAS, O_CREAT | O_RDWR, 0660);
  Filepos pos = lseek(fd, 0, SEEK_END);
  if (modifica_stock(codigo, quantidade, output) == 1){
    write(fd, &novo, sizeof(novo));
    close(fd);
    return 1 + pos / sizeof(Venda);
  }
  else {
    print(output, "Venda invalida por falta de stock\n");
    close(fd);
    return 0;
  }

}

//imprime a venda
void imprimir_venda(ArtIndex venda, int output) {
  Venda registo;
  int fd = open(FVENDAS, O_CREAT | O_RDONLY, 0660);
  Filepos pos = lseek(fd, (venda - 1), SEEK_SET);
  if (read(fd, &registo, sizeof(registo)) < sizeof(registo))
    print(output, "esse artigo nao existe\n");
  else {
    char str[200];
    sprintf(str, "codigo %ld\nquantidade %lf\npreço total %lf\ntempo:%s", registo.codigo, registo.quantidade, registo.total, ctime(&registo.tempo));
    print(output, str);
  }
  close(fd);
}


//imprime o stock
void imprimir_stock(ArtIndex stock, int output) {
  Stock registo;
  int fd = open(FSTOCKS, O_CREAT | O_RDONLY, 0660);
  Filepos pos = lseek(fd, stock * sizeof(Stock), SEEK_SET);
  if (read(fd, &registo, sizeof(registo)) < sizeof(registo))
    print(output, "esse artigo nao existe\n");
  else {
    char str[200];
    sprintf(str, "id: %ld\nquantidade:%lf\n", stock, registo.quantidade);
    print(output, str);
  }
  close(fd);
}


//interpretar os pedidos do cliente
void interpretar_linha(char *input, int output) {
  char str[200], nomesaida[200], *cmd;
  char comandos[5];

  sscanf(input, "%s", nomesaida, sizeof(nomesaida));
  cmd = &input[strlen(nomesaida) + 1];

  char *string, *found;
  string = strdup(input);
  int i =0;
  while( (found = strsep(&string," ")) != NULL){
    sprintf(comandos, "%s", found);
  }
  printf("comandos : %d %d", comandos[0], comandos[1]);

  if ((output = open(nomesaida, O_WRONLY)) < 0)
    perror("Erro ao abrir o fifo de saida!\n");

  printf("i:%d\n",i);
  switch (i+1) {
    case 1:
    {
      Filepos stock;
      int params = sscanf(comandos, "%lu", &stock);
      printf("comando: %d\n",params );
      imprimir_stock(stock, output);
      snprintf(str, sizeof(str), "stock %ld\n", stock);
      printf("ssss %d %s",output, str);
      break;
      printf("puuuuuta\n" );

    }
    case 2:
    {
      if(comandos[1]<0){
          ArtIndex codigo;
          double quantidade;
          sscanf(comandos[0], "%ld", &codigo);
          sscanf(comandos[1], "%lf", &quantidade);
          printf("%f\n",quantidade );
          Filepos venda = inserir_venda(codigo, quantidade, output);
          snprintf(str, sizeof(str), "Venda %ld\n", venda);
          print(output, str);
          break;
        }
      if(comandos[1]>0){
          printf("hello\n");
          Filepos stock;
          double quantidade;
          sscanf(comandos[0], "%lu", &stock);
          sscanf(comandos[1], "%lf", &quantidade);
          atualiza_stock(stock,quantidade);
          imprimir_stock(stock,output);
          //snprintf(str, sizeof(str), "venda %ld\n", venda);
          //print(output, str);
          break;
        }
      }

    default:
      snprintf(str, sizeof(str), "operacao invalida: %s\n", cmd);
      print(output, str);
  }
  close(output);
}


int fifo_entrada = 0, fifo_saida = 1;

void criar_fifos() {
  mkfifo("fifo-servidor", 0660);

  if ((fifo_entrada = open("fifo-entrada", O_RDONLY)) < 0)
    perror("Erro ao abrir o fifo de entrada!\n");
}


void atender_pedidos() {
  char buffer[200];
  int len, rd;
  printf("a ler do pipe\n");
  fflush(stdout);
  while ((rd = read(fifo_entrada, &len, sizeof(len))) > 0) {
    rd += read(fifo_entrada, &buffer, len - sizeof(int));
    printf("LER %d/%d, buffer %s\n", rd, len, buffer);

    interpretar_linha(buffer, fifo_saida);

  }
  close(rd);
}


int main() {
  //char cmd[200];
  //while (fgets(cmd, sizeof(cmd), stdin) > 0) {
  //  if (strlen(cmd) > 0)
  //    interpretar_linha(cmd);
  //}
  criar_fifos();
  atender_pedidos();
}
