# Sistema de Cadastro de Compras em C

Programa de desktop para Windows, escrito em C com a API Win32, que permite cadastrar compras, calcular o total de cada item e visualizar o histórico salvo em arquivo.

## Funcionalidades

- Cadastro de compras com produto, quantidade e preço unitário
- Cálculo automático do total (quantidade × preço)
- Aceita vírgula ou ponto como separador decimal
- Gravação das compras no arquivo `aval1234.txt`
- Listagem de todas as compras cadastradas, com o total geral

## Requisitos

- Windows
- Compilador C (MinGW/GCC ou MSVC)

## Como compilar

Com MinGW (GCC):

```bash
gcc main.c -o cadastro.exe -mwindows -luser32 -lgdi32
```

Com MSVC (Developer Command Prompt):

```bash
cl main.c
```

> **Nota:** a linha `#pragma comment(lib, "user32.lib")` no código é opcional e serve apenas para quem testa o programa no VS Code (MSVC), pois faz o linker incluir a biblioteca automaticamente. Compiladores GCC ignoram essa linha, por isso as bibliotecas precisam ser passadas no comando, como mostrado acima.

## Como usar

1. Execute `cadastro.exe`.
2. Preencha o nome do produto, a quantidade e o preço unitário.
3. Clique em **Salvar compra** para gravar no arquivo.
4. Clique em **Mostrar compras** para listar o histórico e ver o total geral.
5. Clique em **Limpar** para limpar os campos e a lista.

## Formato do arquivo

Cada compra é salva em uma linha, com os campos separados por `;`:

```
produto;quantidade;preco_unitario;total
```

Exemplo:

```
Arroz;2;15.50;31.00
```

## Observações

- O nome do produto não pode conter ponto e vírgula.
- A quantidade deve ser um inteiro positivo.
- O preço não pode ser negativo.
