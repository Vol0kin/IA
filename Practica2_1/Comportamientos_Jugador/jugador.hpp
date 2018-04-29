#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <algorithm>

struct estado {
  int fila;
  int columna;
  int orientacion;
};

class Node {
  private:
      int posX, posY;
      int numGiros;
      int distOrig, distFin;
      int coste;
      Node* padre;
      
      void borrar();
      void copiar(const Node& otro);
  public:
      Node(int x, int y);
      Node(int x, int y, Node* pad, Node* destino);
      Node(const Node& otro);
      ~Node();
      Node& operator=(const Node &otro);
      bool operator==(const Node& otro) const;
      bool operator!=(const Node& otro) const;
      int getPosX() const;
      int getPosY() const;
      Node* getPadre() const;
      int getCoste() const;
      int calcularCoste() const;
      int distanciaManhattan(const Node& otro) const;
      bool hayGiro();
      int tipoGiro() const;
};

struct functor {
    bool operator()(const Node* a, const Node* b) {
        return a->getCoste() > b->getCoste();
    }
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      ultimaAccion = actIDLE;
      hayPlan = false;
      numEsperas = 0;
      radar.resize(200);
      fill(radar.begin(), radar.end(), vector<unsigned char>(200, '?'));
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      fil = col = 99;
      brujula = 0; // 0: Norte, 1:Este, 2:Sur, 3:Oeste
      destino.fila = -1;
      destino.columna = -1;
      destino.orientacion = -1;
      ultimaAccion = actIDLE;
      hayPlan = false;
      numEsperas = 0;
      radar.resize(200);
      fill(radar.begin(), radar.end(), vector<unsigned char>(200, '?'));
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);

    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}

  private:
    // Declarar Variables de Estado
    int fil, col, brujula;
    estado destino;
    list<Action> plan;

    // Nuevas Variables de Estado
    Action ultimaAccion;
    bool hayPlan;

    // Numero de esperas a que el aldeano se aparte (numEsperas <= 10)
    unsigned numEsperas;

    // Radar que permite ir guardando el mapa obtenido relativo a la posicion del agente
    vector< vector<unsigned char> > radar;

    Node obtenerCasillaFrente(const estado &origen);
    
    bool esNodoAdyacenteExplorable(int posX, int posY, const vector< vector <unsigned char> >& mapa);
    void rellenarMapa(vector< vector <unsigned char> >& mapa, Sensores sensores);
    bool esCasillaFrenteObstaculo(Sensores sensores);
    Action decidirSiguienteAccion(Sensores sensores);
    void aStar(const estado &origen, const estado &destino, list<Action> &plan, const vector< vector <unsigned char> >& mapa, bool ignorarAldeano);
    bool pathFinding(const estado &origen, const estado &destino, list<Action> &plan, const vector< vector <unsigned char> >& mapa, bool ignorarAldeano = false);
    void PintaPlan(list<Action> plan);
};

#endif
