#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "timer.h"

struct rota{
		int origemID;
		int destinoID;
		int custo;
};

struct city{
	int id;
	struct rota *ligacoes;
};

struct tour
{
	int *cities;
	int cost;
	int num_cities;
};

typedef struct tour stackElementT;

typedef struct {
  stackElementT *contents;
  int size;
  int top;
} stackT;

void StackInit(stackT *stackP)
{
  stackP->contents = (stackElementT *) malloc(sizeof(stackElementT) * 1);
  /* Allocate a new array to hold the contents. */

  stackP->size = 1;
  stackP->top = -1;  /* I.e., empty */
}

void StackDestroy(stackT *stackP)
{
  /* Get rid of array. */
  free(stackP->contents);

  stackP->contents = NULL;
  stackP->size = 0;
  stackP->top = -1;  /* I.e., empty */
}

int StackIsEmpty(stackT *stackP)
{
  return stackP->top < 0;
}

int StackIsFull(stackT *stackP)
{
  return stackP->top >= stackP->size - 1;
}

void StackPush(stackT *stackP, stackElementT element)
{
  if (StackIsFull(stackP)) {
  	//printf("Full\n");
  	stackP->size++;
    stackP->contents = (stackElementT *) realloc(stackP->contents, sizeof(stackElementT) * stackP->size);
  }


  /* Put information in array; update top. */
  //printf("push antes\n");
  stackP->contents[++stackP->top] = element;
  //printf("push depois\n");
}

stackElementT StackPop(stackT *stackP)
{
  if (StackIsEmpty(stackP)) {
    fprintf(stderr, "Can't pop element from stack: stack is empty.\n");
    exit(1);  /* Exit, returning error code. */
  }

  return stackP->contents[stackP->top--];
}

int cityInTour(struct tour t, int cidade){
	int i;
	for (i = 0; i < t.num_cities; i++)
	{
		if (t.cities[i] == cidade)
		{
			return 1;
		}
	}
	return 0;
}

int checkTour(struct tour t, int cidade, int cidadeInicial, int num_cidades)
{
	if (t.num_cities == num_cidades && cidade == cidadeInicial)
	{
		return 1;
	}
	else if (!cityInTour(t, cidade))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void printTour(struct tour t)
{
	int i;
	printf("Rota ");
	for (i = 0; i < t.num_cities; i++)
	{
		printf("%d ", t.cities[i]);
	}
	printf("\n");
	printf("Custo = %d\n", t.cost);
}

void addCity(struct tour *t, int cidade, int num_cidades, int **rotas)
{
	//printf("addCity\n");
	int cidadeOrigem = t->cities[t->num_cities - 1];
	t->cost += rotas[cidadeOrigem][cidade];
	t->cities[t->num_cities++] = cidade;
}

void removeCity(struct tour *t, int cidade, int num_cidades, int **rotas)
{
	//printf("removeCity\n");
	//printTour(*t);
	//printf("%d ", t->num_cities);
	t->num_cities--;
	int cidadeOrigem = t->cities[t->num_cities - 1];
	t->cost -= rotas[cidadeOrigem][i];
}

void copyTour(struct tour *k, struct tour t, int num_cidades){
	k->cities = (int*) malloc((num_cidades+1)*sizeof(int));
	k->num_cities = t.num_cities;
	k->cost = t.cost;
	memcpy(k->cities, t.cities, (num_cidades+1)*sizeof(int));
}

void initTour(struct tour *t, int cidadeInicial, int num_cidades)
{
	t->cities = (int*) malloc((num_cidades+1)*sizeof(int));
	t->cost = 0;
	t->num_cities = 1;
	t->cities[0] = cidadeInicial;
}

void calculateMinimumCost(struct tour tourInicial, struct tour *bestTour, int num_cidades, int cidadeInicial, int *cidades, int **rotas)
{
	int i;
	struct tour t;
	stackT stack;
	StackInit(&stack);

	StackPush(&stack, tourInicial);
	while(!StackIsEmpty(&stack)){
		t = StackPop(&stack);
		if (t.num_cities == num_cidades + 1)
		{
		# 	pragma omp critical
			if (t.cost < bestTour->cost)
			{
				*bestTour = t;
			}
		}
		else{

			for (i = num_cidades - 1; i >= 0; i--)
			{
				struct tour k;
				if (checkTour(t, i, cidadeInicial, num_cidades))
				{
					copyTour(&k, t, num_cidades);
					addCity(&k, cidades[i], num_cidades, rotas);
					StackPush(&stack, k);
					removeCity(&k, cidades[i], num_cidades, rotas);
				}
			}
			free(t.cities);
		}
	}
	StackDestroy(&stack);
# 	pragma omp critical
	if (bestTour->cities != t.cities) free(t.cities);

}

void printVector(int *cities, int num_cidades){
	int i;
	for (i = 0; i < num_cidades; i++)
	{
		printf("%d ", cities[i]);
	}
	printf("\n");
}

void printStack(stackT stack){
	while(!StackIsEmpty(&stack)){
		struct tour t = StackPop(&stack);
		printVector(t.cities, t.num_cities);
	}
}

int main(int argc, char *argv[]){
	int i, j, thread_count;
	int num_cidades;
	int cidadeInicial;
	int **matriz;
	int *cidades;
	struct tour t;
	struct tour bestTour;
	stackT stack; 
	stackT stackTemp;
	double start, finish;
	struct tour *tourIniciais;


	bestTour.cost = 99999;

	thread_count = strtol(argv[1], NULL, 10);

	scanf("%d", &num_cidades);
	scanf("%d", &cidadeInicial);

	cidades = (int*) malloc(num_cidades*(sizeof(int)));
	tourIniciais = (struct tour*) malloc((num_cidades-1)*(sizeof(struct tour)));
	matriz = (int **)malloc(num_cidades*sizeof(int*));

	for (i = 0; i < num_cidades; i++)
	{
		cidades[i] = i;
		matriz[i] = (int *)malloc(num_cidades*sizeof(int));
		for (j = 0; j < num_cidades; j++)
		{
			//printf("%d %d\n", i, j);
			scanf("%d", &matriz[i][j]);
		}
	}

	// for (i = 0; i < num_cidades; i++)
	// {
	// 	printf("Cidade %d\n", cidades[i].id);
	// 	for (j = 0; j < num_cidades; j++)
	// 	{
	// 		printf("Rota: origem %d, destino %d, custo %d\n", cidades[i].ligacoes[j].origemID, cidades[i].ligacoes[j].destinoID, cidades[i].ligacoes[j].custo);
	// 	}
	// }

	initTour(&t, cidades[cidadeInicial], num_cidades);

	GET_TIME(start);
	j = 0;
	for (i = 0; i < num_cidades; i++)
	{
		if (i != cidadeInicial)
		{
			initTour(&tourIniciais[j], cidades[cidadeInicial], num_cidades);
			addCity(&tourIniciais[j], cidades[i], num_cidades, matriz);
			j++;
		}
	}
#	pragma omp parallel for num_threads(thread_count)
	for (i = 0; i < num_cidades-1; i++)
	{
		calculateMinimumCost(tourIniciais[i], &bestTour, num_cidades, cidadeInicial, cidades, matriz);
	}	

	
	GET_TIME(finish);

	printf("finish\n");
	printTour(bestTour);
	printf("tempo=%f\n", finish-start);

	free(bestTour.cities);
	free(tourIniciais);
	//if (bestTour.cities != t.cities) free(t.cities);
	free(cidades);
	free(matriz);

	return 0;
}
