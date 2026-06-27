// TAD de Filas (FIFO e Round Robin com Prioridade) e Listas, etc...

// Estruturas

struct pf {
	
	int pidf;
	struct pf *prox;
	
}; typedef struct pf ProcF;

struct processo {
	
	int pid; // identificador do processo
	int pai; // pid do pai
	int prioridade; // prioridade do processo em fila de prontos
	int tempo_cpu; // tempo necessario que precisa ficar na cpu para terminar (CPU Burst)
	int tempo_exe; // tempo total de execucao desde quando é criado até quando é terminado
	
	int inicio_bloq; // momento em que foi bloqueado
	int fim_bloq; // momento em que foi desloqueado (quando for desbloqueado, já faz a conta do tempo de bloqueio e zera os registradores de inicio e fim)
	int tempo_bloq; // tempo_bloq = inicio_bloq - fim_bloq
	int tempo_bloqF; // tempo_bloq apenas na *lista de espera por filhos
	
	int inicio_esp; // momento em que entrou em espera (sempre q entra em execucao, deve volta a 0, para se ocorrer uma interrupção na execução)
	int fim_esp; // momento em q saiu de espera ou entrou em outra fila de espera
	int tempo_esp; // tempo de espera total (tempo_esp = entrada - tempo (no momento que começa a execução))
	
	char status[10]; // novo, pronto, bloqueado, rodando, terminado || new, ready, waiting, running, terminated
	
	char foiBloq;
	char pertenceItem3;
	
	int nFilhos; // número/quantidade de filhos criados
	ProcF *filhos; // lista com os pids dos filhos
	
	struct processo *prox; // aponta para o proximo da fila
	
}; typedef struct processo Proc;

struct fifo {
	char okDequeue; // booleano que diz se pode ou nao retirar um processo da fila, seria oq indica se o evento esperado saiu ou ñ
	Proc *inicio, *fim;
}; typedef struct fifo Fila;

// Assinaturas

ProcF *criarFilhos(int pidf);
Proc *criarProc(int pid, int pid_pai);
Fila* criarFila(void);
void init(Fila **f);
char vazia(Fila *f);
Proc *top(Fila *f);
void enqueue(Fila *f, Proc *p);
Proc *dequeue(Fila *f);
void enqueueC(Fila *f, Proc *p);
Proc *dequeueC(Fila *f);
void exibir(Fila *f);
void exibirFinal(Fila *f);
void exibirC(Fila *f);
void exibirComFilhos(Fila *f);
void exibirPertencentesItem3(Fila *f);
char vaziaFilhos(Proc *pai);
void inserirFilhos(Proc *pai, int pidf);
char buscarFilhos(Fila *terminados, int pf);
char terminadosFilhos(Proc *pai, Fila *terminados);
Proc *buscarPaiPronto(Fila *listaPais, Fila *terminados);
Proc *removerProcesso(Fila *f, int pidParaRemover);
char isFilho(Proc *pai, int potencialFilho);
int buscaUltimoFilho(Proc *pai, Fila *terminados);
void destroyFilhos(Proc *pai);
void destroy(Fila **f);

// Funções

ProcF *criarFilhos(int pidf) {
	
	ProcF *f = (ProcF*)malloc(sizeof(ProcF));
	f->pidf = pidf;
	f->prox = NULL;
	return f;
	
}

Proc *criarProc(int pid, int pid_pai) {
	
	Proc *n = (Proc*)malloc(sizeof(Proc));
	n->pid = pid;
	n->pai = pid_pai;
	strcpy(n->status, "new");
	n->prioridade = (rand() % 5) + 1; // 1 - 5
	n->tempo_cpu = (rand() % 20) + 1; // CPU Burst de 1 a 20 unidades de tempo
	n->nFilhos = 0;
	n->tempo_exe = n->tempo_cpu;
	n->inicio_bloq = n->fim_bloq = n->tempo_bloq = 0;
	n->tempo_bloqF=0;
	n->inicio_esp = n->fim_esp = n->tempo_esp = 0;
	n->prox = NULL;
	n->filhos = NULL;
	
	n->foiBloq=0;
	if(n->tempo_cpu > 10)
		n->pertenceItem3 = 1; // se o tempo de cpu for maior q o quantum (significa q ele vai executar mais de uma vez) ele é um potencial pertencente ao item 3 dos relatórios
	else n->pertenceItem3 = 0;
	
	return n;
	
}

Fila* criarFila(void) {
	
    Fila *f = (Fila*) malloc(sizeof(Fila));
    f->okDequeue=0;
    f->inicio = f->fim = NULL;
    return f;
    
}

void init(Fila **f) {
	*f = criarFila();
}

