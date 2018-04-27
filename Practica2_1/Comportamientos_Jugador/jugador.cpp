#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"


#include <iostream>
#include <cmath>
#include <queue>
#include <stack>
#include <algorithm>
#include <cstdlib>
#include <vector>

//////////////////////////////////////////////////////////////////////////////////////
// Metodos de Node
//////////////////////////////////////////////////////////////////////////////////////

void Node::borrar() {
    posX = posY = numGiros = distOrig = distFin = coste = 0;
    padre = 0;
}

void Node::copiar(const Node& otro) {
    posX = otro.posX;
    posY = otro.posY;
    padre = otro.padre;
    distOrig = otro.distOrig;
    distFin = otro.distFin;
    numGiros = otro.numGiros;
    coste = otro.coste;
}

Node::Node(int x, int y) {
    posX = x;
    posY = y;
    numGiros = distOrig = distFin = coste = 0;
    padre = 0;
}

Node::Node(int x, int y, Node* pad, Node* destino) {
    posX = x;
    posY = y;
    padre = pad;

    numGiros = pad->numGiros;

    if (hayGiro())               // Modificar esta linea en caso de error
        numGiros++;

    distOrig = pad->distOrig + 1;
    distFin = distanciaManhattan(*destino);
    coste = calcularCoste();
}

Node::Node(const Node& otro) {
    copiar(otro);
}

Node::~Node() {
    borrar();
}

int Node::getPosX() const {
    return posX;
}

int Node::getPosY() const {
    return posY;
}

int Node::getCoste() const {
    return coste;
}

Node* Node::getPadre() const {
    return padre;
}

Node& Node::operator=(const Node &otro) {
  
    if (this != &otro) {
        borrar();
        copiar(otro);
    }
  
    return *this;
}

bool Node::operator==(const Node& otro) const {
    if (this != &otro)
        return posX == otro.posX && posY == otro.posY;
    else
        return true;
}

bool Node::operator!=(const Node& otro) const {
    return !(*this == otro);
}

bool Node::hayGiro() {
    bool hayGiro = false;
        
    if (padre->padre != 0) {
        Node nodo = *(padre->padre);
        
        if (posX != nodo.posX && posY != nodo.posY)
            hayGiro = true;
    }

    return hayGiro;
}

int Node::calcularCoste() const {
    int costeCamino = distOrig + distFin + numGiros;

    return costeCamino;
}

int Node::distanciaManhattan(const Node& otro) const {
    return abs(otro.posX - posX) + abs(otro.posY - posY); 
}

// Devuelve 1 para giro a la derecha, 2 para giro a la izquierda
int Node::tipoGiro() const {
    Node padreNodo = *padre, antecesorPadre = *(padre->padre);
    int difHijoPadreX = posX - padreNodo.posX,
        difHijoPadreY = posY - padreNodo.posY,
        difPadreAncX  = antecesorPadre.posX - padreNodo.posX,
        difPadreAncY  = antecesorPadre.posY - padreNodo.posY;
        
    int giro;
    
    if (difHijoPadreY != 0) {
        if (difHijoPadreY == 1) {
            if (difPadreAncX == 1)
                giro = 1;
            else
                giro = 2;
        } else {
            if (difPadreAncX == 1)
                giro = 2;
            else
                giro = 1;
        }
    } else {
        if (difHijoPadreX == 1) {
            if (difPadreAncY == 1)
                giro = 2;
            else
                giro = 1;
        } else {
            if (difPadreAncY == 1)
                giro = 1;
            else
                giro = 2;
        }
    }
    
    return giro;
}

///////////////////////////////////////////////////////////////////////////////
// Metodos de ComportamientoJugador
///////////////////////////////////////////////////////////////////////////////

void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}


bool ComportamientoJugador::esNodoAdyacenteExplorable(int posX, int posY) {
    bool explorable = false;
    
    if (mapaResultado[posX][posY] == 'S' || mapaResultado[posX][posY] == 'T'
        || mapaResultado[posX][posY] == 'K')
        explorable = true;

    return explorable;
}

