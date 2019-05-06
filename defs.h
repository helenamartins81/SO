#define FVENDAS "vendas.txt"
#define FSTRINGS "strings.txt"
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
  double quantidade;
} Stock;

