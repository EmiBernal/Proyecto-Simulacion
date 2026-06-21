#include "actuador_bomba.h"
#include "event.h"
#include <cmath>

// Inicializacion
void actuador_bomba::init(double t, ...) {
  valor_actual_bomba = 0;
  valor_entrante = 0;
  sigma = INFINITY;
}

// Defino el ta
double actuador_bomba::ta(double t) { return sigma; }

// Defino la delta interna
void actuador_bomba::dint(double t) {
  valor_actual_bomba = valor_entrante;
  sigma = INFINITY;
}

// Defino la delta externa
void actuador_bomba::dext(Event x, double t) {
  // En el caso que tenga que ajustar el caudal
  if (x.port == 0) {
    valor_entrante = *(double *)x.value;
    sigma = 0.5;
  }

  // En el caso de detener la bomba
  if (x.port == 1) {
    valor_entrante = 0;
    sigma = 0.5;
  }
}

// Defino lambda
Event actuador_bomba::lambda(double t) { return Event(&valor_entrante, 0); }

// Defino la salida
void actuador_bomba::exit() {}
