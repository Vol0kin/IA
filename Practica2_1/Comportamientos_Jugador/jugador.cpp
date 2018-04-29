#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"


#include <iostream>
#include <cmath>
#include <queue>
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

    if (hayGiro())
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

// Devuelve si un nodo ayacente es explorable
bool ComportamientoJugador::esNodoAdyacenteExplorable(int posX, int posY, const vector< vector <unsigned char> >& mapa) {
    bool explorable = false;

    if (posX >= 0 && posX < mapa.size() && posY >= 0 && posY < mapa.size()) {
        if (mapa[posX][posY] == 'S' || mapa[posX][posY] == 'T'
            || mapa[posX][posY] == 'K' || mapa[posX][posY] == '?')
            explorable = true;
    }

    return explorable;
}

// Permite obtener la casilla situada en frente del jugador
Node ComportamientoJugador::obtenerCasillaFrente(const estado &origen) {
    Node casillaFrente(0, 0);

    switch(origen.orientacion) {
        case 0:
            casillaFrente = Node(origen.fila - 1, origen.columna);
            break;
        case 1:
            casillaFrente = Node(origen.fila, origen.columna + 1);
            break;
        case 2:
            casillaFrente = Node(origen.fila + 1, origen.columna);
            break;
        case 3:
            casillaFrente = Node(origen.fila, origen.columna - 1);
            break;
    }

    return casillaFrente;
}

// Metodo de busqueda A*
void ComportamientoJugador::aStar(const estado &origen, const estado &destino,
                                  list<Action> &plan, const vector< vector <unsigned char> >& mapa,
                                  bool ignorarAldeano) {
    vector< vector <bool> > mapaExplorado;                                              // Mapa de bits que indica cuales son las posiciones exploradas
    mapaExplorado.resize(200);
    bool haySolucion = false;                                                           // Variable que indica si se ha encontrado una solucion (se ha alcanzado el objetivo)
    
    priority_queue<Node*, vector<Node*>, functor> frontera;                             // Frontera de nodos (nodos abiertos)
    std::list<Node*> explorados;                                                        // Lista de nodos explorados
    Node* nodoInicial = new Node(origen.fila, origen.columna);                          // Nodo inicial
    Node* nodoFinal   = new Node(destino.fila, destino.columna);                        // Nodo final (representacion simbolica)
    Node nodoIgnorar = obtenerCasillaFrente(origen);                                    // Nodo a ignorar (inicialmente el 0,0 , si hay que ignorar alguno se determinara mas adelante)

    frontera.push(nodoInicial);

    // Se rellena el mapa de bits con false (ningun nodo se ha explorado)
    fill(mapaExplorado.begin(), mapaExplorado.end(), vector<bool>(200, false));

    if (ignorarAldeano)
        mapaExplorado[nodoIgnorar.getPosX()][nodoIgnorar.getPosY()] = true;

    while (!haySolucion && !frontera.empty()) {
        Node* nodoActual = frontera.top();
        int posX = nodoActual->getPosX(), posY = nodoActual->getPosY();

        // Marcar como explorado el nodo actual del mapa
        mapaExplorado[posX][posY] = true;

        // Comprobar si nodoActual es solucion
        if (*nodoActual == *nodoFinal) { 
            haySolucion = true;
        } else {
            if (esNodoAdyacenteExplorable(posX - 1, posY, mapa) && (posX - 1) >= 0
                && mapaExplorado[posX - 1][posY] == false)
                frontera.push(new Node(posX - 1, posY, nodoActual, nodoFinal));

            if (esNodoAdyacenteExplorable(posX + 1, posY, mapa) && (posX + 1) < mapa.size()
                && mapaExplorado[posX + 1][posY] == false)
                frontera.push(new Node(posX + 1, posY, nodoActual, nodoFinal));

            if (esNodoAdyacenteExplorable(posX, posY - 1, mapa) && (posY - 1) >= 0
                && mapaExplorado[posX][posY - 1] == false)
                frontera.push(new Node(posX, posY - 1, nodoActual, nodoFinal));

            if (esNodoAdyacenteExplorable(posX, posY + 1, mapa) && (posY + 1) < mapa.size()
                && mapaExplorado[posX][posY + 1] == false)
                frontera.push(new Node(posX, posY + 1, nodoActual, nodoFinal));
        }

        // Se aniade a la lista de nodos explorados
        explorados.push_front(nodoActual);

        // Se saca el elemento de la cima de la cola
        frontera.pop();
    }

    // Comprobar si hay solucion
    if (haySolucion) {
        Node* iter = explorados.front();                                                // Puntero que permite iterar sobre la lista de nodos
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

        // Determinar primer giro (en caso de que sea necesario hacerlo)
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
                    st.orientacion = 3;
                } else if(difY == 1) {
                    plan.push_front(actTURN_L);
                    st.orientacion = 1;
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
    }


    // Liberacion de memoria dinamica
    for (auto iterExplorados = explorados.begin(); iterExplorados != explorados.end(); ++iterExplorados)
        delete *iterExplorados;

    while (!frontera.empty()) {
        Node* aux = frontera.top();
        delete aux;
        frontera.pop();
    }
}

