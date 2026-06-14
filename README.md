# 🏦 Sistema de Manutenção de Contas em C

Projeto da disciplina de Programação em C — Sistema de gerenciamento de contas bancárias usando **arquivo binário com registros de tamanho fixo** e as funções `fseek()`, `fread()`, `fwrite()` e `rewind()`.

---

## 📋 Funcionalidades

| Opção | Descrição |
|-------|-----------|
| 1 | Cadastrar novo cliente em uma posição específica |
| 2 | Consultar cliente pelo número da conta |
| 3 | Atualizar saldo de um cliente |
| 4 | Encerrar conta (marcação lógica) |
| 5 | Listar todos os clientes ativos |
| 6 | Repetir listagem usando `rewind()` |
| 7 | Encerrar o programa |

---

## 🗂️ Estrutura do Registro

```c
typedef struct {
    int    conta;           // número da conta (= posição no arquivo)
    char   nome[50];        // nome do titular
    double saldo;           // saldo atual
    int    ativo;           // 1 = ativo | 0 = encerrado
} Cliente;
```

Cada registro ocupa **exatamente `sizeof(Cliente)` bytes**, o que permite acesso direto por posição usando `fseek()`.

---

## ⚙️ Como Compilar e Executar

### Pré-requisito
Qualquer compilador C (GCC recomendado).

```bash
# Compilar
gcc contas.c -o contas

# Executar
./contas          # Linux / macOS
contas.exe        # Windows
```

---

## 🧠 Conceitos-chave

### `fseek()` — Acesso direto por posição
```c
// Para acessar a posição N do arquivo:
fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);
```
Como todos os registros têm o mesmo tamanho (`sizeof(Cliente)`), multiplicar o índice pelo tamanho do struct dá exatamente o byte inicial daquele registro. Isso torna a busca **O(1)** — sem precisar varrer o arquivo.

### `fwrite()` / `fread()` — Leitura e escrita binária
```c
fwrite(&c, sizeof(Cliente), 1, fp);  // grava 1 registro
fread(&c,  sizeof(Cliente), 1, fp);  // lê 1 registro
```
A leitura/escrita binária preserva a estrutura exata dos dados em memória, sem conversão de texto.

### `rewind()` — Volta ao início
```c
rewind(fp);  // equivalente a fseek(fp, 0, SEEK_SET)
```
Reposiciona o ponteiro de leitura no byte 0 do arquivo, permitindo relistar os registros sem fechar e reabrir o arquivo.

### Encerramento lógico vs. físico
A opção 4 **não apaga** o registro do arquivo. Em vez disso, marca o campo `ativo = 0`. Isso preserva as posições dos demais registros e evita deslocamentos no arquivo. A posição fica disponível para um futuro recadastro.

---

## 📁 Arquivo Gerado

O programa cria `contas.dat` no diretório de execução. É um arquivo binário — não é legível em editores de texto normais.

---

## 🖥️ Exemplo de Uso

```
╔══════════════════════════════════════╗
║      SISTEMA DE CONTAS BANCÁRIAS     ║
╠══════════════════════════════════════╣
║  1. Cadastrar novo cliente           ║
║  2. Consultar cliente                ║
...
  Opção: 1

╔══════════════════════════════════════╗
║  CADASTRAR NOVO CLIENTE              ║
╚══════════════════════════════════════╝
  Posição (0, 1, 2, ...): 0
  Nome: João Rafael
  Saldo inicial: R$ 1500.00
  ✔  Cliente cadastrado na posição 0.
```

---

## 📚 Referências

- Livro: *Linguagem C Completa e Descomplicada* — André Backes  
- Documentação: `man fseek`, `man fread`, `man fwrite`
