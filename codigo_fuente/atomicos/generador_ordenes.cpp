#include "generador_ordenes.h"
#include "event.h"
#include <cmath>

// Inicializacion
void generador_ordenes::init(double t, ...) {
  va_list parameters;
  va_start(parameters, t);

  modo = (int)va_arg(parameters, double);
  tiempoFijo = va_arg(parameters, double);
  minTiempo = va_arg(parameters, double);
  maxTiempo = va_arg(parameters, double);
  lambdaTiempo = va_arg(parameters, double);

  va_end(parameters);

  caudalObjetivo = 100;
  sigma = 0;
  indice = 0;

  srand(1234); // Semilla fija para que se pueda repetir la simulacion
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
  if (modo == 0) {
    // Deterministico
    double caudales[] = {80, 120, 200, 120, 180};
    double valor = caudales[indice % 5];
    indice++;
    return valor;
  }
  // estocastico
  double caudales[] = {0, 50, 80, 100, 120};
  int cantidad = 5;
  int pos = rand() % cantidad;
  return caudales[pos];
}

// Tiempo hasta la siguiente orden
double generador_ordenes::siguiente_tiempo() {
  double tiempo;

  if (modo == 0) {
    tiempo = tiempoFijo;
  }

  if (modo == 1) {
    tiempo = uniforme(minTiempo, maxTiempo);
  }

  if (modo == 2) {
    tiempo = exponencial(lambdaTiempo);
  }

  if (tiempo <= 0.0) {
    tiempo = 1.0;
  }

  return tiempo;
}

// Uniforme entre a y b
double generador_ordenes::uniforme(double a, double b) {
  double r =
      (double)rand() / (double)RAND_MAX; // Numero aleatorio uniforme entre [0,]
  return a + r * (b - a);
}

// Exponencial con parametro lambda
double generador_ordenes::exponencial(double lambdaTiempo) {
  if (lambdaTiempo <= 0.0) {
    return tiempoFijo;
  }

  double r = (double)rand() / (double)RAND_MAX;

  if (r <= 0.0) {
    r = 0.000001;
  }

  return -std::log(1.0 - r) / lambdaTiempo;
}
