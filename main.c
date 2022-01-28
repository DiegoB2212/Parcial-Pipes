
//INTEGRANTES: DIEGO BARRERA - DANIEL RINCON - ANGELA ROJAS

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#define READ 0
#define WRITE 1
#define PLAYER_A 0
#define PLAYER_B 1

char positions[9] = {' ',' ',' ',' ',' ',' ',' ',' ',' '}; // arreglo de posiciones de jugador

void printGameboard(int numbersR[9], char avaiblesR[9]); // imprime tablero de juego con posicones de jugador 
int takePoints(int numbersR[9], char avaiblesR[9], int player); // calcula el numero de puntos de cada jugador

int main()
{
	int T_H1_H2[2],T_H2_H1[2]; // tuberias
	pid_t processA, processB; // procesos
	int fd1, fd2;
	char movimientoJA [30], movimientoJB [30]; // arreglo que almacena jugada de cada player
    int numbers[9] = {1,2,3,4,5,6,7,8,9}; // arreglo de numeros para sumar
    int pointsA = 0; // Puntos Jugador A
    int pointsB = 0; // Puntos Jugador B

//---------------- CREACION DE TUBERIAS----------------------
 	if (pipe(T_H1_H2) < 0)
        exit(1);
	else if (pipe(T_H2_H1) < 0)
		exit(2);
	else
		printf("Pipes Created Succesfuly \n\n"); 
//-----------------CODIGO DE JUEGO --------------------------------
		
 	
 	processA = fork();
 	
 	if (processA == 0) //hijo 1
 	{   
	 	close(T_H1_H2[READ]); // Cierre tuberias no usaddas en el proceso hijo 1
	 	close(T_H2_H1[WRITE]);
	 	
	 	while(1)
 		{
 			fd2 = read(T_H2_H1[READ], &movimientoJB, sizeof(movimientoJA)); // lee dato recibido desde hijo 2

            // condicional contra sobre escritura en posiciones, si A esta en la posicion 3 B no podra ubicarse ahi 
            if(positions[(movimientoJB[0] & 0xf)-1] == ' ' || positions[(movimientoJA[0] & 0xf)-1] == ' ')
            {
                positions[(movimientoJB[0] & 0xf)-1] = 'A';
                positions[(movimientoJA[0] & 0xf)-1] = 'B';
            }
            // Calcula puntos dependiendo de las posiciones de los jugadores
            pointsA = takePoints(numbers, positions, PLAYER_A);
            pointsB = takePoints(numbers, positions, PLAYER_B);

            // Imprime tablero con cada una de las posiciones de ambos jugadores
            printGameboard(numbers, positions);
 			printf("Mensaje recibido por Jugador A (%d) es: %c \n\n", getpid(), movimientoJB[0]);
            printf("Jugador A tiene: %d Puntos\n", pointsA);

            // Condicionales para terminar el juego, en caso de que algun jugador consiga los 15 Pts o los sobrepase rompe ciclo de juego "while"
            if(pointsA == 15)
            {
                system("clear");
                printf("Jugador A tiene: %d Puntos\n", pointsA);
                printf("Gano el Jugador A");
                break;
            }
            else if(pointsB == 15)
            {
                system("clear");
                printf("Jugador B tiene: %d Puntos\n", pointsB);
                printf("Gano el Jugador B");
                break;
            }
            else if(pointsA > 15 || pointsB > 15)
            {
                system("clear");
                printf("Superaron los 15 Puntos");
                break;
            }

            //Envio de movimiento de jugador B a Jugador A
 			printf("Turno Jugador B (%d) \n",getpid());
 			fgets(movimientoJA, 30, stdin);
            sleep(1);
 			write(T_H1_H2[WRITE],&movimientoJA,sizeof(movimientoJA));
 			sleep(1);
 		}
 		close(T_H1_H2[WRITE]);
 	} 
 	else 
 	{
		processB = fork();
 		if (processB == 0) //hijo 2
 		{
            // MISMA LOGICA PARA JUGADOR 2
            //Cierre de tuberias no usadas
 			close(T_H1_H2[WRITE]);
 			close(T_H2_H1[READ]);
 			while(1)
			{
                //Lee movimiento jugador A y envia a Jugador B
                sleep(0.5);
 	    		printf("Turno Jugador A (%d) \n",getpid());
				fgets(movimientoJB, 30, stdin);
                sleep(1);
				write(T_H2_H1[WRITE],&movimientoJB,sizeof(movimientoJB));
				sleep(0.5);

                //Lee tecla de juego Jugador B
				fd1 = read(T_H1_H2[READ], &movimientoJA, sizeof(movimientoJA));

                // condicional contra sobre escritura en posiciones, si A esta en la posicion 3 B no podra ubicarse ahi 
                if(positions[(movimientoJB[0] & 0xf)-1] == ' ' || positions[(movimientoJA[0] & 0xf)-1] == ' ')
                {
                    positions[(movimientoJB[0] & 0xf)-1] = 'A';
                    positions[(movimientoJA[0] & 0xf)-1] = 'B';
                }

                //Obtencion de puntos
                pointsA = takePoints(numbers, positions, PLAYER_A);
                pointsB = takePoints(numbers, positions, PLAYER_B);

                //Imprime tablero de juego con movimientos
                printGameboard(numbers, positions);
                printf("Jugador B tiene: %d Puntos\n", pointsB);

                //Condicionales para tterminar el juego
                if(pointsA == 15)
                {
                    system("clear");
                    printf("Jugador A tiene: %d Puntos\n", pointsA);
                    printf("Gano el Jugador A");
                    break;
                }
                else if(pointsB == 15)
                {
                    system("clear");
                    printf("Jugador B tiene: %d Puntos\n", pointsB);
                    printf("Gano el Jugador B");
                    break;
                }
                else if(pointsA > 15 || pointsB > 15)
                {
                    system("clear");
                    printf("Superaron los 15 Puntos");
                    break;
                }
				printf("Mensaje recibido por Jugador B (%d) es: %d \n\n", getpid(), (movimientoJA[0] & 0xf));
                sleep(1);
			}
            close(T_H2_H1[WRITE]);
		} 
		else 
		{
			close(T_H1_H2[READ]);
			close(T_H1_H2[WRITE]);
			close(T_H2_H1[READ]);
			close(T_H2_H1[WRITE]);

			while(wait(NULL) !=-1);
			printf("\nGAME OVER \n");
		}
	}
	return 0;
 }
