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

char * myfifo = "fifo-mensagem";
char *myfifo2 = "fifo2";
char resposta[200];


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
  int fd = open(FSTOCKS, O_RDWR, 0660);
  lseek(fd, artigo * sizeof(Stock), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  if (registo.quantidade-(-1*quantidade) < 0){

    return 0;}
  else registo.quantidade = registo.quantidade - (-1*quantidade);
  lseek(fd, artigo * sizeof(Stock), SEEK_SET);
  write(fd, &registo, sizeof(registo));
  close(fd);
  return 1;
}


//incrementa o stock quando são adicionados artigos
int atualiza_stock(Filepos codigo, int quantidade){
  Stock registo;
  int fd = open(FSTOCKS, O_RDWR, 0666);
  lseek(fd, codigo * sizeof(Stock), SEEK_SET);
  read(fd, &registo, sizeof(registo));
  registo.quantidade += quantidade;
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
    sprintf(resposta, "quantidade: %lf\n", registo.quantidade);
  }
  close(fd);
}


//imprime o stock
void imprimir_stock(ArtIndex stock, int output) {
  Stock registo;
  int fd = open(FSTOCKS, O_CREAT | O_RDONLY, 0666);
  Filepos pos = lseek(fd, stock * sizeof(Stock), SEEK_SET);
  if (read(fd, &registo, sizeof(registo)) < sizeof(registo))
    print(output, "esse artigo nao existe\n");
  else {
    char str[200];
    sprintf(str, "id: %ld\nquantidade:%lf\n", stock, registo.quantidade);
    print(output, str);
    sprintf(resposta, "quantidade: %lf\n", registo.quantidade);
  }
  close(fd);
}


//interpretar os pedidos do cliente
void interpretar_linha(char *input, int output) {
  char str[200], nomesaida[200], *cmd;
  char *dest[3];

  int fifo_saida = mkfifo("output_comando",0666);


  sscanf(input, "%s", nomesaida);
  cmd = &input[strlen(nomesaida) + 1];

  char *st = input;
  int i;
  for (i = 0; i < 3 && (dest[i] = strsep(&st, " ")) != NULL; i++);


  switch (i) {
    case 2:
    {
      Filepos stock;
      sscanf(dest[1], "%lu", &stock);
      imprimir_stock(stock, output);
      snprintf(str, sizeof(str), "stock %ld\n", stock);
      break;

    }
    case 3:
    {
      if(atoi(dest[2])>0){
        Filepos stock;
        double quantidade;
        sscanf(dest[1], "%lu", &stock);
        sscanf(dest[2], "%lf", &quantidade);
        atualiza_stock(stock,quantidade);
        imprimir_stock(stock,output);
        //snprintf(str, sizeof(str), "venda %ld\n", venda);
        //print(output, str);
        break;
      }
      if(atoi(dest[2])<0){
          ArtIndex codigo;
          double quantidade;
          sscanf(dest[1], "%ld", &codigo);
          sscanf(dest[2], "%lf", &quantidade);

          Filepos venda = inserir_venda(codigo, quantidade, output);
          imprimir_stock(codigo,output);
          snprintf(str, sizeof(str), "Venda %ld\n", venda);
          print(output, str);
          break;
        }

    default:
      snprintf(str, sizeof(str), "operacao invalida: %s\n", cmd);
  }
  close(output);
  }
}


int ler_do_pipe = 0, fifo_saida = 1;

void atender_pedidos() {
  char buffer[200];
  int fd, rd, w, out;

  char * myfifo = "fifo-mensagem";
  mkfifo(myfifo, 0666);

  if ((fd = open(myfifo, O_RDONLY)) < 0){
    perror("Erro ao abrir o fifo de entrada!\n");
    exit(-1);
  }

  while ((rd = read(fd, buffer, sizeof(buffer))) > 0) {

    interpretar_linha(buffer, fifo_saida);

  }

  close(rd);
  close(fd);

  if ((out = open(myfifo2, O_WRONLY,0666)) < 0){
      perror("Erro ao abrir o fifo de saida!\n");
      exit(-1);
  }



  write(out, &resposta, sizeof(resposta));


  close (out);

}


int main() {
    while(1)
    atender_pedidos();

}
