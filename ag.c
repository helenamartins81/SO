#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <math.h>
#include "defs.h"

int main(int argc, char** argv){
    int vendas = open(FVENDAS, O_RDWR);
    pid_t son;

    int p1[2];
    int p2[2];


    if(pipe(p1) < 0 || pipe(p2) < 0){
        exit(1);
    }

    if((son = fork())==0){
        char ad[512];

        sprintf(ad, "%d", getpid());
        int sumFile = open(ad, O_RDWR | O_CREAT, 0666);
        int dif = 0;

        close(p1[1]);
        dup2(p1[0], 0);
        close(p1[0]);

        close(p2[0]);
        dup2(p2[1], 1);
        close(p2[1]);

        Venda venda;
        Venda venda_aux;
        int i;
        int quant = 0;
        double preco = 0.0;

        while(read(0, &venda, 16)){

            lseek(sumFile, 0, SEEK_SET);
            for(i = 0; i < dif; i++){
                read(sumFile, &venda_aux, 16);

                if(venda_aux.codigo == venda.codigo){
                    quant = venda_aux.quantidade + venda.quantidade;
                    preco = venda_aux.total + venda.total;

                    lseek(sumFile, (i*16) + 4, SEEK_SET);
                    write(sumFile, &quant, 4);
                    write(sumFile, &preco, 8);

                    break;
                }
            }

            if(i == dif){
                lseek(sumFile, 0, SEEK_END);
                write(sumFile, &venda, 16);
                dif++;
            }

        }

        lseek(sumFile, 0, SEEK_SET);

        while(read(sumFile, &venda, 16) > 0){
            write(1, &venda, 16);
        }
        close(1);
        execlp("rm", "rm", ad, NULL);

    } else {
        close(p1[0]);
        close(p2[1]);

        Venda venda;
        while(read(vendas, &venda, 16)){
            printf("Codigo: %ld, Quantidade: %f, Preco: %.2f\n", venda.codigo, venda.quantidade, venda.total);
            write(p1[1], &venda, 16);
        }

        close(p1[1]);

        while(read(p2[0], &venda, 16)){
            printf("Codigo: %ld, Quantidade: %f, Preco: %.2f\n", venda.codigo, venda.quantidade, venda.total);
        }
    }
}