/*
    La funcion printGameboard se compone de 2 ciclos que se encargan de imprimir los dos que se encuentran dentro de los arreglos positions y numbers
    con una separacion con lines verticales de por medio para cumplir con la condicion grafica que se nos es solicitada, los valores del arreglo positions
    que es donde se guardan los movimeintos de los jugadores A y B seran re impresos con ayuda de esta funcion para observar donde se encuentran los 
    movimientos de los jugadores
*/
void printGameboard(int numbersR[9], char avaiblesR[9])
{
    printf("\n");
    for(int i = 0; i < 9; i++)
    {
        printf("|%d|", numbersR[i]);
    }
    printf("\n");

    for(int j = 0; j < 9; j++)
    {
        printf("|%c|", avaiblesR[j]);
    }
    printf("\n\n");
}

/*
    La funcion de takePoints utiliza en primera instancia un condicional con el que se preguntan el jugador que esta solicitando el calculo de sus puntos
    si el PÑAYER_A o PLAYER_B de esta manera ya sea que se pregunte por uno por otro se pasara a hacer un chekeo de todo el arreglo position de donde se 
    encuentran las B o A que a colocado cada jugador, y al encontrar alguna de estas suma el numero que se encuentre en la posicion de la letra guardado en 
    el arreglo numbers, es decir si se llega a revisar los puntos del Jugador A y se encuentra una A en la posicion 2 del arreglo positions se suma a los
    puntos el valor que se encuentre en la posicion 2 del arreglo numbers. Retornando al final los puntos a la varible desde donde fue invocada
*/

int takePoints(int numbersR[9], char avaiblesR[9], int player)
{
    int points = 0;
    if(player == PLAYER_A)
    {
        for(int i = 0; i < 9; i++)
        {
            if(avaiblesR[i] == 'A')
            {
                points += numbersR[i];
            }
        }
    }
    else
    {
        for(int j = 0; j < 9; j++)
        {
            if(avaiblesR[j] == 'B')
            {
                points += numbersR[j];
            }
        }
    }
   return points;; 
}
