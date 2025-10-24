/*
  ============================================
       DETECTIVE QUEST — Coleta de Pistas
  ============================================
  Autor: Paulo Ferreira
  Descrição:
    Expande o sistema da mansão (árvore binária)
    para permitir coleta e organização de pistas
    em uma árvore BST, exibidas em ordem alfabética.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------- Estruturas -------- */
typedef struct sala {
    char nome[50];
    char pista[100];
    struct sala *esquerda;
    struct sala *direita;
} Sala;

typedef struct pistaNode {
    char pista[100];
    struct pistaNode *esquerda;
    struct pistaNode *direita;
} PistaNode;

/* -------- Protótipos -------- */
Sala* criarSala(const char *nome, const char *pista);
PistaNode* criarPistaNode(const char *pista);
PistaNode* inserirPista(PistaNode *raiz, const char *pista);
void explorarSalasComPistas(Sala *atual, PistaNode **raizPistas);
void exibirPistas(PistaNode *raiz);
void liberarSalas(Sala *raiz);
void liberarPistas(PistaNode *raiz);

/* -------- Função principal -------- */
int main() {
    // Criação do mapa da mansão (árvore binária)
    Sala *hall = criarSala("Hall de Entrada", "Pegada de lama");
    Sala *salaEstar = criarSala("Sala de Estar", "Lenço rasgado");
    Sala *cozinha = criarSala("Cozinha", "Copo quebrado");
    Sala *biblioteca = criarSala("Biblioteca", "Diário antigo");
    Sala *jardim = criarSala("Jardim", "Chave enferrujada");
    Sala *despensa = criarSala("Despensa", "Luvas sujas");
    Sala *garagem = criarSala("Garagem", "Pneu com marca estranha");

    // Ligações da árvore
    hall->esquerda = salaEstar;
    hall->direita = cozinha;
    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;
    cozinha->esquerda = despensa;
    cozinha->direita = garagem;

    // Árvore de pistas (BST)
    PistaNode *raizPistas = NULL;

    // Início da exploração
    printf("🕵️‍♂️  Bem-vindo(a) ao Detective Quest: Coleta de Pistas!\n");
    printf("-------------------------------------------------------\n");
    explorarSalasComPistas(hall, &raizPistas);

    // Exibição final das pistas
    printf("\n📜 Pistas coletadas (em ordem alfabética):\n");
    exibirPistas(raizPistas);

    // Liberação da memória
    liberarSalas(hall);
    liberarPistas(raizPistas);

    printf("\n🕵️  Obrigado por jogar Detective Quest!\n");
    return 0;
}

/* -------- Cria dinamicamente uma sala -------- */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro de alocação de memória.\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

/* -------- Cria um nó de pista -------- */
PistaNode* criarPistaNode(const char *pista) {
    PistaNode *novo = (PistaNode*) malloc(sizeof(PistaNode));
    if (!novo) {
        printf("Erro ao alocar memória para pista.\n");
        exit(1);
    }
    strcpy(novo->pista, pista);
    novo->esquerda = NULL;
    novo->direita = NULL;
    return novo;
}

/* -------- Insere uma pista na árvore BST -------- */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) return criarPistaNode(pista);
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);
    return raiz;
}

/* -------- Exploração interativa -------- */
void explorarSalasComPistas(Sala *atual, PistaNode **raizPistas) {
    char opcao;
    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Coleta automática da pista
        if (strlen(atual->pista) > 0) {
            printf("🔎 Pista encontrada: \"%s\"\n", atual->pista);
            *raizPistas = inserirPista(*raizPistas, atual->pista);
        } else {
            printf("Nenhuma pista neste cômodo.\n");
        }

        printf("\nEscolha seu próximo passo:\n");
        if (atual->esquerda) printf("(e) Esquerda -> %s\n", atual->esquerda->nome);
        if (atual->direita) printf("(d) Direita  -> %s\n", atual->direita->nome);
        printf("(s) Sair da mansão\n> ");
        scanf(" %c", &opcao);

        if (opcao == 'e' || opcao == 'E') {
            if (atual->esquerda)
                atual = atual->esquerda;
            else
                printf("⚠️  Caminho inexistente à esquerda!\n");
        } 
        else if (opcao == 'd' || opcao == 'D') {
            if (atual->direita)
                atual = atual->direita;
            else
                printf("⚠️  Caminho inexistente à direita!\n");
        } 
        else if (opcao == 's' || opcao == 'S') {
            printf("\nVocê decidiu encerrar a exploração.\n");
            return;
        } 
        else {
            printf("⚠️  Opção inválida. Use 'e', 'd' ou 's'.\n");
        }
    }
}

/* -------- Exibe pistas em ordem alfabética (in-order) -------- */
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    exibirPistas(raiz->esquerda);
    printf(" - %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}

/* -------- Libera memória das árvores -------- */
void liberarSalas(Sala *raiz) {
    if (raiz == NULL) return;
    liberarSalas(raiz->esquerda);
    liberarSalas(raiz->direita);
    free(raiz);
}

void liberarPistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}