char vazia(Fila *f) {
	return f->inicio == NULL;
}

Proc *top(Fila *f) {
	
	if(vazia(f)) return NULL;
	
	return f->inicio;
	
}

void enqueue(Fila *f, Proc *p) {
    if(p == NULL) return;

	if(vazia(f)) {
		
		f->inicio = p;
	} else {
		
		f->fim->prox = p;
	}
	f->fim = p;
}

Proc *dequeue(Fila *f) {
    if(vazia(f)) return NULL;

	Proc *ret = f->inicio;
	f->inicio = f->inicio->prox;

	if(f->inicio == NULL) {
		
		f->fim = NULL;
	}
	
    ret->prox = NULL;
	return ret;
}


// --- LÓGICA PARA FILA ESTRUTURALMENTE CIRCULAR ---

void enqueueC(Fila *f, Proc *p) {
	
    if(p == NULL) return;
    p->prox = NULL;

    // CASO 1: A fila está vazia.
    if(vazia(f)) {
        f->inicio = p;
        f->fim = p;
    }
    // CASO 2: A fila NÃO está vazia e o novo processo tem prioridade MAIOR que o primeiro da fila.
    else if(p->prioridade < f->inicio->prioridade) {
        p->prox = f->inicio;
        f->inicio = p;
    }
    // CASO 3: A fila NÃO está vazia e o processo deve ser inserido no meio ou no fim.
    else {
        // Percorre a fila para achar o lugar certo.
        Proc *ant = f->inicio;
        Proc *atual = f->inicio->prox;

        // Procura a posição correta para inserção.
        while(atual != NULL && p->prioridade >= atual->prioridade) {
            ant = atual;
            atual = atual->prox;
        }
        ant->prox = p;
        p->prox = atual;
        if (atual == NULL) f->fim = p;
    }
}

Proc *dequeueC(Fila *f) {
    if(vazia(f)) return NULL;

    Proc *ret = f->inicio;
    f->inicio = f->inicio->prox;

    if(f->inicio == NULL) {
        f->fim = NULL;
    }

    ret->prox = NULL;
    return ret;
}

void exibir(Fila *f) {
	Proc *auxP = f->inicio;
    while(auxP != NULL) {
    	
        printf("\tProcesso %d, Status: %s, Pai: %d, CPU Burst: %d.\n", auxP->pid, auxP->status, auxP->pai, auxP->tempo_cpu);
        auxP = auxP->prox;
    }
}

void exibirFinal(Fila *f) {
	Proc *auxP = f->inicio;
    while(auxP != NULL) {
    	
        printf("\tProcesso %d, Status: %s, Pai: %d, Tempo Total de Execucao: %d.\n", auxP->pid, auxP->status, auxP->pai, auxP->tempo_exe);
        auxP = auxP->prox;
    }
}

void exibirC(Fila *f) {
	Proc *auxP = f->inicio;
    while(auxP != NULL) {
        printf("\tProcesso %d, Status: %s, Prioridade: %d, Pai: %d, CPU Burst: %d.\n", auxP->pid, auxP->status, auxP->prioridade, auxP->pai, auxP->tempo_cpu);
        auxP = auxP->prox;
    }
}

void exibirComFilhos(Fila *f) {
	char flag=0;
	Proc *auxP = f->inicio;
    while(auxP != NULL) {
    	
        if(auxP->nFilhos > 0) {
        	
			printf("\tProcesso %d, Status: %s, Numero de Filhos: %d, Tempo de Bloqueio por Espera de Filho(s): %d.\n", auxP->pid, auxP->status, auxP->nFilhos, auxP->tempo_bloqF);
			flag=1;
        }
		auxP = auxP->prox;
    }
	if(!flag) printf("\tNenhum processo criou filho(s).\n");
}

void exibirPertencentesItem3(Fila *f) {
	
	char flag=0;
	Proc *auxP = f->inicio;
	while(auxP != NULL) {
		
		if(auxP->pertenceItem3) {
			
			printf("\tProcesso %d, Status: %s, Pai: %d, Tempo Total de Execucao: %d.\n", auxP->pid, auxP->status, auxP->pai, auxP->tempo_exe);
			flag=1;
		}
		auxP = auxP->prox;
	}
	if(!flag) printf("\tNao ha nenhum processo com essa caracteristica.\n");
}

char vaziaFilhos(Proc *pai) {
	return pai->filhos == NULL;
}

void inserirFilhos(Proc *pai, int pidf) {
    ProcF *novoFilho = criarFilhos(pidf);

    if (pai->filhos == NULL) {
        pai->filhos = novoFilho;
    } else {
        ProcF *temp = pai->filhos;
        while(temp->prox != NULL) {
            temp = temp->prox;
        }
        temp->prox = novoFilho;
    }
}