bool ComportamientoJugador::pathFinding(const estado &origen, const estado &destino, list<Action> &plan, const vector< vector <unsigned char> >& mapa, bool ignorarAldeano) {
	//Borro la lista
	plan.clear();
	
    aStar(origen, destino, plan, mapa, ignorarAldeano);

	// Descomentar para ver el plan en el mapa
	//VisualizaPlan(origen, plan);

	return true;
}

// Determina si la casilla situada en frente del agente es un obstaculo
bool ComportamientoJugador::esCasillaFrenteObstaculo(Sensores sensores) {
    bool evitar = false;
    unsigned char elementoFrente = sensores.terreno[2];

    if (elementoFrente == 'B' || elementoFrente == 'A' || 
        elementoFrente == 'P' || elementoFrente == 'M' ||
        elementoFrente == 'D')
        evitar = true;

    return evitar;
}

// Determina si la casilla en frente del agente es un aldeano
bool ComportamientoJugador::esCasillaFrenteAldeano(Sensores sensores) {
    return sensores.superficie[2] == 'a';
}

// Permite aniadir a la informacion del mapa que se posee la superficie captada por los sensores
void ComportamientoJugador::rellenarMapa(vector< vector <unsigned char> >& mapa, Sensores sensores) {
    int posicionesCentralesSensores[4] = {0, 2, 6, 12};
    int filaReferencia = fil, columnaReferencia = col, indice;

    for (int i = 0; i < 4; i++) {
        switch(brujula) {
            case 0:
                filaReferencia = fil - i;
                break;
            case 1:
                columnaReferencia = col + i;
                break;
            case 2:
                filaReferencia = fil + i;
                break;
            case 3:
                columnaReferencia = col - i;
                break;
        }

        indice = posicionesCentralesSensores[i];
        mapa[filaReferencia][columnaReferencia] = sensores.terreno[indice];

        for (int j = 1; j <= i; j++) {
            switch(brujula) {
                case 0:
                    mapa[filaReferencia][columnaReferencia - j] = sensores.terreno[indice - j];
                    mapa[filaReferencia][columnaReferencia + j] = sensores.terreno[indice + j];
                    break;
                case 1:
                    mapa[filaReferencia - j][columnaReferencia] = sensores.terreno[indice - j];
                    mapa[filaReferencia + j][columnaReferencia] = sensores.terreno[indice + j];
                    break;
                case 2:
                    mapa[filaReferencia][columnaReferencia + j] = sensores.terreno[indice - j];
                    mapa[filaReferencia][columnaReferencia - j] = sensores.terreno[indice + j];
                    break;
                case 3:
                    mapa[filaReferencia + j][columnaReferencia] = sensores.terreno[indice - j];
                    mapa[filaReferencia - j][columnaReferencia] = sensores.terreno[indice + j];
                    break;
            }
        }
    }
}

// Comprueba si se ha encontrado un punto de referencia
bool ComportamientoJugador::detectadoPuntoReferencia(Sensores sensores) {
    auto posicion = find(sensores.terreno.begin(), sensores.terreno.end(), 'K');

    return posicion != sensores.terreno.end();
}

// Busca el punto de referencia en el radar
void ComportamientoJugador::identificarPK() {
    bool encontrado = false;
    int i, j;

    for (i = 0; i < radar.size() && !encontrado; i++) {
        for (j = 0; j < radar.size() && !encontrado; j++) {
            if (radar[i][j] == 'K')
                encontrado = true;
        }
    }
    
    localizacionPK.fila = i - 1;
    localizacionPK.columna = j - 1;
}


