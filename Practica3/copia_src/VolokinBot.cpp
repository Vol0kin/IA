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
	lim = 0;

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
	static int threshhold = 60000;
	static bool incrementar = false;
	static int i = 1;
	movimiento = M_NONE;

	int alfa = INT_MIN,
		beta = INT_MAX,
		k = 0;


	Player jugador = state.getCurrentPlayer();
	if (jugador == 1) {
		if (incrementar) {
			lim += i;
			incrementar = false;
			i++;
		}
	}

	nodosExplorados = 0;
	int poda = podaAlfaBeta(state, alfa, beta, 0, state.getCurrentPlayer());
	cerr << nodosExplorados << endl;

	if (nodosExplorados < threshhold) {
		incrementar = true;
		threshhold /= 2;
	}	

	return movimiento;
}

int VolokinBot::heuristica(GameState estado, Player jugador) {
	int semillasJug[2], diffSemillas, diffHuecos = 0, diffSemSeg = 0, maxPerJug = 0, maxPerCont = 0;
	Player contrario = (Player) (1-jugador);

	semillasJug[0] = estado.getScore( (Player) 0);
	semillasJug[1] = estado.getScore( (Player) 1);


	diffSemillas = semillasJug[jugador] - semillasJug[contrario];

	for (int i = 1; i <= 6; i++) {
		if (estado.getSeedsAt(jugador, (Position) i) <= i) {
			if (estado.getSeedsAt(jugador, (Position) i) == 0)
				diffHuecos++;
			diffSemSeg += estado.getSeedsAt(jugador, (Position) i);
		} else {
			diffSemSeg += i;
			maxPerJug = max(estado.getSeedsAt(jugador, (Position) i) - i, maxPerJug);
		}

		if (estado.getSeedsAt(contrario, (Position) i) <= i) {
			if (estado.getSeedsAt( contrario, (Position) i) == 0)
				diffHuecos--;
			diffSemSeg -= estado.getSeedsAt(contrario, (Position) i);
		} else {
			diffSemSeg -= i;
			maxPerCont = max(estado.getSeedsAt(contrario, (Position) i) - i, maxPerCont);
		}
	}
	
	diffSemillas += diffHuecos + diffSemSeg + (maxPerCont - maxPerJug);

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
	if (estado.isFinalState() || k == (11 + lim))
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
