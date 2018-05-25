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
public:
	VolokinBot();
	~VolokinBot();


	void initialize();
	string getName();
	Move nextMove(const vector<Move> &adversary, const GameState &state);
};

#endif /* MANUPCBOT_H_ */
