#include "enfermero.h"
#include <cmath>
#include <cstdlib>

void enfermero::init(double t, ...) {
  va_list parameters;
  va_start(parameters, t);

  modo = (int)va_arg(parameters, double);
  tiempoFijo = va_arg(parameters, double);
  minTiempo = va_arg(parameters, double);
  maxTiempo = va_arg(parameters, double);
  lambdaTiempo = va_arg(parameters, double);

  va_end(parameters);

  sigma = INFINITY;
  yendo = false;
  confirmacion = 1.0;

  srand(5678); // semilla fija para repetir la simulacion
}

double enfermero::ta(double t) { return sigma; }

void enfermero::dint(double t) {
  // Luego de emitir confirmacion, vuelve a quedar libre
  yendo = false;
  sigma = INFINITY;
}

void enfermero::dext(Event x, double t) {
  if (x.port == 0 && !yendo) {
    double alarma = *(double *)x.value;

    if (alarma == 1.0) {
      yendo = true;
      sigma = siguiente_tiempo();
    }
  }
}

Event enfermero::lambda(double t) { return Event(&confirmacion, 0); }

void enfermero::exit() {}

double enfermero::siguiente_tiempo() {
  double tiempo;

  if (modo == 0) {
    tiempo = tiempoFijo;
  } else if (modo == 1) {
    tiempo = uniforme(minTiempo, maxTiempo);
  } else if (modo == 2) {
    tiempo = exponencial(lambdaTiempo);
  } else {
    tiempo = tiempoFijo;
  }

  if (tiempo <= 0.0) {
    tiempo = 1.0;
  }

  return tiempo;
}

double enfermero::uniforme(double a, double b) {
  double r = (double)rand() / (double)RAND_MAX;
  return a + r * (b - a);
}

double enfermero::exponencial(double lambdaTiempo) {
  if (lambdaTiempo <= 0.0) {
    return tiempoFijo;
  }

  double r = (double)rand() / (double)RAND_MAX;

  if (r <= 0.0) {
    r = 0.000001;
  }

  return -std::log(1.0 - r) / lambdaTiempo;
}
