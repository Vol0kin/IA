/*
 * AlumnoBot.cpp
 *
 *  Created on: 15 ene. 2018
 *      Author: manupc
 */

#include "VolokinBot.h"

#include <string>
#include <cstdlib>
#include <iostream>
#include <climits>
using namespace std;

VolokinBot::VolokinBot() {
	// Inicializar las variables necesarias para ejecutar la partida
	movimiento = M_NONE;

}

VolokinBot::~VolokinBot() {
	// Liberar los recursos reservados (memoria, ficheros, etc.)
}

void VolokinBot::initialize() {
	// Inicializar el bot antes de jugar una partida
}

string VolokinBot::getName() {
	return "VolokinBot"; // Sustituir por el nombre del bot
}

Move VolokinBot::nextMove(const vector<Move> &adversary, const GameState &state) {

	movimiento = M_NONE;

	int alfa = INT_MIN,
		beta = INT_MAX,
		k = 0;

	Player jugador = state.getCurrentPlayer();

	int poda = podaAlfaBeta(state, alfa, beta, 0, state.getCurrentPlayer());

	// Implementar aquí la selección de la acción a realizar

	// OJO: Recordatorio. NO USAR cin NI cout.
	// Para salidas por consola (debug) utilizar cerr. Ejemplo:
	// cerr<< "Lo que quiero mostrar"<<endl;


	// OJO: Recordatorio. El nombre del bot y de la clase deben coincidir.
	// En caso contrario, el bot no podrá participar en la competición.
	// Se deberá sustituir el nombre AlumnoBot como nombre de la clase por otro
	// seleccionado por el alumno. Se deberá actualizar también el nombre
	// devuelto por el método getName() acordemente.

	return movimiento;
}

int VolokinBot::heuristica(GameState estado, Player max) {
	int semillasJ1, semillasJ2, diffSemillas;
	
	semillasJ1 = estado.getScore( (Player) 0);
	semillasJ2 = estado.getScore( (Player) 1);
	
	if (max == 0)
		diffSemillas = semillasJ1 - semillasJ2;
	else
		diffSemillas = semillasJ2 - semillasJ1;

	if (estado.isFinalState()) {
		if (estado.getWinner() == max)
			diffSemillas = 48;
		else
			diffSemillas = -48;
	}
	
	return diffSemillas;
}

/*
	max permite conocer en que lado esta el jugador actual
*/

int VolokinBot::podaAlfaBeta(const GameState& estado, int alfa, int beta, int k, const Player& jugador) {
	int valor;

	if (estado.isFinalState() || k == 6)
		return heuristica(estado, jugador);
	
	if (estado.getCurrentPlayer() == jugador) {
		for (int i = 1; i <= 6; i++) {
			Move mov = (Move) i;

			valor = podaAlfaBeta(estado.simulateMove(mov), alfa, beta, k + 1, jugador);

			if (k == 0) {
				if (valor > alfa) {
					movimiento = mov;
				}
			}

			alfa = max(valor, alfa);

			if (beta <= alfa)
				return beta;
		}

		return alfa;
		
	} else {

		for (int i = 1; i <= 6; i++) {
			Move mov = (Move) i;

			valor = podaAlfaBeta(estado.simulateMove(mov), alfa, beta, k + 1, jugador);
			beta = min(valor, beta);

			if (beta <= alfa)
				return alfa;
		}

		return beta;
	}
	
	return 0;
}
