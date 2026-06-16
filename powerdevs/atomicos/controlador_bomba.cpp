#include "controlador_bomba.h"
#include "event.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

// Defino los puertos de entrada
const int ORDEN_MEDICA = 0;
const int SENSOR_FLUJO = 1;
const int FIN_BOLSA_PUERTO = 2;
const int CONFIRMACION_ENFERMERO = 3;

// Defino los puertos de salida
const int AJUSTAR_CAUDAL = 0;
const int DETENER_BOMBA = 1;
const int ALARMA_BAJA_PUERTO = 2;
const int ALARMA_MEDIA_PUERTO = 3;
const int ALARMA_CRITICA_PUERTO = 4;

// Inicializacion
void controlador_bomba::init(double t, ...) {
  fase = DETENIDA;
  caudalObjetivo = 0;
  caudalReal = 0;
  estadoBolsa = BOLSA_NORMAL;
  estadoAlarma = SIN_ALARMA;
  tiempo_desvio = 0;
  tiempo_bolsa = 0;
  last_t = t;
  sigma = INFINITY;
};

// Defino el ta
double controlador_bomba::ta(double t) { return sigma; }

// Defino la delta interna
void controlador_bomba::dint(double t) {

  // Luego de ordenar ajustar caudal, la bomba queda INFUNDIENDO
  if (fase == INFUNDIENDO && caudalObjetivo > 0) {
    fase = INFUNDIENDO;
    estadoBolsa = BOLSA_NORMAL;
    tiempo_desvio = 0;
    sigma = INFINITY;
  }

  // Luego de ordenar detener bomba, la bomba queda DETENIDA
  if (fase == DETENIDA && caudalObjetivo == 0) {
    fase = DETENIDA;
    caudalObjetivo = 0;
    estadoAlarma = SIN_ALARMA;
    tiempo_desvio = 0;
    sigma = INFINITY;
  }

  // Despues de emitir alarma media
  if (fase == CORRIGIENDO && estadoAlarma == SIN_ALARMA) {
    fase = CORRIGIENDO;
    estadoAlarma = ALARMA_MEDIA;
    tiempo_desvio = 0;
    sigma = 5.0;
  }

  // Despues de emitir alarma critica
  if (fase == CORRIGIENDO && estadoAlarma == ALARMA_MEDIA) {
    fase = DETENIDA;
    estadoAlarma = ALARMA_CRITICA;
    tiempo_desvio = 0;
    sigma = INFINITY;
  }

  // Luego de emitir alarmaBaja por fin de bolsa
  if (estadoBolsa == FIN_BOLSA && estadoAlarma == ALARMA_BAJA &&
      tiempo_bolsa == 0) {
    // fase = fase;
    estadoBolsa = FIN_BOLSA;
    estadoAlarma = ALARMA_BAJA;
    // tiempo_desvio = tiempo_desvio
    tiempo_bolsa = 60.0;
    sigma = 60.0;
  }

  // Luego de emitir detenerBomba por fin de bolsa
  if (estadoBolsa == FIN_BOLSA && estadoAlarma == ALARMA_BAJA &&
      tiempo_bolsa == 60.0) {
    fase = DETENIDA;
    caudalObjetivo = 0;
    estadoBolsa = FIN_BOLSA;
    estadoAlarma = ALARMA_BAJA;
    tiempo_bolsa = 60.0;
    sigma = INFINITY;
  }
}

