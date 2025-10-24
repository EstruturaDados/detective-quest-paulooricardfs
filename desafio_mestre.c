/*
  Detective Quest - Capítulo Final: Acusação de Suspeitos
  - Exploração de mansão (árvore binária)
  - Coleta de pistas em BST
  - Associação pista -> suspeito via tabela hash (encadeamento)
  - Julgamento final: acusação e verificação (>=2 pistas)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOME 64
#define MAX_PISTA 128
#define HASH_SIZE 101   // número primo razoável para hash

/* ------------------ Estruturas ------------------ */

/* Nó da árvore da mansão (cada cômodo) */
typedef struct Sala {
    char nome[MAX_NOME];
    char pista[MAX_PISTA]; // string vazia se não tiver pista
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

/* Nó da BST de pistas (armazenamos contagem para pistas repetidas) */
typedef struct PistaNode {
    char pista[MAX_PISTA];
    int ocorrencias; // quantas vezes coletada
    struct PistaNode *esquerda;
    struct PistaNode *direita;
} PistaNode;

/* Entrada na tabela hash (encadeamento) - mapeia pista -> suspeito */
typedef struct HashEntry {
    char pista[MAX_PISTA];
    char suspeito[MAX_NOME];
    struct HashEntry *proximo;
} HashEntry;

/* ------------------ Protótipos (requisitos/documentação) ------------------ */

/* criarSala() – cria dinamicamente um cômodo com ou sem pista. */
Sala* criarSala(const char *nome, const char *pista);

/* explorarSalas() – navega pela árvore e ativa o sistema de pistas. */
void explorarSalas(Sala *atual, PistaNode **raizPistas, HashEntry *hash[]);

/* inserirPista() / adicionarPista() – insere a pista coletada na árvore de pistas. */
PistaNode* inserirPista(PistaNode *raiz, const char *pista);

/* inserirNaHash() – insere associação pista/suspeito na tabela hash. */
void inserirNaHash(HashEntry *hash[], const char *pista, const char *suspeito);

/* encontrarSuspeito() – consulta o suspeito correspondente a uma pista. */
const char* encontrarSuspeito(HashEntry *hash[], const char *pista);

/* exibirPistas() – imprime a árvore de pistas em ordem alfabética. */
void exibirPistas(PistaNode *raiz);

/* verificarSuspeitoFinal() – conduz à fase de julgamento final.
   Retorna o número de pistas que apontam para o suspeito acusado. */
int verificarSuspeitoFinal(PistaNode *raiz, HashEntry *hash[], const char *acusado);

/* utilitários: hash, liberar memória, strip newline */
unsigned long hash_djb2(const char *str);
void liberarSalas(Sala *raiz);
void liberarPistas(PistaNode *raiz);
void liberarHash(HashEntry *hash[]);
void strip_newline(char *s);

/* ------------------ Implementação ------------------ */

/* criarSala: aloca e inicializa uma sala (cômodo) dinamicamente */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) { perror("malloc"); exit(EXIT_FAILURE); }
    strncpy(s->nome, nome, MAX_NOME-1); s->nome[MAX_NOME-1] = '\0';
    if (pista) {
        strncpy(s->pista, pista, MAX_PISTA-1); s->pista[MAX_PISTA-1] = '\0';
    } else {
        s->pista[0] = '\0';
    }
    s->esquerda = s->direita = NULL;
    return s;
}

/* inserirPista: insere pista na BST. Se já existe, incrementa ocorrencias. */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (!pista || pista[0] == '\0') return raiz;
    if (raiz == NULL) {
        PistaNode *n = (PistaNode*) malloc(sizeof(PistaNode));
        if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
        strncpy(n->pista, pista, MAX_PISTA-1); n->pista[MAX_PISTA-1] = '\0';
        n->ocorrencias = 1;
        n->esquerda = n->direita = NULL;
        return n;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp == 0) {
        raiz->ocorrencias++;
    } else if (cmp < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else {
        raiz->direita = inserirPista(raiz->direita, pista);
    }
    return raiz;
}

/* hash function djb2 */
unsigned long hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % HASH_SIZE;
}