// Determina cual es la siguiente accion que tiene que realizar el comportamiento reactivo
// Marca objetivos en el mapa a los que el agente intenta llegar, reconociendo su entorno
// a la vez que intenta alcanzarlos. el objetivo es llegar a un punto de referencia K
Action ComportamientoJugador::decidirSiguienteAccion(Sensores sensores) { 
    static list<Action> planAux;                                                                        // Lista de acciones para el agente reactivo
    estado objetivos[12];                                                                               // Objetivos que se impone el agente reactivo
    initObjetivosReactivos(objetivos);                                                                  // Inicializacion de los objetivos
    static int objetivoActual = 0, numReplanificaciones = 0;                                            // Indica el objetivo actual y el numero de planificaciones extra
                                                                                                        // que se han hecho (max 15)
    static bool hayPlanObjetivo = false;                                                                // Guarda si hay un plan para llegar al objetivo
    static bool detectadoPK = false;                                                                    // Guarda si se ha localizado un punto de referencia
    bool necesitaReplanificar;                                                                          // Indica si se tiene que volver a planificar
    Action accion;                                                                                      // Accion a realizar

    // Se replanifica si la casilla de enfrente es un aldeano o un obstaculo
    necesitaReplanificar = esCasillaFrenteObstaculo(sensores) || esCasillaFrenteAldeano(sensores);

    // Solo se comprueba si se ha detectado PK si no se ha detectado hasta ahora
    if (!detectadoPK)
        detectadoPK = detectadoPuntoReferencia(sensores);

    if (!detectadoPK) {
        if (!hayPlanObjetivo) {
            estado inicio, fin;
            inicio.fila = fil;
            inicio.columna = col;
            inicio.orientacion = brujula;

            fin = objetivos[objetivoActual];

            if (esCasillaFrenteAldeano(sensores))
                hayPlanObjetivo = pathFinding(inicio, fin, planAux, radar, true);
            else
                hayPlanObjetivo = pathFinding(inicio, fin, planAux, radar);

            numReplanificaciones++;
            necesitaReplanificar = false;
        }

        if (hayPlanObjetivo && planAux.size() > 0) {
            if (necesitaReplanificar) {
                estado inicio, fin;
                inicio.fila = fil;
                inicio.columna = col;
                inicio.orientacion = brujula;
                
                if (numReplanificaciones == 15) {
                    objetivoActual = (objetivoActual + 1) % 12;
                    numReplanificaciones = 0;
                }

                fin = objetivos[objetivoActual];

                if (esCasillaFrenteAldeano(sensores))
                    hayPlanObjetivo = pathFinding(inicio, fin, planAux, radar, true);
                else
                    hayPlanObjetivo = pathFinding(inicio, fin, planAux, radar);

                numReplanificaciones++;
                necesitaReplanificar = false;
            }
            
            if (!necesitaReplanificar && planAux.size() > 0) {
                accion = planAux.front();
                planAux.erase(planAux.begin());
            } else {
                hayPlanObjetivo = false;
                accion = actIDLE;
            }
        } else {
            hayPlanObjetivo = false;
            objetivoActual = (objetivoActual + 1) % 12;
            numReplanificaciones = 0;
            accion = actIDLE;
        }
    } else {
        if (!hayPlanPK) {
            numReplanificaciones = 0;
            objetivoActual = 0;
            identificarPK();

            estado inicio;
            inicio.fila = fil;
            inicio.columna = col;
            inicio.orientacion = brujula;

            if (esCasillaFrenteAldeano(sensores))
                hayPlanPK = pathFinding(inicio, localizacionPK, planPK, radar, true);
            else
                hayPlanPK = pathFinding(inicio, localizacionPK, planPK, radar);

            necesitaReplanificar = false;
        }

        if (hayPlanPK && planPK.size() > 0) {
            if (necesitaReplanificar) {
                estado inicio;
                inicio.fila = fil;
                inicio.columna = col;
                inicio.orientacion = brujula;
                
                if (esCasillaFrenteAldeano(sensores))
                    hayPlanPK = pathFinding(inicio, localizacionPK, planPK, radar, true);
                else
                    hayPlanPK = pathFinding(inicio, localizacionPK, planPK, radar);

                necesitaReplanificar = false;
            }
            
            if (!necesitaReplanificar && planPK.size() > 0) {
                accion = planPK.front();
                planPK.erase(planPK.begin());
            } else {
                hayPlanPK = false;
                accion = actIDLE;
            }
        } else {
            hayPlanPK = false;
            accion = actIDLE;
        }
    }

    return accion;
}

