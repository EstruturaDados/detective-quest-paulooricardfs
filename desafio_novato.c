/*
  ============================================
       DETECTIVE QUEST — Mapa da Mansão
  ============================================
  Autor: Paulo Ferreira
  Descrição:
    Simula a exploração de uma mansão representada
    por uma árvore binária. O jogador pode escolher
    caminhos à esquerda ou direita até encontrar
    um cômodo final.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------- Estrutura da Sala (nó da árvore) -------- */
typedef struct sala {
    char nome[50];
    struct sala *esquerda;
    struct sala *direita;
} Sala;

/* -------- Protótipos -------- */
Sala* criarSala(const char *nome);
void explorarSalas(Sala *atual);
void liberarSalas(Sala *raiz);

/* -------- Função principal -------- */
int main() {
    // Montagem manual do mapa da mansão
    Sala *hall = criarSala("Hall de Entrada");
    Sala *salaEstar = criarSala("Sala de Estar");
    Sala *cozinha = criarSala("Cozinha");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *jardim = criarSala("Jardim");
    Sala *despensa = criarSala("Despensa");
    Sala *garagem = criarSala("Garagem");

    // Conexões entre as salas (estrutura da árvore)
    hall->esquerda = salaEstar;
    hall->direita = cozinha;

    salaEstar->esquerda = biblioteca;
    salaEstar->direita = jardim;

    cozinha->esquerda = despensa;
    cozinha->direita = garagem;

    // Início da exploração
    printf("🔎 Bem-vindo(a) à mansão ENIGMA!\n");
    printf("Sua missão: explorar os cômodos e encontrar o caminho até o fim.\n");
    printf("---------------------------------------------------------------\n");
    
    explorarSalas(hall);

    // Libera a memória alocada
    liberarSalas(hall);

    printf("\n🕵️  Obrigado por jogar Detective Quest!\n");
    return 0;
}

/* -------- Criação dinâmica de uma sala -------- */
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*) malloc(sizeof(Sala));
    if (!nova) {
        printf("Erro de alocação de memória.\n");
        exit(1);
    }
    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

/* -------- Função de exploração interativa -------- */
void explorarSalas(Sala *atual) {
    char opcao;
    while (atual != NULL) {
        printf("\nVocê está no: %s\n", atual->nome);

        // Caso seja uma sala sem caminhos (nó folha)
        if (atual->esquerda == NULL && atual->direita == NULL) {
            printf("Nenhum caminho disponível. Fim da exploração!\n");
            return;
        }

        printf("Escolha um caminho:\n");
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
            printf("\nVocê decidiu sair da mansão.\n");
            return;
        } 
        else {
            printf("⚠️  Opção inválida. Use 'e', 'd' ou 's'.\n");
        }
    }
}

/* -------- Função recursiva para liberar memória -------- */
void liberarSalas(Sala *raiz) {
    if (raiz == NULL) return;
    liberarSalas(raiz->esquerda);
    liberarSalas(raiz->direita);
    free(raiz);
}