/* inserirNaHash: adiciona mapeamento pista -> suspeito (encadeamento) */
void inserirNaHash(HashEntry *hash[], const char *pista, const char *suspeito) {
    if (!pista || pista[0] == '\0' || !suspeito) return;
    unsigned long idx = hash_djb2(pista);
    HashEntry *novo = (HashEntry*) malloc(sizeof(HashEntry));
    if (!novo) { perror("malloc"); exit(EXIT_FAILURE); }
    strncpy(novo->pista, pista, MAX_PISTA-1); novo->pista[MAX_PISTA-1] = '\0';
    strncpy(novo->suspeito, suspeito, MAX_NOME-1); novo->suspeito[MAX_NOME-1] = '\0';
    novo->proximo = hash[idx];
    hash[idx] = novo;
}

/* encontrarSuspeito: retorna ponteiro para nome do suspeito (ou NULL se não existir) */
const char* encontrarSuspeito(HashEntry *hash[], const char *pista) {
    if (!pista) return NULL;
    unsigned long idx = hash_djb2(pista);
    for (HashEntry *e = hash[idx]; e != NULL; e = e->proximo) {
        if (strcmp(e->pista, pista) == 0) return e->suspeito;
    }
    return NULL;
}

/* explorarSalas: interação do jogador; coleta pistas automaticamente */
void explorarSalas(Sala *atual, PistaNode **raizPistas, HashEntry *hash[]) {
    char opcao;
    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        if (strlen(atual->pista) > 0) {
            printf("🔎 Pista encontrada: \"%s\"\n", atual->pista);
            // Adiciona à BST de pistas
            *raizPistas = inserirPista(*raizPistas, atual->pista);
        } else {
            printf("Nenhuma pista neste cômodo.\n");
        }

        // Menu de navegação
        printf("\nEscolha o caminho:\n");
        if (atual->esquerda) printf("(e) Esquerda -> %s\n", atual->esquerda->nome);
        if (atual->direita) printf("(d) Direita  -> %s\n", atual->direita->nome);
        printf("(s) Sair e apresentar as pistas coletadas\n> ");
        if (scanf(" %c", &opcao) != 1) { while (getchar() != '\n'); opcao = 's'; }

        if (opcao == 'e' || opcao == 'E') {
            if (atual->esquerda) atual = atual->esquerda;
            else printf("⚠️  Caminho inexistente à esquerda!\n");
        } else if (opcao == 'd' || opcao == 'D') {
            if (atual->direita) atual = atual->direita;
            else printf("⚠️  Caminho inexistente à direita!\n");
        } else if (opcao == 's' || opcao == 'S') {
            printf("\nVocê decidiu encerrar a exploração.\n");
            return;
        } else {
            printf("Opção inválida. Use 'e', 'd' ou 's'.\n");
        }
    }
}

/* exibirPistas: percorre BST em ordem e imprime pista + ocorrencias */
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf(" - \"%s\" (x%d)\n", raiz->pista, raiz->ocorrencias);
    exibirPistas(raiz->direita);
}

/* verificarSuspeitoFinal: percorre BST, usa hash para contar pistas que apontam para 'acusado' */
int verificarSuspeitoFinal(PistaNode *raiz, HashEntry *hash[], const char *acusado) {
    if (!raiz) return 0;
    int total = 0;
    // in-order traversal with accumulation
    if (raiz->esquerda) total += verificarSuspeitoFinal(raiz->esquerda, hash, acusado);
    const char *sus = encontrarSuspeito(hash, raiz->pista);
    if (sus && strcmp(sus, acusado) == 0) total += raiz->ocorrencias;
    if (raiz->direita) total += verificarSuspeitoFinal(raiz->direita, hash, acusado);
    return total;
}

/* liberarSalas: libera árvore da mansão */
void liberarSalas(Sala *raiz) {
    if (!raiz) return;
    liberarSalas(raiz->esquerda);
    liberarSalas(raiz->direita);
    free(raiz);
}

/* liberarPistas: libera BST de pistas */
void liberarPistas(PistaNode *raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

/* liberarHash: libera todos os buckets da tabela hash */
void liberarHash(HashEntry *hash[]) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashEntry *e = hash[i];
        while (e) {
            HashEntry *t = e->proximo;
            free(e);
            e = t;
        }
        hash[i] = NULL;
    }
}

