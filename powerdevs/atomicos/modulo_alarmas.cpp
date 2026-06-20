#include "modulo_alarmas.h"
#include "event.h"
#include <cmath>

// Defino los puertos de entrada
const int ALARMA_BAJA_PUERTO = 0;
const int ALARMA_MEDIA_PUERTO = 1;
const int ALARMA_CRITICA_PUERTO = 2;
const int CONFIRMACION_ENFERMERO_PUERTO = 3;

// Defino el puerto de salida
const int NOTIFICACION_ALARMA = 0;

// Inicializacion
void modulo_alarmas::init(double t, ...) {
  fase = PASIVO;
  tipo = NINGUNA;
  sigma = INFINITY;
  last_t = t;
}

// Defino el ta
double modulo_alarmas::ta(double t) { return sigma; }

// Defino la delta interna
void modulo_alarmas::dint(double t) {

  if (fase == NOTIFICANDO && tipo == BAJA) {
    fase = PASIVO;
    tipo = NINGUNA;
    sigma = INFINITY;
    return;
  }

  if (fase == NOTIFICANDO && tipo == MEDIA) {
    fase = PASIVO;
    tipo = NINGUNA;
    sigma = INFINITY;
    return;
  }

  if (fase == NOTIFICANDO && tipo == CRITICA) {
    fase = ESPERANDO_CONFIRMACION;
    tipo = CRITICA;
    sigma = 30.0;
    return;
  }

  if (fase == ESPERANDO_CONFIRMACION && tipo == CRITICA) {
    fase = REPITIENDO;
    tipo = CRITICA;
    sigma = 10.0;
    return;
  }

  if (fase == REPITIENDO && tipo == CRITICA) {
    fase = REPITIENDO;
    tipo = CRITICA;
    sigma = 10.0;
    return;
  }
}

// Defino la delta externa
void modulo_alarmas::dext(Event x, double t) {

  // En el caso de llegar una alarma baja
  if (x.port == ALARMA_BAJA_PUERTO) {
    fase = NOTIFICANDO;
    tipo = BAJA;
    sigma = 0;
    return;
  }

  // En el caso de llegar una alarma media
  if (x.port == ALARMA_MEDIA_PUERTO) {
    fase = NOTIFICANDO;
    tipo = MEDIA;
    sigma = 0;
    return;
  }

  // En el caso de llegar una alarma critica
  if (x.port == ALARMA_CRITICA_PUERTO) {
    fase = NOTIFICANDO;
    tipo = CRITICA;
    sigma = 0;
    return;
  }

  // En el caso de llegar la confirmacion de un enfermero
  if (x.port == CONFIRMACION_ENFERMERO_PUERTO) {
    double v = *(double *)x.value;

    if (v == 1) {
      fase = PASIVO;
      tipo = NINGUNA;
      sigma = INFINITY;
    }

    return;
  }
}

Event modulo_alarmas::lambda(double t) {
  static double valor;

  if (fase == NOTIFICANDO && tipo == BAJA) {
    valor = 1;
    return Event(&valor, NOTIFICACION_ALARMA);
  }

  if (fase == NOTIFICANDO && tipo == MEDIA) {
    valor = 2;
    return Event(&valor, NOTIFICACION_ALARMA);
  }

  if (fase == NOTIFICANDO && tipo == CRITICA) {
    valor = 3;
    return Event(&valor, NOTIFICACION_ALARMA);
  }

  if (fase == ESPERANDO_CONFIRMACION && tipo == CRITICA) {
    valor = 3;
    return Event(&valor, NOTIFICACION_ALARMA);
  }

  if (fase == REPITIENDO && tipo == CRITICA) {
    valor = 3;
    return Event(&valor, NOTIFICACION_ALARMA);
  }

  valor = 0;
  return Event(&valor, NOTIFICACION_ALARMA);
}

void modulo_alarmas::exit() {}
