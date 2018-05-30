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

	nodosExplorados = 0;
	int poda = podaAlfaBeta(state, alfa, beta, 0, state.getCurrentPlayer());
	cerr << nodosExplorados << endl;
	

	return movimiento;
}

int VolokinBot::heuristica(GameState estado, Player max) {
	int semillasJ1, semillasJ2, diffSemillas;
	
	semillasJ1 = estado.getScore( (Player) 0);
	semillasJ2 = estado.getScore( (Player) 1);
	
	if (max == 0) {
		diffSemillas = semillasJ1 - semillasJ2;

		for (int i = 1; i <= 6; i++) {
			if (estado.getSeedsAt( (Player) 0, (Position) i) == i)
				diffSemillas++;
		}

		for (int i = 1; i <= 6; i++) {
			if (estado.getSeedsAt( (Player) 0, (Position) i) == 0 && estado.getSeedsAt( (Player) 1, (Position) (7-i)) != 0) {
				for (int j = i + 1; j <= 6; j++) {
					if (estado.getSeedsAt( (Player) 0, (Position) j) == j-i) {
						diffSemillas += estado.getSeedsAt( (Player) 1, (Position) (7-i));
					}
				}
			}
		}
	}
	else {
		diffSemillas = semillasJ2 - semillasJ1;

		for (int i = 1; i <= 6; i++) {
			if (estado.getSeedsAt( (Player) 1, (Position) i) == i)
				diffSemillas++;
		}

		for (int i = 1; i <= 6; i++) {
			if (estado.getSeedsAt( (Player) 1, (Position) i) == 0 && estado.getSeedsAt( (Player) 0, (Position) (7-i)) != 0) {
				for (int j = i + 1; j <= 6; j++) {
					if (estado.getSeedsAt( (Player) 1, (Position) j) == j-i) {
						diffSemillas += estado.getSeedsAt( (Player) 0, (Position) (7-i));
					}
				}
			}
		}
	}
	
	return diffSemillas;
}

/*
	max permite conocer en que lado esta el jugador actual
*/

int VolokinBot::podaAlfaBeta(const GameState& estado, int alfa, int beta, int k, const Player& jugador) {
	int valor;
	nodosExplorados++;

// probar a variar la profundidad entre 12 y 14
	// anteriormente se usaba 13
	if (estado.isFinalState() || k == 11)
		return heuristica(estado, jugador);
	
	if (estado.getCurrentPlayer() == jugador) {
		for (int i = 1; i <= 6; i++) {
			Move mov = (Move) i;

			valor = podaAlfaBeta(estado.simulateMove(mov), alfa, beta, k + 1, jugador);

			if (k == 0) {
				if (valor > alfa) {
					movimiento = mov;
					cerr << movimiento << endl;
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
