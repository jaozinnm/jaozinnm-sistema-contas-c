/*
 * Sistema de Manutenção de Contas
 * Arquivo binário com registros de tamanho fixo
 * Uso de fseek(), fread(), fwrite() e rewind()
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME   50
#define ARQUIVO    "contas.dat"

/* ───────────── Estrutura do registro ───────────── */
typedef struct {
    int    conta;           /* número da conta      */
    char   nome[MAX_NOME];  /* nome do titular       */
    double saldo;           /* saldo atual           */
    int    ativo;           /* 1 = ativo, 0 = encerrado */
} Cliente;

/* ───────────── Utilitários ───────────── */
void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void cabecalho(const char *titulo) {
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║  %-36s║\n", titulo);
    printf("╚══════════════════════════════════════╝\n");
}

void pausar(void) {
    printf("\nPressione ENTER para continuar...");
    limpar_buffer();
}

/* ───────────── 1. Cadastrar cliente ───────────── */
void cadastrar(void) {
    cabecalho("CADASTRAR NOVO CLIENTE");

    FILE *fp = fopen(ARQUIVO, "r+b");
    if (!fp) fp = fopen(ARQUIVO, "w+b"); /* cria se não existir */
    if (!fp) { perror("Erro ao abrir arquivo"); return; }

    int pos;
    printf("  Posição (0, 1, 2, ...): ");
    scanf("%d", &pos);
    limpar_buffer();

    /* move para a posição desejada */
    fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);

    /* verifica se já existe registro ativo nessa posição */
    Cliente temp;
    if (fread(&temp, sizeof(Cliente), 1, fp) == 1 && temp.ativo) {
        printf("  ⚠  Posição já ocupada pelo cliente '%s'.\n", temp.nome);
        fclose(fp);
        pausar();
        return;
    }

    /* volta para a posição e grava o novo registro */
    fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);

    Cliente c;
    c.conta = pos;
    c.ativo = 1;

    printf("  Nome: ");
    fgets(c.nome, MAX_NOME, stdin);
    c.nome[strcspn(c.nome, "\n")] = '\0';

    printf("  Saldo inicial: R$ ");
    scanf("%lf", &c.saldo);
    limpar_buffer();

    fwrite(&c, sizeof(Cliente), 1, fp);
    printf("  ✔  Cliente cadastrado na posição %d.\n", pos);

    fclose(fp);
    pausar();
}

/* ───────────── 2. Consultar cliente ───────────── */
void consultar(void) {
    cabecalho("CONSULTAR CLIENTE");

    FILE *fp = fopen(ARQUIVO, "rb");
    if (!fp) { printf("  Arquivo ainda não existe.\n"); pausar(); return; }

    int pos;
    printf("  Número da conta (posição): ");
    scanf("%d", &pos);
    limpar_buffer();

    fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);

    Cliente c;
    if (fread(&c, sizeof(Cliente), 1, fp) != 1 || !c.ativo) {
        printf("  ✘  Conta %d não encontrada ou encerrada.\n", pos);
    } else {
        printf("\n  Conta  : %d\n", c.conta);
        printf("  Nome   : %s\n",  c.nome);
        printf("  Saldo  : R$ %.2f\n", c.saldo);
        printf("  Status : Ativo\n");
    }

    fclose(fp);
    pausar();
}

/* ───────────── 3. Atualizar saldo ───────────── */
void atualizar_saldo(void) {
    cabecalho("ATUALIZAR SALDO");

    FILE *fp = fopen(ARQUIVO, "r+b");
    if (!fp) { printf("  Arquivo ainda não existe.\n"); pausar(); return; }

    int pos;
    printf("  Número da conta: ");
    scanf("%d", &pos);
    limpar_buffer();

    fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);

    Cliente c;
    if (fread(&c, sizeof(Cliente), 1, fp) != 1 || !c.ativo) {
        printf("  ✘  Conta %d não encontrada ou encerrada.\n", pos);
    } else {
        printf("  Titular: %s\n", c.nome);
        printf("  Saldo atual: R$ %.2f\n", c.saldo);
        printf("  Novo saldo : R$ ");
        scanf("%lf", &c.saldo);
        limpar_buffer();

        /* volta para a posição e sobrescreve */
        fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);
        fwrite(&c, sizeof(Cliente), 1, fp);
        printf("  ✔  Saldo atualizado.\n");
    }

    fclose(fp);
    pausar();
}

