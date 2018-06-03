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
	movimiento = M_NONE;
	lim = 0;
}

VolokinBot::~VolokinBot() { }

void VolokinBot::initialize() { }

string VolokinBot::getName() {
	return "VolokinBot";
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
	int poda = podaAlfaBeta(state, alfa, beta, 0, jugador);

	if (nodosExplorados < threshhold) {
		incrementar = true;
		threshhold /= 2;
	}	

	return movimiento;
}

int VolokinBot::heuristica(GameState estado, Player jugador) {
	int semillasJug[2], diffSemillas, diffHuecos = 0, diffSemSeg = 0, maxPerJug = 0, maxPerCont = 0, roboJug = 0, roboContr = 0;
	Player contrario = (Player) (1-jugador);

	semillasJug[0] = estado.getScore( (Player) 0);
	semillasJug[1] = estado.getScore( (Player) 1);


	diffSemillas = semillasJug[jugador] - semillasJug[contrario];

	for (int i = 1; i <= 6; i++) {
		if (estado.getSeedsAt(jugador, (Position) i) <= i) {
			if (estado.getSeedsAt(jugador, (Position) i) == 0) {
				diffHuecos++;
				if (estado.getSeedsAt(contrario, (Position) (7-i)) != 0){
					for (int j = i + 1; j <= 6; j++) {
						if (estado.getSeedsAt(jugador, (Position) j) == j - i)
							roboJug = max(roboJug, estado.getSeedsAt(contrario, (Position) (7-i))+1);
					}
				}					
			}
			diffSemSeg += estado.getSeedsAt(jugador, (Position) i);
		} else {
			diffSemSeg += i;
			maxPerJug = max(estado.getSeedsAt(jugador, (Position) i) - i, maxPerJug);
		}

		if (estado.getSeedsAt(contrario, (Position) i) <= i) {
			if (estado.getSeedsAt( contrario, (Position) i) == 0) {
				diffHuecos--;
				if (estado.getSeedsAt(jugador, (Position) i) != 0){
					for (int j = i + 1; j <= 6; j++) {
						if (estado.getSeedsAt(contrario, (Position) j) == j - i)
							roboContr = max(roboContr, estado.getSeedsAt(jugador, (Position) i)+1);
					}
				}
			}			
			diffSemSeg -= estado.getSeedsAt(contrario, (Position) i);
		} else {
			diffSemSeg -= i;
			maxPerCont = max(estado.getSeedsAt(contrario, (Position) i) - i, maxPerCont);
		}
	}
	
	diffSemillas += diffHuecos + 3 * diffSemSeg + 2 * (maxPerCont - maxPerJug) + 2 * (roboJug - roboContr);

	return diffSemillas;
}


int VolokinBot::podaAlfaBeta(const GameState& estado, int alfa, int beta, int k, const Player& jugador) {
	int valor;
	nodosExplorados++;

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
