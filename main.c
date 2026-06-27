#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <windows.h>
#include <math.h>

#include "TAD_filas.h"

#define QUANTUM_BASE 10

int main(void) {
	
	int i, j, k, x, y, z, quantum=0;
	int tempo=0, tempoMax, procTerminados=0, procBloqueados=0, somaTempBloq=0;
	int pidPC=2; // Contador de Programas, aqui guarda os pids que usaremos ao criar processos novos (lembrar smp de incrementar ao criar um processo
	// PC começa de 2 pois o processo 1 é a própria simulação
	
	Proc *auxP;
	Fila *auxF;
	Proc *cpu=NULL;
	Fila *prontos, *bloqHD, *bloqMouse, *bloqTeclado, *bloqEvent, *terminados, *listaPais;
	init(&prontos); init(&bloqHD); init(&bloqMouse); init(&bloqTeclado); init(&bloqEvent); init(&terminados); init(&listaPais);
	prontos->okDequeue = 1; // smp será 1 na fila de prontos
	
	do {
		system("cls");
		printf("Desenvolvedores:\n");
		printf("Caua Toninato\n");
		printf("Gustavo Imada\n");
		printf("Luiz Carlos\n");
		printf("Raul Santos\n");
		printf("\nPara iniciar a simulacao, informe o tempo de execucao: ");
		scanf("%d", &tempo);
		if(tempo < 1) { printf("\nInválido. Apresente um valor maior que 0 (zero), ([ENTER] p/ continuar): "); getch(); }
	} while(tempo < 1);
	tempoMax = tempo;
	
	printf("Execucao comecou, simulacao iniciada [Processo 1]. [ENTER] p/ continuar: "); getch(); printf("\n");
	
	do {
		
		do {
		
			for(z=1; z<=80; z++) printf("-"); printf("\n");
			
			printf("Tempo restante: %d / %d\n", tempo, tempoMax);
			
			if(cpu == NULL) { // aq se pode colocar um processo na cpu que se encontra nula
			
				if(!vazia(prontos)) cpu = dequeueC(prontos);
				if(cpu != NULL) {
					
					strcpy(cpu->status, "running");
					cpu->fim_esp = tempo; // ao começar a executar, define o fim do tempo de espera, calcula, e zera o inicio e fim, para se caso entrar novamente em espera
					cpu->tempo_esp += cpu->inicio_esp - cpu->fim_esp;
					cpu->inicio_esp = cpu->fim_esp = 0;
					printf("CPU: Executando Processo %d, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
					quantum=QUANTUM_BASE;
					printf("Quantum: %d / %d\n", quantum, QUANTUM_BASE);
					
				} else {
					printf("CPU: Vazia.\n");
					
					printf("\nPossiveis Eventos:\n\n");
				}
				
			} else {
					
				// aq o processo e o quantum tem seus valores decrementados e exibidos
				cpu->tempo_cpu--;
				quantum--;
				printf("CPU: Executando Processo %d, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
				printf("Quantum: %d / %d\n", quantum, QUANTUM_BASE);
				
				printf("\nPossiveis Eventos:\n\n");
				
				// verifica se o quantum ou processo acabaram
				if(quantum == 0 && cpu->tempo_cpu > 0) { // se o processo nao terminou e o quantum acabou
				
					strcpy(cpu->status, "ready");
					cpu->inicio_esp = tempo; // ao entrar em ready define o momento inicial desse tempo de espera
					printf("Processo %d, cpu p/ prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
					enqueueC(prontos, cpu);
					cpu=NULL;
					quantum=QUANTUM_BASE;
				} else if(cpu->tempo_cpu == 0) { // se o processo terminou, e o quantum acabou ou nao
				
					strcpy(cpu->status, "terminated");
					if(terminadosFilhos(cpu, terminados)) { // se não possui filhos ou todos os filhos estao em terminados, pode ir para terminados
						
						cpu->tempo_exe += cpu->tempo_esp; // soma o tempo de espera com o tempo de cpu (que já foi somado ao ser criado no TAD
						printf("Processo %d, cpu p/ terminados, Status: %s, Pai: %d, Tempo de Execucao: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_exe);
						enqueue(terminados, cpu);
						cpu=NULL;
						quantum=QUANTUM_BASE;
						procTerminados++;
					} else { // se possui filhos, vai para espera dos filhos até todos terminarem, quando todos estiverem em terminados, pode entrar em terminados
						
						if(!cpu->foiBloq) procBloqueados++;
						cpu->foiBloq=1;
						cpu->pertenceItem3 = 0;
						
						printf("Processo %d, em espera dos filhos, Status: %s, Pai: %d.\n", cpu->pid, cpu->status, cpu->pai);
						strcpy(cpu->status, "waiting");
						cpu->inicio_bloq = tempo; // ao entrar em waiting define o momento inicial desse tempo de bloqueado
						cpu->inicio_esp = tempo; // ao entrar em waiting define o momento inicial desse tempo de espera
						enqueue(listaPais, cpu);
						cpu=NULL;
					}
				}
				
				if(cpu != NULL) {
					
					// aq ele tem a chance de esperar um evento (disco, mouse, teclado, etc...)
					if(rand() % 9 > 4) { // chance de entrar aq (ser bloqueado / entrar em waiting)
						
						if(!cpu->foiBloq) procBloqueados++;
						cpu->foiBloq=1;
						cpu->pertenceItem3 = 0;
					
						x = rand() % 4;
						switch(x) {
							case 0: // entra na fila de espera por HD/Disco
								printf("Processo %d, cpu p/ disco, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
								strcpy(cpu->status, "waiting");
								cpu->inicio_bloq = tempo; // ao entrar em waiting define o momento inicial desse tempo de bloqueado
								cpu->inicio_esp = tempo; // ao entrar em waiting define o momento inicial desse tempo de espera
								enqueue(bloqHD, cpu);
								cpu = NULL;
								break;
							case 1: // entra na fila de espera por evento do mouse
								printf("Processo %d, cpu p/ mouse, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
								strcpy(cpu->status, "waiting");
								cpu->inicio_bloq = tempo;
								cpu->inicio_esp = tempo;
								enqueue(bloqMouse, cpu);
								cpu = NULL;
								break;
							case 2: // entra na fila de espera por evento do teclado
								printf("Processo %d, cpu p/ teclado, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
								strcpy(cpu->status, "waiting");
								cpu->inicio_bloq = tempo;
								cpu->inicio_esp = tempo;
								enqueue(bloqTeclado, cpu);
								cpu = NULL;
								break;
							case 3: // entra na fila de espera por evento
								printf("Processo %d, cpu p/ evento, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
								strcpy(cpu->status, "waiting");
								cpu->inicio_bloq = tempo;
								cpu->inicio_esp = tempo;
								enqueue(bloqEvent, cpu);
								cpu = NULL;
								break;
						}
					}
					
					// aq o processo tem a chance de criar um filho, e o processo entra em uma pilha de wait
					if(cpu != NULL && rand() % 4 > 2) {
						
						// tem q dar um jeito de ligar o pai e o filho para se o pai terminar ele esperar o filho em uma lista ou fila de espera de filhos, e se o filho terminar informar ao pai q ele morreu
						
						cpu->nFilhos++;
						Proc *filho = criarProc(pidPC, cpu->pid); // pid do pai é o pid do processo que está em execução
						pidPC++;
						inserirFilhos(cpu, filho->pid); // adiciona o filho em uma lista de filhos (ela informa se os filhos acabaram ou não
						printf("Processo filho %d criado pelo pai %d, Status: %s, CPU Burst: %d.\n", filho->pid, filho->pai, filho->status, filho->tempo_cpu);
						strcpy(filho->status, "ready");
						filho->inicio_esp = tempo;
						enqueueC(prontos, filho);                                                                                              
						printf("Processo %d, em fila de prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", filho->pid, filho->status, filho->pai, filho->tempo_cpu);
						
					}
				}
			}
			
			if(rand() % 4 > 1) { // aq a simulação cria um processo
				
				Proc *n = criarProc(pidPC, 1); // pid do pai é smp 0 aqui pois é a simulação q criou
				pidPC++;
				printf("Processo %d criado, Status: %s, Pai: %d, CPU Burst: %d.\n", n->pid, n->status, n->pai, n->tempo_cpu);
				strcpy(n->status, "ready");
				n->inicio_esp = tempo; // ao entrar em ready define o momento inicial desse tempo de espera
				enqueueC(prontos, n);
				printf("Processo %d, em fila de prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", n->pid, n->status, n->pai, n->tempo_cpu);
			}
			
			if(!vazia(listaPais)) { // aq verifica se algum pai pode ir para terminados (seus filhos todos terminaram)
				
				if(terminadosPais(listaPais, terminados)) { // se um ou mais pais tem todos os seus filhos em terminados, pode ser retirado e mandado para terminados tbm
					
					Proc* pai = NULL;
					// este loop continuará enquanto houver pais prontos para serem finalizados na lista
					while((pai = buscarPaiPronto(listaPais, terminados)) != NULL) {
					    
					    // encontramos um pai (paiPronto) que pode ser finalizado.
					    // primeiro, removemos ele da lista de espera.
					    removerProcesso(listaPais, pai->pid);
					
					    // agora que 'paiPronto' está "solto", fazemos os cálculos finais.
					    strcpy(pai->status, "terminated");
					
					    int tempoBloqFilhos = pai->inicio_bloq - tempo;
					    pai->tempo_bloq += tempoBloqFilhos;
					    pai->tempo_bloqF = tempoBloqFilhos; // Apenas o tempo na lista de pais
					    somaTempBloq += tempoBloqFilhos;
					    pai->inicio_bloq = 0;
					    
					    pai->tempo_esp += pai->inicio_esp - tempo;
					    pai->tempo_exe += pai->tempo_esp;
					    
					    // colocamos na fila de terminados.
					    printf("Processo %d terminado (filhos terminaram), Status: %s, Pai: %d, Tempo de Execucao: %d.\n", pai->pid, pai->status, pai->pai, pai->tempo_exe);
					    enqueue(terminados, pai);
					    procTerminados++;
					}
				}
			}
			
			if(!vazia(bloqHD)) { // se alguma fila de espera não estiver vazia, tem chance de liberar um processo
			
				if(bloqHD->okDequeue) {
					auxP = dequeue(bloqHD);
					
					auxP->fim_bloq = tempo; // ao sair de uma fila de espera/bloqueio, define o momento final do tempo de bloqueio e calcula o tempo de bloqueio, dps reseta o inicio e fim, para caso entre novamente em uma fila
					auxP->tempo_bloq += auxP->inicio_bloq - auxP->fim_bloq;
					somaTempBloq += auxP->inicio_bloq - auxP->fim_bloq;
					auxP->inicio_bloq = auxP->fim_bloq = 0;
					
					auxP->fim_esp = tempo; // o mesmo para tempo de espera
					auxP->tempo_esp += auxP->inicio_esp - auxP->fim_esp;
					auxP->inicio_esp = tempo;
					auxP->fim_esp = 0;
					
					printf("Processo %d, disco p/ prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", auxP->pid, auxP->status, auxP->pai, auxP->tempo_cpu);
					
					strcpy(auxP->status, "ready");
					enqueueC(prontos, auxP);
					bloqHD->okDequeue=0;
				} else if(rand() % 4 > 1) bloqHD->okDequeue=1;
				if(bloqHD->okDequeue) printf("Fila de Disco: Um processo pode ir p/ pronto.\n");
				
			}
			
			if(!vazia(bloqMouse)) {
				
				if(bloqMouse->okDequeue) {
					auxP = dequeue(bloqMouse);
					
					auxP->fim_bloq = tempo;
					auxP->tempo_bloq += auxP->inicio_bloq - auxP->fim_bloq;
					somaTempBloq += auxP->inicio_bloq - auxP->fim_bloq;
					auxP->inicio_bloq = auxP->fim_bloq = 0;
					
					auxP->fim_esp = tempo;
					auxP->tempo_esp += auxP->inicio_esp - auxP->fim_esp;
					auxP->inicio_esp = tempo;
					auxP->fim_esp = 0;
					
					printf("Processo %d, mouse p/ prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", auxP->pid, auxP->status, auxP->pai, auxP->tempo_cpu);
					
					strcpy(auxP->status, "ready");
					enqueueC(prontos, auxP);
					bloqMouse->okDequeue=0;
				} else if(rand() % 4 > 1) bloqMouse->okDequeue=1;
				if(bloqMouse->okDequeue) printf("Fila de Mouse: Um processo pode ir p/ pronto.\n");
				
			}
			
			if(!vazia(bloqTeclado)) {
				
				if(bloqTeclado->okDequeue) {
					auxP = dequeue(bloqTeclado);
					
					auxP->fim_bloq = tempo;
					auxP->tempo_bloq += auxP->inicio_bloq - auxP->fim_bloq;
					somaTempBloq += auxP->inicio_bloq - auxP->fim_bloq;
					auxP->inicio_bloq = auxP->fim_bloq = 0;
					
					auxP->fim_esp = tempo;
					auxP->tempo_esp += auxP->inicio_esp - auxP->fim_esp;
					auxP->inicio_esp = tempo;
					auxP->fim_esp = 0;
					
					printf("Processo %d, teclado p/ prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", auxP->pid, auxP->status, auxP->pai, auxP->tempo_cpu);
					
					strcpy(auxP->status, "ready");
					enqueueC(prontos, auxP);
					bloqTeclado->okDequeue=0;
				} else if(rand() % 4 > 1) bloqTeclado->okDequeue=1;
				if(bloqTeclado->okDequeue) printf("Fila de Teclado: Um processo pode ir p/ pronto.\n");
				
			}
			
			if(!vazia(bloqEvent)) {
				
				if(bloqEvent->okDequeue) {
					auxP = dequeue(bloqEvent);
					
					auxP->fim_bloq = tempo;
					auxP->tempo_bloq += auxP->inicio_bloq - auxP->fim_bloq;
					somaTempBloq += auxP->inicio_bloq - auxP->fim_bloq;
					auxP->inicio_bloq = auxP->fim_bloq = 0;
					
					auxP->fim_esp = tempo;
					auxP->tempo_esp += auxP->inicio_esp - auxP->fim_esp;
					auxP->inicio_esp = tempo;
					auxP->fim_esp = 0;
					
					printf("Processo %d, evento p/ prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", auxP->pid, auxP->status, auxP->pai, auxP->tempo_cpu);
					
					strcpy(auxP->status, "ready");
					enqueueC(prontos, auxP);
					bloqEvent->okDequeue=0;
				} else if(rand() % 4 > 1) bloqEvent->okDequeue=1;
				if(bloqEvent->okDequeue) printf("Fila de Eventos: Um processo pode ir p/ pronto.\n");
				
			}
			
			// exibições das filas
			printf("\n\nExibicoes das Filas:\n");
			
			printf("\nFila de Prontos:\n");
			if(vazia(prontos)) printf("\tVazia.\n");
			else exibirC(prontos);
			
			printf("\nFila de Espera HD:\n");
			if(vazia(bloqHD)) printf("\tVazia.\n");
			else exibir(bloqHD);
			
			printf("\nFila de Espera Mouse:\n");
			if(vazia(bloqMouse)) printf("\tVazia.\n");
			else exibir(bloqMouse);
			
			printf("\nFila de Espera Teclado:\n");
			if(vazia(bloqTeclado)) printf("\tVazia.\n");
			else exibir(bloqTeclado);
			
			printf("\nFila de Espera Evento:\n");
			if(vazia(bloqEvent)) printf("\tVazia.\n");
			else exibir(bloqEvent);
			
			printf("\nFila de Espera de Filhos:\n");
			if(vazia(listaPais)) printf("\tVazia.\n");
			else exibir(listaPais);
			
			tempo--;
			Sleep(2000);
			
		} while(!kbhit() && tempo > 0);
		
		// aq o usuario pode criar um processo
		if(tempo > 0) {
			
			char o = ' ';
			while(o != 'S' && o != 'N') {
			
				printf("\n\nDeseja inserir o processo %d ? <S/N> : ", pidPC);
				getch();
				o = toupper(getch());
				
				if(o == 'S') {
					
					Proc *n = criarProc(pidPC, 1); // pid do pai é smp 0 aqui pois é a simulação q criou
					pidPC++;
					printf("\n\nProcesso %d criado, Status: %s, Pai: %d, CPU Burst: %d, P=%d.\n", n->pid, n->status, n->pai, n->tempo_cpu, n->prioridade);
					strcpy(n->status, "ready");
					n->inicio_esp = tempo; // ao entrar em ready define o momento inicial desse tempo de espera
					enqueueC(prontos, n);
					printf("Processo %d, em fila de prontos, Status: %s, Pai: %d, CPU Burst: %d, P=%d.\n", n->pid, n->status, n->pai, n->tempo_cpu, n->prioridade);
					printf("[ENTER] p/ continuar: "); getch(); printf("\n");
				} else if(o == 'N') {
					
					printf("\n\nProcesso nao foi criado.\n");
					getch();
					printf("\n");
				} else {
					
					printf("\n\nInvalido. Deseja inserir o processo %d ? <S/N> : ", pidPC);
					o = toupper(getch());
				}
			}
		}
	} while(tempo > 0);
	
	printf("\n");
	for(z=1; z<=80; z++) printf("#"); printf("\n");
	
	// tem q terminar o processo q ficou na cpu, e os processos q ficaram nas filas, sendo que todos os programas que terminam ficam na fila de terminados
	
	tempo=0;
	while(!vazia(prontos) || !vazia(bloqHD) || !vazia(bloqTeclado) || !vazia(bloqMouse) || !vazia(bloqEvent) || !vazia(listaPais) || cpu != NULL) {
		
		printf("\n");
		for(z=1; z<=80; z++) printf("-"); printf("\n");
		
		printf("\nTempo extra: %d\n", (-tempo)+1);
		
		if(cpu == NULL) {
			
			if(!vazia(prontos)) cpu = dequeueC(prontos);
			if(cpu != NULL) {
				
				strcpy(cpu->status, "running");
				cpu->fim_esp = tempo; // ao começar a executar, define o fim do tempo de espera, calcula, e zera o inicio e fim, para se caso entrar novamente em espera
				cpu->tempo_esp += cpu->inicio_esp - cpu->fim_esp;
				cpu->inicio_esp = cpu->fim_esp = 0;
				printf("CPU: Executando Processo %d, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
				quantum=QUANTUM_BASE;
				printf("Quantum: %d / %d\n", quantum, QUANTUM_BASE);
			} else {
				printf("CPU: Vazia.\n");
				printf("\nPossiveis Eventos:\n\n");
			}
			
		} else {
			
			// aq o processo e o quantum tem seus valores decrementados e exibidos
			cpu->tempo_cpu--;
			quantum--;
			printf("CPU: Executando Processo %d, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
			printf("Quantum: %d / %d\n", quantum, QUANTUM_BASE);
			
			printf("\nPossiveis Eventos:\n\n");
			
			// verifica se o quantum ou processo acabaram
			if(quantum == 0 && cpu->tempo_cpu > 0) { // se o processo nao terminou e o quantum acabou
			
				strcpy(cpu->status, "ready");
				cpu->inicio_esp = tempo; // ao entrar em ready define o momento inicial desse tempo de espera
				printf("Processo %d, cpu p/ prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
				enqueueC(prontos, cpu);
				cpu=NULL;
				quantum=QUANTUM_BASE;
			} else if(cpu->tempo_cpu == 0) { // se o processo terminou, e o quantum acabou ou nao
			
				strcpy(cpu->status, "terminated");
				if(terminadosFilhos(cpu, terminados)) { // se não possui filhos ou todos os filhos estao em terminados, pode ir para terminados
					
					cpu->tempo_exe += cpu->tempo_esp; // soma o tempo de espera com o tempo de cpu (que já foi somado ao ser criado no TAD
					printf("Processo %d, cpu p/ terminados, Status: %s, Pai: %d, Tempo de Execucao: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_exe);
					enqueue(terminados, cpu);
					cpu=NULL;
					quantum=QUANTUM_BASE;
					procTerminados++;
				} else { // se possui filhos, vai para espera dos filhos até todos terminarem, quando todos estiverem em terminados, pode entrar em terminados
					
					if(!cpu->foiBloq) procBloqueados++;
					cpu->foiBloq=1;
					cpu->pertenceItem3 = 0;
					
					printf("Processo %d, em espera dos filhos, Status: %s, Pai: %d.\n", cpu->pid, cpu->status, cpu->pai);
					strcpy(cpu->status, "waiting");
					cpu->inicio_bloq = tempo; // ao entrar em waiting define o momento inicial desse tempo de bloqueado
					cpu->inicio_esp = tempo; // ao entrar em waiting define o momento inicial desse tempo de espera
					enqueue(listaPais, cpu);
					cpu=NULL;
				}
			}
			
			if(cpu != NULL) {
				
				// aq ele tem a chance de esperar um evento (disco, mouse, teclado, etc...)
				if(rand() % 9 > 4) { // chance de entrar aq (ser bloqueado / entrar em waiting)
					
					if(!cpu->foiBloq) procBloqueados++;
					cpu->foiBloq=1;
					cpu->pertenceItem3 = 0;
				
					x = rand() % 4;
					switch(x) {
						case 0: // entra na fila de espera por HD/Disco
							printf("Processo %d, cpu p/ disco, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
							strcpy(cpu->status, "waiting");
							cpu->inicio_bloq = tempo; // ao entrar em waiting define o momento inicial desse tempo de bloqueado
							cpu->inicio_esp = tempo; // ao entrar em waiting define o momento inicial desse tempo de espera
							enqueue(bloqHD, cpu);
							cpu = NULL;
							break;
						case 1: // entra na fila de espera por evento do mouse
							printf("Processo %d, cpu p/ mouse, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
							strcpy(cpu->status, "waiting");
							cpu->inicio_bloq = tempo;
							cpu->inicio_esp = tempo;
							enqueue(bloqMouse, cpu);
							cpu = NULL;
							break;
						case 2: // entra na fila de espera por evento do teclado
							printf("Processo %d, cpu p/ teclado, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
							strcpy(cpu->status, "waiting");
							cpu->inicio_bloq = tempo;
							cpu->inicio_esp = tempo;
							enqueue(bloqTeclado, cpu);
							cpu = NULL;
							break;
						case 3: // entra na fila de espera por evento
							printf("Processo %d, cpu p/ evento, Status: %s, Pai: %d, CPU Burst: %d.\n", cpu->pid, cpu->status, cpu->pai, cpu->tempo_cpu);
							strcpy(cpu->status, "waiting");
							cpu->inicio_bloq = tempo;
							cpu->inicio_esp = tempo;
							enqueue(bloqEvent, cpu);
							cpu = NULL;
							break;
					}
				}
				
				// aq o processo tem a chance de criar um filho, e o processo entra em uma pilha de wait
				if(cpu != NULL && rand() % 100 > 95) {
					
					// tem q dar um jeito de ligar o pai e o filho para se o pai terminar ele esperar o filho em uma lista ou fila de espera de filhos, e se o filho terminar informar ao pai q ele morreu
					
					cpu->nFilhos++;
					Proc *filho = criarProc(pidPC, cpu->pid); // pid do pai é o pid do processo que está em execução
					pidPC++;
					inserirFilhos(cpu, filho->pid); // adiciona o filho em uma lista de filhos (ela informa se os filhos acabaram ou não
					printf("Processo filho %d criado pelo pai %d, Status: %s, CPU Burst: %d.\n", filho->pid, filho->pai, filho->status, filho->tempo_cpu);
					strcpy(filho->status, "ready");
					filho->inicio_esp = tempo;
					enqueueC(prontos, filho);                                                                                              
					printf("Processo %d, em fila de prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", filho->pid, filho->status, filho->pai, filho->tempo_cpu);
					
				}
			}
		}
		
		if(!vazia(listaPais)) { // aq verifica se algum pai pode ir para terminados (seus filhos todos terminaram)
			
			if(terminadosPais(listaPais, terminados)) { // se um ou mais pais tem todos os seus filhos em terminados, pode ser retirado e mandado para terminados tbm
				
				Proc* pai = NULL;
				// este loop continuará enquanto houver pais prontos para serem finalizados na lista
				while((pai = buscarPaiPronto(listaPais, terminados)) != NULL) {
				    
				    // encontramos um pai (paiPronto) que pode ser finalizado.
				    // primeiro, removemos ele da lista de espera.
				    removerProcesso(listaPais, pai->pid);
				
				    // agora que 'paiPronto' está "solto", fazemos os cálculos finais.
				    strcpy(pai->status, "terminated");
				
				    int tempoBloqFilhos = pai->inicio_bloq - tempo;
				    pai->tempo_bloq += tempoBloqFilhos;
				    pai->tempo_bloqF = tempoBloqFilhos; // Apenas o tempo na lista de pais
				    somaTempBloq += tempoBloqFilhos;
				    pai->inicio_bloq = 0;
				    
				    pai->tempo_esp += pai->inicio_esp - tempo;
				    pai->tempo_exe += pai->tempo_esp;
				    
				    // colocamos na fila de terminados.
				    printf("Processo %d terminado (filhos terminaram), Status: %s, Pai: %d, Tempo de Execucao: %d.\n", pai->pid, pai->status, pai->pai, pai->tempo_exe);
				    enqueue(terminados, pai);
				    procTerminados++;
				}
			}
		}
		
		if(!vazia(bloqHD)) { // se alguma fila de espera não estiver vazia, tem chance de liberar um processo
		
			if(bloqHD->okDequeue) {
				auxP = dequeue(bloqHD);
				
				auxP->fim_bloq = tempo; // ao sair de uma fila de espera/bloqueio, define o momento final do tempo de bloqueio e calcula o tempo de bloqueio, dps reseta o inicio e fim, para caso entre novamente em uma fila
				auxP->tempo_bloq += auxP->inicio_bloq - auxP->fim_bloq;
				somaTempBloq += auxP->inicio_bloq - auxP->fim_bloq;
				auxP->inicio_bloq = auxP->fim_bloq = 0;
				
				auxP->fim_esp = tempo; // o mesmo para tempo de espera
				auxP->tempo_esp += auxP->inicio_esp - auxP->fim_esp;
				auxP->inicio_esp = tempo;
				auxP->fim_esp = 0;
				
				printf("Processo %d, disco p/ prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", auxP->pid, auxP->status, auxP->pai, auxP->tempo_cpu);
				
				strcpy(auxP->status, "ready");
				enqueueC(prontos, auxP);
				bloqHD->okDequeue=0;
			} else if(rand() % 4 > 1) bloqHD->okDequeue=1;
			if(bloqHD->okDequeue) printf("Fila de Disco: Um processo pode ir p/ pronto.\n");
			
		}
		
		if(!vazia(bloqMouse)) {
			
			if(bloqMouse->okDequeue) {
				auxP = dequeue(bloqMouse);
				
				auxP->fim_bloq = tempo;
				auxP->tempo_bloq += auxP->inicio_bloq - auxP->fim_bloq;
				somaTempBloq += auxP->inicio_bloq - auxP->fim_bloq;
				auxP->inicio_bloq = auxP->fim_bloq = 0;
				
				auxP->fim_esp = tempo;
				auxP->tempo_esp += auxP->inicio_esp - auxP->fim_esp;
				auxP->inicio_esp = tempo;
				auxP->fim_esp = 0;
				
				printf("Processo %d, mouse p/ prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", auxP->pid, auxP->status, auxP->pai, auxP->tempo_cpu);
				
				strcpy(auxP->status, "ready");
				enqueueC(prontos, auxP);
				bloqMouse->okDequeue=0;
			} else if(rand() % 4 > 1) bloqMouse->okDequeue=1;
			if(bloqMouse->okDequeue) printf("Fila de Mouse: Um processo pode ir p/ pronto.\n");
			
		}
		
		if(!vazia(bloqTeclado)) {
			
			if(bloqTeclado->okDequeue) {
				auxP = dequeue(bloqTeclado);
				
				auxP->fim_bloq = tempo;
				auxP->tempo_bloq += auxP->inicio_bloq - auxP->fim_bloq;
				somaTempBloq += auxP->inicio_bloq - auxP->fim_bloq;
				auxP->inicio_bloq = auxP->fim_bloq = 0;
				
				auxP->fim_esp = tempo;
				auxP->tempo_esp += auxP->inicio_esp - auxP->fim_esp;
				auxP->inicio_esp = tempo;
				auxP->fim_esp = 0;
				
				printf("Processo %d, teclado p/ prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", auxP->pid, auxP->status, auxP->pai, auxP->tempo_cpu);
				
				strcpy(auxP->status, "ready");
				enqueueC(prontos, auxP);
				bloqTeclado->okDequeue=0;
			} else if(rand() % 4 > 1) bloqTeclado->okDequeue=1;
			if(bloqTeclado->okDequeue) printf("Fila de Teclado: Um processo pode ir p/ pronto.\n");
			
		}
		
		if(!vazia(bloqEvent)) {
			
			if(bloqEvent->okDequeue) {
				auxP = dequeue(bloqEvent);
				
				auxP->fim_bloq = tempo;
				auxP->tempo_bloq += auxP->inicio_bloq - auxP->fim_bloq;
				somaTempBloq += auxP->inicio_bloq - auxP->fim_bloq;
				auxP->inicio_bloq = auxP->fim_bloq = 0;
				
				auxP->fim_esp = tempo;
				auxP->tempo_esp += auxP->inicio_esp - auxP->fim_esp;
				auxP->inicio_esp = tempo;
				auxP->fim_esp = 0;
				
				printf("Processo %d, evento p/ prontos, Status: %s, Pai: %d, CPU Burst: %d.\n", auxP->pid, auxP->status, auxP->pai, auxP->tempo_cpu);
				
				strcpy(auxP->status, "ready");
				enqueueC(prontos, auxP);
				bloqEvent->okDequeue=0;
			} else if(rand() % 4 > 1) bloqEvent->okDequeue=1;
			if(bloqEvent->okDequeue) printf("Fila de Eventos: Um processo pode ir p/ pronto.\n");
			
		}
		
		// exibições das filas
		printf("\n\nExibicoes das Filas:\n");
		
		printf("\nFila de Prontos:\n");
		if(vazia(prontos)) printf("\tVazia.\n");
		else exibirC(prontos);
		
		printf("\nFila de Espera HD:\n");
		if(vazia(bloqHD)) printf("\tVazia.\n");
		else exibir(bloqHD);
		
		printf("\nFila de Espera Mouse:\n");
		if(vazia(bloqMouse)) printf("\tVazia.\n");
		else exibir(bloqMouse);
		
		printf("\nFila de Espera Teclado:\n");
		if(vazia(bloqTeclado)) printf("\tVazia.\n");
		else exibir(bloqTeclado);
		
		printf("\nFila de Espera Evento:\n");
		if(vazia(bloqEvent)) printf("\tVazia.\n");
		else exibir(bloqEvent);
		
		printf("\nFila de Espera de Filhos:\n");
		if(vazia(listaPais)) printf("\tVazia.\n");
		else exibir(listaPais);
		
		tempo--;
		Sleep(2000);
	}
	
	printf("\n\nA Simulacao terminou por completo. [ENTER] p/ continuar: "); getch(); printf("\n");
	
	printf("\n");
	for(z=1; z<=80; z++) printf("#"); printf("\n");
	
	printf("\nRELATORIOS:\n\n");
	
	//item 1 - feito
	printf("\nQuantidade de processos terminados: %d. (Alem do processo simulacao [1])\n", procTerminados);
	
	//item 2 - feito
	printf("\nQuantidade de processos bloqueados: %d.\n", procBloqueados);
	if(procBloqueados > 0) printf("Tempo medio de bloqueio: %.2f.\n", (float)somaTempBloq/procBloqueados);
	else printf("Tempo medio de bloqueio: 0.\n");
	
	// item 3 - feito
	printf("\nProcessos que estiveram entre o estado de Execucao e Pronto, e nunca foram Bloqueados:\n");
	if(vazia(terminados)) printf("Nao ha nenhum processo com essa caracteristica.\n");
	else exibirPertencentesItem3(terminados);
	
	// item 4 - feito
	printf("\nProcessos Terminados:\n");
	if(vazia(terminados)) printf("Nenhum processo foi terminado.\n");
	else exibirFinal(terminados);
	
	// item 5
	printf("\nProcessos que criaram filho(s):\n");
	if(!vazia(terminados)) {
		exibirComFilhos(terminados);
	} else printf("Nenhum processo criou filho(s).\n");
	
	printf("\nLimpando memoria...\n");
	destroy(&terminados);
	
	printf("\n\nSimulacao concluida. [ENTER] p/ continuar: "); getch(); printf("\n");
	
	return 0;
	
}