/* ───────────── 4. Encerrar conta ───────────── */
void encerrar_conta(void) {
    cabecalho("ENCERRAR CONTA");

    FILE *fp = fopen(ARQUIVO, "r+b");
    if (!fp) { printf("  Arquivo ainda não existe.\n"); pausar(); return; }

    int pos;
    printf("  Número da conta: ");
    scanf("%d", &pos);
    limpar_buffer();

    fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);

    Cliente c;
    if (fread(&c, sizeof(Cliente), 1, fp) != 1 || !c.ativo) {
        printf("  ✘  Conta %d não encontrada ou já encerrada.\n", pos);
    } else {
        printf("  Confirma encerramento da conta de '%s'? (s/n): ", c.nome);
        char resp;
        scanf(" %c", &resp);
        limpar_buffer();

        if (resp == 's' || resp == 'S') {
            c.ativo = 0;
            fseek(fp, (long)pos * sizeof(Cliente), SEEK_SET);
            fwrite(&c, sizeof(Cliente), 1, fp);
            printf("  ✔  Conta encerrada.\n");
        } else {
            printf("  Operação cancelada.\n");
        }
    }

    fclose(fp);
    pausar();
}

/* ───────────── 5. Listar todos ───────────── */
void listar(FILE *fp) {
    cabecalho("LISTAR CLIENTES");

    if (!fp) { printf("  Arquivo ainda não existe.\n"); pausar(); return; }

    Cliente c;
    int encontrou = 0, pos = 0;

    printf("  %-6s %-30s %15s\n", "Conta", "Nome", "Saldo");
    printf("  %-6s %-30s %15s\n", "------", "------------------------------", "---------------");

    while (fread(&c, sizeof(Cliente), 1, fp) == 1) {
        if (c.ativo) {
            printf("  %-6d %-30s R$ %12.2f\n", c.conta, c.nome, c.saldo);
            encontrou = 1;
        }
        pos++;
    }

    if (!encontrou)
        printf("  Nenhum cliente ativo cadastrado.\n");

    pausar();
}

/* ───────────── 6. Rewind + relistar ───────────── */
void relistar(FILE *fp) {
    if (!fp) { printf("  Arquivo ainda não existe.\n"); pausar(); return; }
    printf("\n  ↺  Voltando ao início do arquivo com rewind()...\n");
    rewind(fp);
    listar(fp);
}

/* ───────────── Menu principal ───────────── */
int main(void) {
    int opcao;

    /* mantém o arquivo aberto para as operações de listagem / rewind */
    FILE *fp_lista = fopen(ARQUIVO, "rb");

    do {
        printf("\n╔══════════════════════════════════════╗\n");
        printf("║      SISTEMA DE CONTAS BANCÁRIAS     ║\n");
        printf("╠══════════════════════════════════════╣\n");
        printf("║  1. Cadastrar novo cliente           ║\n");
        printf("║  2. Consultar cliente                ║\n");
        printf("║  3. Atualizar saldo                  ║\n");
        printf("║  4. Encerrar conta                   ║\n");
        printf("║  5. Listar todos os clientes         ║\n");
        printf("║  6. Repetir listagem (rewind)        ║\n");
        printf("║  7. Encerrar programa                ║\n");
        printf("╚══════════════════════════════════════╝\n");
        printf("  Opção: ");
        scanf("%d", &opcao);
        limpar_buffer();

        switch (opcao) {
            case 1:
                cadastrar();
                /* reabre fp_lista para refletir novos registros */
                if (fp_lista) fclose(fp_lista);
                fp_lista = fopen(ARQUIVO, "rb");
                break;
            case 2: consultar();       break;
            case 3:
                atualizar_saldo();
                if (fp_lista) fclose(fp_lista);
                fp_lista = fopen(ARQUIVO, "rb");
                break;
            case 4:
                encerrar_conta();
                if (fp_lista) fclose(fp_lista);
                fp_lista = fopen(ARQUIVO, "rb");
                break;
            case 5: listar(fp_lista);  break;
            case 6: relistar(fp_lista); break;
            case 7: printf("\n  Até logo!\n\n"); break;
            default: printf("  Opção inválida.\n");
        }

    } while (opcao != 7);

    if (fp_lista) fclose(fp_lista);
    return 0;
}
