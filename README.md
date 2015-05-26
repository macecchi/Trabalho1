# Solução paralela para o Problema do Caixeiro Viajante
Apresentamos soluções para o problema do Caixeiro Viajante através da busca em árvore. A solução paralela do problema foi desenvolvida com a biblioteca concorrente **OpenMP** e pode ser executada com um número arbitrário de threads.

O trabalho foi desenvolvido para a turma de 2015.1 da disciplina eletiva Programação Paralela e Distribuída (MAB622), oferecida pelo Departamento de Ciência da Computação da UFRJ e ministrada pela professora Silvana Rossetto.

A especificação do trabalho pode ser encontrada no arquivo *trab1.pdf*.

## Execução

Um exemplo de entrada pode ser encontrado no arquivo `input.txt`.

### Compilação

Sequencial: `gcc sequencial.c -o sequencial`

Paralelo: `gcc paralelo.c -o paralelo -fopenmp`

### Execução

Sequencial: `./sequencial <input.txt`

Paralelo: `./paralelo <num_threads> <input.txt`


## Descrição 

### Considerações iniciais

Para resolver o problema da busca em árvore, definimos duas estruturas:

* **tour_t**, que representa um tour na árvore, contendo um vetor de cidades percorridas, o custo total e o número de cidades
* **pilha_t**, que representa uma pilha que irá guardar os tours, contendo um vetor de tours, um índice representando qual o topo da pilha e um inteiro indicando o tamanho da pilha.

Cada uma dessas estruturas possui suas relativas funções utilitárias, como as de acesso à pilha e as de atualização do tour. Os cabeçalhos destas funções estão definidos no começo do programa e os corpos no final.

A entrada do programa deve ser feita pela entrada padrão (*stdin*) e deve ser iniciada por dois inteiros - um contendo o tamanho da entrada **n** (número de cidades) e outro com o índice da cidade inicial. Em seguida, é esperada uma matriz de tamanho nxn, onde o elemento de uma célula ci,j contém o custo partindo da cidade i até a cidade j.

### Solução sequencial
 
Para executar a solução paralela, o usuário deve apenas especificar qual a entrada do programa através da entrada padrão. Isso pode ser feito simplesmente especificando um arquivo pelo bash, como no exemplo a seguir:

`./sequencial <input.txt`

Após fazer a leitura da entrada, criamos uma pilha que contém apenas a cidade inicial. Em seguida, um loop while pega o tour do topo da pilha e verifica se esse tour é completo, isto é, se já passou por todas as cidades. Caso esteja completo, adicionamos a cidade inicial com seu respectivo custo e verificamos se esse tour é o melhor até agora.

Caso o tour ainda não seja completo, executamos um loop com todas as cidades, tentando as inserir no tour. Elas só serão inseridas se formarem um tour completo ou se a cidade ainda não estiver no tour. Assim, duplicamos esse tour e o inserimos na pilha. Para continuar o loop para a próxima cidade, a removemos desse tour temporário e repetimos estes passos para a próxima.

O while inicial só irá encerrar quando esta pilha estiver vazia, o que vai acontecer quando todos os tours já estiverem completos, logo. quando todas as rotas forem verificadas.

### Solução paralela

Para executar a solução paralela, o usuário deve especificar nos argumentos do programa o número de threads que deseja executar. A chamada do programa deve ficar neste formato, por exemplo, se quiser executar com 4 threads e lendo a entrada do arquivo input.txt:

`./paralelo 4 <input.txt`

A lógica da nossa solução paralela consiste em dividir as tarefas de acordo com as ramificações iniciais das árvores. Dessa maneira, se tenho n caminhos a partir da cidade inicial, terei n pilhas que podem ser executadas em paralelo.
	
Para realizar essa paralelização, o loop que verifica os tours contidos na pilha foi separado dentro da função calcular_custo_minimo. Criamos inicialmente as n pilhas através de um for para o número de cidades e chamamos para cada um essa função, passando como argumento seu respectivo tour inicial. Para dividir essa tarefa entre as threads, uso a diretiva parallel for do OpenMP. Para garantir a sincronização do programa, precisamos apenas definir como região crítica as funções que testam e atualizam o melhor tour encontrado.

## Resultados

Os testes a seguir foram executados numa máquina equipada com Intel Core i5.

10 cidades

* Sequencial: 0.019 segundos
* Paralelo com 2 threads: 0.021 segundos (speedUp: 0.90 / eficiência: 0.45)
* Paralelo com 4 threads: 0.015 segundos (speedUp: 1.26 / eficiência: 0.31)

13 cidades

* Sequencial: 1.60 segundos
* Paralelo com 2 threads: 1.33 segundos (speedUp: 1.20 / eficiência: 0.60)
* Paralelo com 4 threads: 1.00 segundos (speedUp: 1.60 / eficiência: 0.40)

15 cidades

* Sequencial: 29.3 segundos
* Paralelo com 2 threads: 18.39 segundos (speedUp: 1.59 / eficiência: 0.79)
* Paralelo com 4 threads: 16.98 segundos (speedUp: 1.72 / eficiência: 0.43)

17 cidades

* Sequencial: 699 segundos
* Paralelo com 2 threads: 489.59 segundos (speedUp: 1.42 / eficiência: 0.71)
* Paralelo com 4 threads: 411.92 segundos (speedUp: 1.69 / eficiência: 0.42)


