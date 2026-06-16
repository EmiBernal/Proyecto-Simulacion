#include "generador_ordenes.h"
#include "event.h"

// Inicializacion
void generador_ordenes::init(double t, ...) {
  caudalObjetivo = 100;
  sigma = 0;
  indice = 0;
}

// Defino el ta
double generador_ordenes::ta(double t) { return sigma; }

// Defino la delta interna
void generador_ordenes::dint(double t) {
  caudalObjetivo = siguiente_caudal();
  sigma = siguiente_tiempo();
}

// Defino la delta externa
void generador_ordenes::dext(Event event, double t) {}

// Defino lambda
Event generador_ordenes::lambda(double t) { return Event(&caudalObjetivo, 0); }

// Defino la funcion de salida
void generador_ordenes::exit() {}

// Funcion estocastica
double generador_ordenes::siguiente_caudal() {
  double caudales[] = {80, 0, 100};
  double valor = caudales[indice % 3];
  indice++;
  return valor;
}

// Funcion estocastica
double generador_ordenes::siguiente_tiempo() { return 10.0; }