// Metodo de busqueda A*
void ComportamientoJugador::aStar(const estado &origen, const estado &destino, list<Action> &plan) {
    bool mapaExplorado[100][100];                                                       // Mapa de bits que indica cuales son las posiciones exploradas
    bool haySolucion = false;                                                           // Variable que indica si se ha encontrado una solucion (se ha alcanzado el objetivo)
    
    std::priority_queue<Node*, std::vector<Node*>, functor> frontera;                   // Frontera de nodos (nodos abiertos)
    std::list<Node*> explorados;                                                        // Lista de nodos explorados
    Node* nodoInicial = new Node(origen.fila, origen.columna);                          // Nodo inicial
    Node* nodoFinal   = new Node(destino.fila, destino.columna);                        // Nodo final (representacion simbolica)

    frontera.push(nodoInicial);

    // Se rellena el mapa de bits con false (ningun nodo se ha explorado)
    std::fill(mapaExplorado[0], mapaExplorado[0] + 100*100, false);

    while (!haySolucion && !frontera.empty()) {
        Node* nodoActual = frontera.top();
        int posX = nodoActual->getPosX(), posY = nodoActual->getPosY();

        // Marcar como explorado el nodo actual del mapa
        mapaExplorado[posX][posY] = true;

        // Comprobar si nodoActual es solucion
        if (*nodoActual == *nodoFinal) { 
            haySolucion = true;
        } else {
            if (esNodoAdyacenteExplorable(posX - 1, posY) && mapaExplorado[posX - 1][posY] == false)
                frontera.push(new Node(posX - 1, posY, nodoActual, nodoFinal));

            if (esNodoAdyacenteExplorable(posX + 1, posY) &&  mapaExplorado[posX + 1][posY] == false)
                frontera.push(new Node(posX + 1, posY, nodoActual, nodoFinal));

            if (esNodoAdyacenteExplorable(posX, posY - 1) && mapaExplorado[posX][posY - 1] == false)
                frontera.push(new Node(posX, posY - 1, nodoActual, nodoFinal));

            if (esNodoAdyacenteExplorable(posX, posY + 1) && mapaExplorado[posX][posY + 1] == false)
                frontera.push(new Node(posX, posY + 1, nodoActual, nodoFinal));
        }

        // Se aniade a la lista de nodos explorados
        explorados.push_front(nodoActual);

        // Se saca el elemento de la cima de la cola
        frontera.pop();
    }

    Node* iter = explorados.front();                                                    // Puntero que permite iterar sobre la lista de nodos
    Node* anterior;
    bool hayGiro = false;

    while(iter->getPadre() != 0) {
        plan.push_front(actFORWARD);

        hayGiro = iter->hayGiro();

        if (hayGiro) {
            int tipoGiro = iter->tipoGiro();

            if (tipoGiro == 1)
                plan.push_front(actTURN_R);
            else
                plan.push_front(actTURN_L);

            hayGiro = false;
        }
        
        anterior = iter;
        iter = iter->getPadre();
    }
    
    int difX = anterior->getPosX() - nodoInicial->getPosX(),
        difY = anterior->getPosY() - nodoInicial->getPosY();
    estado st = origen;


    switch(st.orientacion) {
        case 0:

            if (difX == 1) {
                plan.push_front(actTURN_R);
                plan.push_front(actTURN_R);
                st.orientacion = 2;
            } else if(difY == -1) {
                plan.push_front(actTURN_L);
                st.orientacion = 3;
            } else if(difY == 1) {
                plan.push_front(actTURN_R);
                st.orientacion = 1;
            }

            break;

        case 1:
 
            if (difY == -1) {
                plan.push_front(actTURN_R);
                plan.push_front(actTURN_R);
                st.orientacion = 3;
            } else if(difX == -1) {
                plan.push_front(actTURN_L);
                st.orientacion = 0;
            } else if(difX == 1) {
                plan.push_front(actTURN_R);
                st.orientacion = 2;
            }

            break;

        case 2:

            if (difX == -1) {
                plan.push_front(actTURN_R);
                plan.push_front(actTURN_R);
                st.orientacion = 0;
            } else if(difY == -1) {
                plan.push_front(actTURN_R);
                st.orientacion = 1;
            } else if(difY == 1) {
                plan.push_front(actTURN_L);
                st.orientacion = 3;
            }

            break;

        case 3:

            if (difY == 1) {
                plan.push_front(actTURN_R);
                plan.push_front(actTURN_R);
                st.orientacion = 1;
            } else if(difX == 1) {
                plan.push_front(actTURN_L);
                st.orientacion = 2;
            } else if(difX == -1) {
                plan.push_front(actTURN_R);
                st.orientacion = 0;
            }

            break;
    }


    for (auto iterExplorados = explorados.begin(); iterExplorados != explorados.end(); ++iterExplorados)
        delete *iterExplorados;

    while (!frontera.empty()) {
        Node* aux = frontera.top();
        delete aux;
        frontera.pop();
    }
}

bool ComportamientoJugador::pathFinding(const estado &origen, const estado &destino, list<Action> &plan) {
	//Borro la lista
	plan.clear();
	
    aStar(origen, destino, plan);

	// Descomentar para ver el plan en el mapa
	VisualizaPlan(origen, plan);

	return true;
}

Action ComportamientoJugador::think(Sensores sensores) {
  if (sensores.mensajeF != -1){
		fil = sensores.mensajeF;
		col = sensores.mensajeC;
	}

	// Actualizar el efecto de la ultima accion
	switch (ultimaAccion){
		case actTURN_R: brujula = (brujula+1)%4; break;
		case actTURN_L: brujula = (brujula+3)%4; break;
		case actFORWARD:
			switch (brujula){
				case 0: fil--; break;
				case 1: col++; break;
				case 2: fil++; break;
				case 3: col--; break;
			}
			cout << "fil: " << fil << "  col: " << col << " Or: " << brujula << endl;
	}



	// Determinar si ha cambiado el destino desde la ultima planificacion
	if (hayPlan and (sensores.destinoF != destino.fila or sensores.destinoC != destino.columna)){
		cout << "El destino ha cambiado\n";
		hayPlan = false;
	}

	// Determinar si tengo que construir un plan
	if (!hayPlan){
		estado origen;
		origen.fila = fil;
		origen.columna = col;
		origen.orientacion = brujula;

		destino.fila = sensores.destinoF;
		destino.columna = sensores.destinoC;

    		hayPlan = pathFinding(origen,destino,plan);
	}


	// Ejecutar el plan
	Action sigAccion;
	if (hayPlan and plan.size()>0){
		sigAccion = plan.front();
		plan.erase(plan.begin());
	}
	else {
		sigAccion = actIDLE;
	}

	ultimaAccion = sigAccion;
	return sigAccion;
}


void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}

void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