/* strip newline de fgets */
void strip_newline(char *s) {
    size_t L = strlen(s);
    if (L == 0) return;
    if (s[L-1] == '\n') s[L-1] = '\0';
}

/* ------------------ Programa principal ------------------ */
int main() {
    /* --- Montagem fixa da mansão (árvore binária) --- */
    Sala *hall = criarSala("Hall de Entrada", "Pegada de lama");
    Sala *salaEstar = criarSala("Sala de Estar", "Lenço rasgado");
    Sala *cozinha = criarSala("Cozinha", "Copo quebrado");
    Sala *biblioteca = criarSala("Biblioteca", "Diário antigo");
    Sala *jardim = criarSala("Jardim", "Chave enferrujada");
    Sala *despensa = criarSala("Despensa", "Luvas sujas");
    Sala *garagem = criarSala("Garagem", "Pneu com marca estranha");
    Sala *porao = criarSala("Porão", "Marca de tinta vermelha");

    // ligações
    hall->esquerda = salaEstar; hall->direita = cozinha;
    salaEstar->esquerda = biblioteca; salaEstar->direita = jardim;
    cozinha->esquerda = despensa; cozinha->direita = garagem;
    biblioteca->esquerda = porao; // exemplo de profundidade extra

    /* --- Inicializa BST de pistas e tabela hash --- */
    PistaNode *raizPistas = NULL;
    HashEntry *hash[HASH_SIZE];
    for (int i = 0; i < HASH_SIZE; i++) hash[i] = NULL;

    /* --- Popula a tabela hash com associação pista -> suspeito --- */
    // (As associações são fixas no código)
    inserirNaHash(hash, "Pegada de lama", "Sr. Verdes");
    inserirNaHash(hash, "Lenço rasgado", "Sra. Marinho");
    inserirNaHash(hash, "Copo quebrado", "Sra. Marinho");
    inserirNaHash(hash, "Diário antigo", "Sr. Rocha");
    inserirNaHash(hash, "Chave enferrujada", "Sr. Verdes");
    inserirNaHash(hash, "Luvas sujas", "Sr. Rocha");
    inserirNaHash(hash, "Pneu com marca estranha", "Motorista");
    inserirNaHash(hash, "Marca de tinta vermelha", "Pintor");

    /* --- Início da exploração --- */
    printf("🕵️ Detective Quest — Investigue a mansão e colete pistas!\n");
    printf("Navegue com (e) esquerda, (d) direita ou (s) sair e acusar.\n");
    explorarSalas(hall, &raizPistas, hash);

    /* --- Fase final: exibir pistas coletadas e acusação --- */
    printf("\n📜 Pistas coletadas (ordenadas):\n");
    if (!raizPistas) {
        printf("Nenhuma pista coletada.\n");
    } else {
        exibirPistas(raizPistas);
    }

    char acusado[MAX_NOME];
    printf("\nQuem você acusa? (digite o nome exato do suspeito):\n> ");
    // Consome newline pendente e lê linha
    getchar(); // consome '\n' restante do último scanf
    if (!fgets(acusado, sizeof(acusado), stdin)) acusado[0] = '\0';
    strip_newline(acusado);

    if (strlen(acusado) == 0) {
        printf("Nenhum acusado informado. Encerrando sem julgamento.\n");
    } else {
        int total = verificarSuspeitoFinal(raizPistas, hash, acusado);
        printf("\nResultado da acusação contra \"%s\":\n", acusado);
        if (total >= 2) {
            printf("✅ Há %d pistas que apontam para %s. Acusação sustentada!\n", total, acusado);
        } else if (total == 1) {
            printf("⚠️ Apenas 1 pista aponta para %s. Provas insuficientes para condenar.\n", acusado);
        } else {
            printf("❌ Nenhuma pista coletada aponta para %s. Acusação infundada.\n", acusado);
        }
    }

    /* --- Limpeza de memória --- */
    liberarSalas(hall);
    liberarPistas(raizPistas);
    liberarHash(hash);

    printf("\nObrigado por jogar Detective Quest — Capítulo Final.\n");
    return 0;
}
