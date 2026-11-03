# Sistema de Cadastro de Compras (C / Win32 API)

Projeto demonstrativo desenvolvido com base nos requisitos e nos trechos de código apresentados no trabalho ARA0363.

## Funcionalidades
- Cadastro de produto, quantidade e preço unitário.
- Validação de campos e de valores numéricos.
- Cálculo automático do total (quantidade × preço).
- Gravação em `aval1234.txt`, acrescentando registros ao final.
- Leitura dos registros e exibição em uma lista.
- Cálculo do total acumulado e da quantidade de compras.

## Compilação no Windows

### MinGW-w64
Abra o terminal na pasta do projeto e execute:

```bash
gcc main.c -o cadastro_compras.exe -mwindows
```

Depois, execute `cadastro_compras.exe`.

### Code::Blocks / Dev-C++
Crie um projeto C para Windows, adicione `main.c` e compile com um compilador MinGW que tenha suporte à API Win32.

## Formato do arquivo
Cada linha de `aval1234.txt` contém:

`produto;quantidade;preco;total`

Exemplo:
`Teclado;2;100.00;200.00`

## Teste sugerido (cinco inserções diferentes)
1. Teclado | 2 | 100,00
2. Mouse | 3 | 50,00
3. Monitor | 1 | 800,00
4. Caderno | 4 | 15,50
5. Fone | 2 | 75,00

Clique em **Mostrar compras** para exibir os registros e o acumulado.

## Observação
Este é um projeto de referência criado a partir do enunciado e dos fragmentos fornecidos. Não é possível garantir que seja idêntico ao código-fonte original do professor, pois esse código não foi anexado.
