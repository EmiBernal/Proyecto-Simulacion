#include "bolsa.h"

void bolsa::init(double t, ...) {
  va_list parameters;
  va_start(parameters, t);

  // Parametros configurables
  volumenInicial = va_arg(parameters, double);
  umbralFinBolsa = va_arg(parameters, double);
  periodo = va_arg(parameters, double);
  va_end(parameters);

  // Estado inicial
  volumenActual = volumenInicial;
  caudalActual = 0.0;
  finBolsaEmitido = false;
  last_t = t;

  // La bolsa se actualiza cada periodo segundos
  sigma = periodo;
}

double bolsa::ta(double t) { return sigma; }

void bolsa::actualizarVolumen(double dt) {
  // caudalActual esta en ml/h
  // dt esta en segundos

  double consumo = caudalActual * dt / 3600.0;

  // consumo quedo en ml/h
  volumenActual = volumenActual - consumo;

  // No negativo
  if (volumenActual < 0.0) {
    volumenActual = 0.0;
  }
}

void bolsa::dint(double t) {
  // Tiempo transcurrido desde la ultima actualizacion
  double delta_t = t - last_t;

  actualizarVolumen(delta_t);

  last_t = t;

  // Sigue actualizando cada periodo segundos
  sigma = periodo;
}

void bolsa::dext(Event x, double t) {
  // Tiempo transcurrido desde la ultima actualizacion
  double delta_t = t - last_t;

  actualizarVolumen(delta_t);

  last_t = t;

  // Entrada 0: nuevo caudal actual desde el actuador
  if (x.port == 0) {
    caudalActual = *(double *)x.value;
  }

  sigma = periodo;
}

Event bolsa::lambda(double t) {
  // Si la bolsa llego al umbral y todavia no se aviso
  if (volumenActual <= umbralFinBolsa && !finBolsaEmitido) {
    finBolsaEmitido = true;

    static double fin = 1.0;
    return Event(&fin, 0); // salida 0: finBolsa
  }

  return Event(&volumenActual, 1); // salida 1: volumenBolsa
}

void bolsa::exit() {}
