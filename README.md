## Correção do T3 - 2017/2


Alunos que submeterem correções ao gabarito que forem aceitas receberão 0,5 pontos na prova.

Para submeter uma correção:

- Criar um pull request com um nome que deixe claro qual o bug ("o tamanho do bloco deve ser múltiplo de 512")
- Deve ter uma descrição do bug
- Deve ter os passos para repetir o bug (incluindo qual o repositório do trabalho e o commit que apresenta o bug)
- Deve ter um arquivo anexo com o patch que resolve o bug para a última versão disponível do teste.c
- Deve ter o nome do aluno que descobriu o bug
- Deve ter o professor como assignee

Por exemplo,


## O tamanho do bloco deve ser múltiplo de 512

Na especificação do trabalho, linha 150, consta que o tamanho do bloco deve ser sempre múltiplo de 512. O caso de teste está aceitando 513, que não é múltiplo.

Passos para repetir:

1. Executa o teste com o meu_repo_si_to_rio, commit 123456. 
2. O resultado "deve ser múltiplo de 512" na linha 150 dá SUCESSO e deveria dar FALHA.

Patch em anexo.

Aluno: Maurício Lima Pilla <pilla@inf.ufpel.edu.br>  


[Pull requests no github](https://help.github.com/articles/about-pull-requests/)