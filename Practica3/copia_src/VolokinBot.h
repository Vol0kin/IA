/*
 * AlumnoBot.h
 *
 *  Created on: 15 ene. 2018
 *      Author: manupc
 */

#include "Bot.h"

#ifndef MANUPCBOT_H_
#define MANUPCBOT_H_

class VolokinBot:Bot {
private:
	Move movimiento;
	int nodosExplorados;
	int lim;
public:
	VolokinBot();
	~VolokinBot();


	void initialize();
	string getName();
	Move nextMove(const vector<Move> &adversary, const GameState &state);
	int podaAlfaBeta(const GameState& estado, int alfa, int beta, int k, const Player& jugador);
	int heuristica(GameState estado, Player jugador);
};

#endif /* MANUPCBOT_H_ */
