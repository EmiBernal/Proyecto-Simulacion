#include "sensor_flujo.h"
#include <algorithm>

// Inicializacion
void sensor_flujo::init(double t, ...) {
  va_list parameters;
  va_start(parameters, t);

  modoFalla = (int)va_arg(parameters, double);
  factorFalla = va_arg(parameters, double);

  va_end(parameters);

  caudal = 0;
  sigma = 1;
  last_t = t;
}

// Defino el ta
double sensor_flujo::ta(double t) { return sigma; }

// Defino la delta interna
void sensor_flujo::dint(double t) { sigma = 1; }

// Defino la delta externa
void sensor_flujo::dext(Event x, double t) {
  double e = t - last_t;
  double valor = *(double *)x.value;

  if (modoFalla == 1) {
    caudal = valor * factorFalla;
  } else {
    caudal = valor;
  }

  sigma = std::max(0.0, sigma - e);
  last_t = t;
}

// Defino el lambda
Event sensor_flujo::lambda(double t) { return Event(&caudal, 0); }

void sensor_flujo::exit() {}