char buscarFilhos(Fila *terminados, int pf) {
	
    Proc *atual = terminados->inicio;
    char encontrado = 0;

    while(atual != NULL && encontrado == 0) { 
        if (atual->pid == pf) {
            encontrado = 1;
        }
        atual = atual->prox;
    }
    return encontrado;
}

char terminadosFilhos(Proc *pai, Fila *terminados) { // pergunta se todos os filhos já terminaram

	if(vaziaFilhos(pai)) {
        return 1;
    }

    ProcF *filhoAtual = pai->filhos;
    char todosTerminaram = 1;
    
    while(filhoAtual != NULL && todosTerminaram == 1) { 
        if(buscarFilhos(terminados, filhoAtual->pidf) == 0) todosTerminaram = 0;
        filhoAtual = filhoAtual->prox;
    }

    return todosTerminaram;
}

char terminadosPais(Fila *listaPais, Fila *terminados) {
    if(vazia(listaPais)) return 0;

    Proc *paiAtual = listaPais->inicio;
    char encontrouPaiPronto = 0;
    while(paiAtual != NULL && encontrouPaiPronto == 0) {
    	
        if(terminadosFilhos(paiAtual, terminados) == 1) encontrouPaiPronto = 1;
        paiAtual = paiAtual->prox;
    } return encontrouPaiPronto;
}

Proc *buscarPaiPronto(Fila *listaPais, Fila *terminados) {
    if(vazia(listaPais)) return NULL;

    Proc *paiAtual = listaPais->inicio;
    Proc *paiEncontrado = NULL;

    while(paiAtual != NULL && paiEncontrado == NULL) {
        
        if(terminadosFilhos(paiAtual, terminados) == 1) paiEncontrado = paiAtual;
        paiAtual = paiAtual->prox;
    } return paiEncontrado;
}

Proc *removerProcesso(Fila *f, int pidParaRemover) {
    if(vazia(f) || pidParaRemover == 0) return NULL;

    Proc *anterior = NULL;
    Proc *atual = f->inicio;
    while(atual != NULL && atual->pid != pidParaRemover) {
        anterior = atual;
        atual = atual->prox;
    }
    
    if(atual == NULL) return NULL;
    if(anterior == NULL) f->inicio = atual->prox;
    else anterior->prox = atual->prox;
    if(atual == f->fim) f->fim = anterior;

    atual->prox = NULL;
    return atual;
}

char isFilho(Proc *pai, int potencialFilho) { // pergunta se o filho é dele
	
	char flag=0; // inicialmente ele diz q o filho não é dele, precisa de análise/ verificação
	ProcF *filhos = pai->filhos;
	while(filhos != NULL && !flag) { // enquanto puder buscar e não achar o filho
		
		if(filhos->pidf == potencialFilho) flag=1; // pergunta se o filho é um dos seus, se for fala q o filho é dele
		filhos = filhos->prox;
	}
	return flag;
}

int buscaUltimoFilho(Proc *pai, Fila *terminados) { // busca o último filho terminado de um processo e retorna quando ele terminou (momento)
	
	int momentoFinal=0;
	Proc *aux = terminados->inicio; // começa no inicio da fila de terminados
	while(aux != NULL) {
		if(isFilho(pai, aux->pid)) { // se o elemento atual é filho do pai recebido por parametro pela função
			momentoFinal = aux->fim_esp; // salva o momento em que o tempo terminou, assim dps se pode somar quando o pai entrou em espera dos filhos e quando o seu último filho terminou
		} // a ideia dessa repetição é buscar o momento em que o último filho terminou, ou seja, quando o pai pôde sair da listaPais (lista de espera dos pais pelos filhos) e ir para terminados
		aux = aux->prox;
	}
	return momentoFinal;
}

void destroyFilhos(Proc *pai) {
	
    ProcF *atual = pai->filhos;
    ProcF *aux;
    while(atual != NULL) {
    	
        aux = atual->prox;
        free(atual);
        atual = aux;
    }
    pai->filhos = NULL;
}

void destroy(Fila **f) {
    if(f == NULL || *f == NULL) return;

    Proc *fila_inicio = (*f)->inicio;
    Proc *fila_fim = (*f)->fim;

    if(fila_fim != NULL) {
        fila_fim->prox = NULL;
    }

    Proc *atual = fila_inicio;
    Proc *aux;
    while(atual != NULL) {
        aux = atual->prox;
        destroyFilhos(atual); 
        free(atual);
        atual = aux;
    }
    free(*f);
    *f = NULL;
}