// Defino la delta externa
void controlador_bomba::dext(Event x, double t) {

  // En el caso de llegar una orden medica
  if (x.port == ORDEN_MEDICA) {

    // Nueva orden medica mayor a 0
    if (*(double *)x.value > 0) {
      fase = INFUNDIENDO;
      caudalObjetivo = *(double *)x.value;
      estadoAlarma = SIN_ALARMA;
      tiempo_desvio = 0;
      sigma = 0;
    }

    // Nueva orden medica igual a 0
    if (*(double *)x.value == 0) {
      fase = DETENIDA;
      caudalObjetivo = 0;
      estadoAlarma = SIN_ALARMA;
      tiempo_desvio = 0;
      sigma = 0;
    }
  }

  // En el caso de llegar un valor nuevo del sensor flujo
  if (x.port == SENSOR_FLUJO) {

    // El desvio continua antes de emitir una alarma media
    if ((abs(*(double *)x.value - caudalObjetivo) > 0.1 * caudalObjetivo) &&
        fase == CORRIGIENDO && estadoAlarma == SIN_ALARMA) {
      double e = t - last_t;
      fase = CORRIGIENDO;
      caudalReal = *(double *)x.value;
      tiempo_desvio = tiempo_desvio + e;
      sigma = std::max(0.0, 5.0 - tiempo_desvio);
      last_t = t;
    }

    // Primer desvio mayor al 10%
    if ((abs(*(double *)x.value - caudalObjetivo) > 0.1 * caudalObjetivo) &&
        fase != CORRIGIENDO) {
      fase = CORRIGIENDO;
      caudalReal = *(double *)x.value;
      tiempo_desvio = 0;
      sigma = 5.0;
    }

    // Caudal dentro del rango permitido
    if ((abs(*(double *)x.value - caudalObjetivo) <= 0.1 * caudalObjetivo)) {
      fase = INFUNDIENDO;
      caudalReal = *(double *)x.value;
      estadoAlarma = SIN_ALARMA;
      tiempo_desvio = 0;
      sigma = INFINITY;
    }

    // Sigue desviando luego de alarma media
    if ((abs(*(double *)x.value - caudalObjetivo) > 0.1 * caudalObjetivo) &&
        estadoAlarma == ALARMA_MEDIA) {
      double e = t - last_t;
      fase = CORRIGIENDO;
      caudalReal = *(double *)x.value;
      tiempo_desvio = tiempo_desvio + e;
      sigma = std::max(0.0, 5 - tiempo_desvio);
      last_t = t;
    }
  }

  // En el caso de llegar el estado de bolsa
  if (x.port == FIN_BOLSA_PUERTO) {
    double v = *(double *)x.value;

    if (v == 1) {
      estadoBolsa = FIN_BOLSA;
      estadoAlarma = ALARMA_BAJA;
      tiempo_bolsa = 0;
      sigma = 0;
    }
  }

  // En el caso de llegar confirmacion del enfermero
  if (x.port == CONFIRMACION_ENFERMERO) {
    double v = *(double *)x.value;

    // Ignoro el valor inicial 0 del Step
    if (v != 1) {
      return;
    }

    // Confirma alarma critica, limpia la alarma y sigue detenida
    if (estadoAlarma == ALARMA_CRITICA && fase == DETENIDA) {
      fase = DETENIDA;
      estadoAlarma = SIN_ALARMA;
      sigma = INFINITY;
    } else {
      estadoAlarma = SIN_ALARMA;
      sigma = INFINITY;
    }
  }
}

// Defino lambda
Event controlador_bomba::lambda(double t) {
  static double valor;

  // Se emite la alarma baja cuando se detecta el fin de la bolsa
  if (estadoBolsa == FIN_BOLSA && estadoAlarma == ALARMA_BAJA &&
      tiempo_bolsa == 0) {
    valor = 1; // no interesa el valor
    return Event(&valor, ALARMA_BAJA_PUERTO);
  }

  // Se detiene la bomba al cumplirse los 60 segundos
  if (estadoBolsa == FIN_BOLSA && estadoAlarma == ALARMA_BAJA &&
      tiempo_bolsa == 60.0) {
    valor = 1; // no interesa el valor
    return Event(&valor, DETENER_BOMBA);
  }

  // Se emite la alarma baja cuando se detecta el fin de la bolsa
  if (fase == INFUNDIENDO && caudalObjetivo > 0 && estadoAlarma == SIN_ALARMA) {
    valor = caudalObjetivo;
    return Event(&valor, AJUSTAR_CAUDAL);
  }

  // Detener por orden medica con caudal = 0
  if (fase == DETENIDA && caudalObjetivo == 0) {
    valor = 1;
    return Event(&valor, DETENER_BOMBA);
  }

  // Desvio persistente mayor al 10
  if (fase == CORRIGIENDO && estadoAlarma == SIN_ALARMA) {
    valor = 1;
    return Event(&valor, ALARMA_MEDIA_PUERTO);
  }

  // Desvio no corregido luego de alarma medica
  if (fase == CORRIGIENDO && estadoAlarma == ALARMA_MEDIA) {
    valor = 1;
    return Event(&valor, ALARMA_CRITICA_PUERTO);
  }

  valor = 0;
  return Event(&valor, 0); // salida por defecto
}

void controlador_bomba::exit() {}
