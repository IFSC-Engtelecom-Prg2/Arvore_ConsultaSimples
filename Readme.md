# Armazenar e consultar dados

Um arquivo chamado "nomes.txt" contém uma relação de nomes de pessoas (um por linha).
Escreva um programa que possibilite consultar se um determinado nome existe nesse arquivo. Para agilizar a consulta, os nomes devem primeiro serem carregados para uma árvore de pesquisa binária. Portanto, as consultas devem ser feitas diretamente nessa árvore.


Após criar a árvore, seu programa deve entrar em um loop. Em cada ciclo do loop o programa apresenta este prompt:

```
Consultar>
```

... e aguardar que seja fornecido um nome para ser pesquisado. O resultado da consulta deve ser apresentado da seguinte forma:

```
Nome Que Foi Consultado: EXISTE
```

Se o nome não existir, o resultado deve ser este:

```
Nome Que Foi Consultado: INEXISTENTE
```

Se em vez de uma matrícula for teclado ENTER, o programa deve terminar.

Seu programa será testado com diferentes números de consultas.