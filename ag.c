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

/*
1- ler do ficheiro vendas e guarda num array (guardar o numero de bytes que se leram)
2- mandar isso para o agregador
3- o agregador devolve o resultado
4- o servidor escreve no novo ficheiro esse resultado
*/

int c = 0;
int main(int argc, char** arv){

  int in = open(FVENDAS, O_RDWR);

  pid_t filho;
  int p1[2];
  int p2[2];

  if(pipe(p1) < 0){
    exit(-1);
  }
  if(pipe(2) < 0){
    exit(-1);
  }


  if((filho = fork()) == 0){


    int dif = 0;


    // Tratar do pipe de entrada FILHO
    close(p1[1]);
    dup2(p1[0], 0);
    close(p1[0]);

    // Tratar do pipe de saída FILHO
    close(p2[0]);
    dup2(p2[1], 1);
    close(p2[1]);

    Venda *vendas;
    Venda venda;
    Venda aux;
    int i;
    double quantidade = 0, preco = 0;


    // Receção das entradas do ficheiro de vendas vindo do pipe
    while(read(0, venda, sizeof(Venda))){ // vai ler para a venda
      //

      // Inicializar uma Venda que vamos ler do ficheiro
      for(i=0, i<dif; i++){
        read(vendas[i], aux, sizeof(Venda));

        if(aux.codigo == venda.codigo){

          //somar a quantidade e preco se os codigos forem iguais
          quantidade = aux.quantidade  + venda.quantidade;
          preco = aux.preco + venda.quantidade;

          vendas[i].quantidade = quantidade;
          vendas[i].preco = preco;

          break;

        }
      }
      if(i == dif){
        vendas[i] = venda;
        dif++;
      }

    }
    //ir para o inicio do array
    i = 0;

    //Ler o resultado e mandar para o servidor

    while(read(vendas[i], venda, sizeof(Venda)) > 0){
      write(1, venda, sizeof(Venda));
    }

    //fechar pipe apos a conclusao da escrita
    close(1);

  }
  else{

    close(p1[0]);
    close(p2[1]);

    Venda v;

    //lerdo ficheiro das VENDAS
    while(read(in, v, sizeof(Venda))){
      printf("ENVIANDO -> Code: %d, Quantity: %d, Price: %.2f\n", v.codigo, v.quantidade, v.preco);
      write(p1[1], v, sizeof(Venda));
    }

    //fechar quando se acaba de enviar
    close(p1[1]);

    //receber conjunto agregado do FILHO
    while(read(p2[0], v, sizeof(Venda))){
        printf("RECEBENDO -> Code: %d, Quantity: %d, Price: %.2f\n", v.codigo, v.quantidade, v.preco);
    }

  }

	return 0;
}