// Distintos objetivos que intenta alcanzar el agente en el comportamiento reactivo
void ComportamientoJugador::initObjetivosReactivos(estado objetivos[]) {
    objetivos[0].fila  = 0;   objetivos[0].columna = 199;
    objetivos[1].fila  = 0;   objetivos[1].columna = 0;
    objetivos[2].fila  = 199; objetivos[2].columna = 199;
    objetivos[3].fila  = 199; objetivos[3].columna = 0;
    objetivos[4].fila  = 0;   objetivos[4].columna = 99;
    objetivos[5].fila  = 199; objetivos[5].columna = 99;
    objetivos[6].fila  = 99;  objetivos[6].columna = 0;
    objetivos[7].fila  = 99;  objetivos[7].columna = 199;
    objetivos[8].fila  = 49;  objetivos[8].columna = 49;
    objetivos[9].fila  = 149; objetivos[9].columna = 149;
    objetivos[10].fila = 149; objetivos[10].columna = 49;
    objetivos[11].fila = 49;  objetivos[11].columna = 149;

}

// Copia el radar en el mapa
void ComportamientoJugador::copiarMapas(int auxFil, int auxCol) {
    int transX = abs(auxFil - fil), transY = abs(auxCol - col);

    for (int i = 0; i < mapaResultado.size(); i++) {
        for (int j = 0; j < mapaResultado.size(); j++)
           mapaResultado[i][j] = radar[transX + i][transY + j];
    }
}

Action ComportamientoJugador::think(Sensores sensores) {
    static bool expandirMapa = false, posicionConocida = false;

    // Se comprueba si se conoce la posicion actual (para el nivel 3)
    if (mapaResultado[0][0] == '?' && sensores.terreno[0] != 'K' && !hayPlan)
        posicionConocida = false;
    else
        posicionConocida = expandirMapa = true;

    if (sensores.mensajeF != -1){
        int auxFil = fil, auxCol = col;

		fil = sensores.mensajeF;
		col = sensores.mensajeC;
        
        // Copiar el mapa del radar en mapaResultado en caso de haber realizado
        // una busqueda reactiva
        if (mapaResultado[fil][col] == '?')
            copiarMapas(auxFil, auxCol);
	}

    if (posicionConocida) {
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

            hayPlan = pathFinding(origen,destino,plan, mapaResultado);
            VisualizaPlan(origen, plan);
    	}


    	// Ejecutar el plan	
    	if (hayPlan and plan.size()>0){
            // Comprobar si hay un aldeano bloqueando el camino
            if (expandirMapa) {
                rellenarMapa(mapaResultado, sensores);
                if (esCasillaFrenteObstaculo(sensores)) {
                    estado estadoActual;
                    estadoActual.fila = fil;
                    estadoActual.columna = col;
                    estadoActual.orientacion = brujula;

                    hayPlan = pathFinding(estadoActual, destino, plan, mapaResultado);
                    VisualizaPlan(estadoActual, plan);
                }
            }

            if (esCasillaFrenteAldeano(sensores) && plan.front() == actFORWARD) {
                list<Action> planAuxiliar;                  // Plan auxiliar en caso de ser necesario cambiar el plan

                estado estadoActual;
                estadoActual.fila = fil;
                estadoActual.columna = col;
                estadoActual.orientacion = brujula;

                Node nodoFrente = obtenerCasillaFrente(estadoActual);

                // Si el nodo siguiente es el destino, hay que esperar a que se aparte el aldeano
                if (nodoFrente.getPosX() == destino.fila && nodoFrente.getPosY() == destino.columna)
                    ultimaAccion = actIDLE;
                else {
                    hayPlan = pathFinding(estadoActual, destino, planAuxiliar, mapaResultado, true);

                    if (planAuxiliar.empty())
                        ultimaAccion = actIDLE;
                    else if(( (int)(planAuxiliar.size() - plan.size()) > 20) && numEsperas < 10 ) {
                        numEsperas++;
                        ultimaAccion = actIDLE;
                        cout << "Esperando a que se aparte el aldeano..." << endl;
                    } else {
                        cout << "Cambiando plan de busqueda" << endl;
                        plan = planAuxiliar;
	                    VisualizaPlan(estadoActual, plan);
                        numEsperas = 0;
		                ultimaAccion = plan.front();
		                plan.erase(plan.begin());
                    }
                }
            } else {
                numEsperas = 0;
    		    ultimaAccion = plan.front();
		        plan.erase(plan.begin());
            }
    	} else 
	    	ultimaAccion = actIDLE;
    } else {
        rellenarMapa(radar, sensores);
        ultimaAccion = decidirSiguienteAccion(sensores);
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

	return ultimaAccion;
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
