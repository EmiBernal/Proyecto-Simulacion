#include "controlador_bomba.h"
#include "event.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

const int ORDEN_MEDICA = 0;
const int SENSOR_FLUJO = 1;
const int FIN_BOLSA_PUERTO = 2;
const int CONFIRMACION_ENFERMERO = 3;

const int AJUSTAR_CAUDAL = 0;
const int DETENER_BOMBA = 1;
const int ALARMA_BAJA_PUERTO = 2;
const int ALARMA_MEDIA_PUERTO = 3;
const int ALARMA_CRITICA_PUERTO = 4;

void controlador_bomba::init(double t, ...) {
  fase = DETENIDA;
  caudalObjetivo = 0;
  caudalReal = 0;
  estadoBolsa = BOLSA_NORMAL;
  estadoAlarma = SIN_ALARMA;
  tiempo_desvio = 0;
  last_t = t;
  sigma = INFINITY;
  alarmaBajaEmitida = false;
  detencionProgramada = false;
}

double controlador_bomba::ta(double t) { return sigma; }

void controlador_bomba::dint(double t) {

  if (estadoBolsa == FIN_BOLSA && !alarmaBajaEmitida) {
    alarmaBajaEmitida = true;
    sigma = 60.0;
    last_t = t;
    return;
  }

  if (estadoBolsa == FIN_BOLSA && alarmaBajaEmitida && !detencionProgramada) {
    fase = DETENIDA;
    caudalObjetivo = 0;
    detencionProgramada = true;
    sigma = INFINITY;
    last_t = t;
    return;
  }

  if (fase == CORRIGIENDO && estadoAlarma == SIN_ALARMA) {
    fase = CORRIGIENDO;
    estadoAlarma = ALARMA_MEDIA;
    tiempo_desvio = 0;
    sigma = 5.0;
    last_t = t;
    return;
  }

  if (fase == CORRIGIENDO && estadoAlarma == ALARMA_MEDIA) {
    fase = DETENIDA;
    caudalObjetivo = 0;
    estadoAlarma = ALARMA_CRITICA;
    tiempo_desvio = 0;
    sigma = 0;
    last_t = t;
    return;
  }

  if (fase == DETENIDA && caudalObjetivo == 0) {
    fase = DETENIDA;
    caudalObjetivo = 0;

    if (estadoAlarma != ALARMA_CRITICA) {
      estadoAlarma = SIN_ALARMA;
    }

    tiempo_desvio = 0;
    sigma = INFINITY;
    last_t = t;
    return;
  }

  if (fase == INFUNDIENDO && caudalObjetivo > 0) {
    fase = INFUNDIENDO;
    estadoBolsa = BOLSA_NORMAL;
    tiempo_desvio = 0;
    sigma = INFINITY;
    last_t = t;
    return;
  }
}

void controlador_bomba::dext(Event x, double t) {

  if (x.port == ORDEN_MEDICA) {
    double orden = *(double *)x.value;

    if (orden > 0) {
      fase = INFUNDIENDO;
      caudalObjetivo = orden;
      estadoAlarma = SIN_ALARMA;
      tiempo_desvio = 0;
      last_t = t;
      sigma = 0;
      return;
    }

    if (orden == 0) {
      fase = DETENIDA;
      caudalObjetivo = 0;
      estadoAlarma = SIN_ALARMA;
      tiempo_desvio = 0;
      last_t = t;
      sigma = 0;
      return;
    }
  }

  if (x.port == SENSOR_FLUJO) {
    double medicion = *(double *)x.value;
    double e = t - last_t;

    if (e < 0.0) {
      e = 0.0;
    }

    caudalReal = medicion;

    if (estadoBolsa == FIN_BOLSA && alarmaBajaEmitida && !detencionProgramada) {
      if (!std::isinf(sigma)) {
        sigma = std::max(0.0, sigma - e);
      }
      last_t = t;
      return;
    }

    bool hayDesvio =
        (caudalObjetivo > 0) &&
        (std::abs(medicion - caudalObjetivo) > 0.1 * caudalObjetivo);

    if (estadoAlarma == ALARMA_CRITICA) {
      last_t = t;
      return;
    }

    if (estadoAlarma == ALARMA_MEDIA) {
      if (hayDesvio) {
        if (!std::isinf(sigma)) {
          sigma = std::max(0.0, sigma - e);
        }

        fase = CORRIGIENDO;
        last_t = t;
        return;
      }

      if (caudalObjetivo > 0) {
        fase = INFUNDIENDO;
      } else {
        fase = DETENIDA;
      }

      estadoAlarma = SIN_ALARMA;
      tiempo_desvio = 0;
      sigma = INFINITY;
      last_t = t;
      return;
    }

    if (hayDesvio && estadoAlarma == SIN_ALARMA && fase == CORRIGIENDO) {
      tiempo_desvio = tiempo_desvio + e;
      sigma = std::max(0.0, 5.0 - tiempo_desvio);
      last_t = t;
      return;
    }

    if (hayDesvio && estadoAlarma == SIN_ALARMA && fase != CORRIGIENDO) {
      fase = CORRIGIENDO;
      tiempo_desvio = 0;
      sigma = 5.0;
      last_t = t;
      return;
    }

    if (!hayDesvio && estadoAlarma == SIN_ALARMA) {
      if (caudalObjetivo > 0) {
        fase = INFUNDIENDO;
      } else {
        fase = DETENIDA;
      }

      tiempo_desvio = 0;
      sigma = INFINITY;
      last_t = t;
      return;
    }
  }

  if (x.port == FIN_BOLSA_PUERTO) {
    double v = *(double *)x.value;

    if (v == 1) {
      estadoBolsa = FIN_BOLSA;
      estadoAlarma = ALARMA_BAJA;
      alarmaBajaEmitida = false;
      detencionProgramada = false;
      last_t = t;
      sigma = 0;
      return;
    }
  }

  if (x.port == CONFIRMACION_ENFERMERO) {
    double v = *(double *)x.value;

    if (v != 1) {
      return;
    }

    if (estadoAlarma == ALARMA_CRITICA && fase == DETENIDA) {
      fase = DETENIDA;
      caudalObjetivo = 0;
      estadoAlarma = SIN_ALARMA;
      tiempo_desvio = 0;
      sigma = INFINITY;
      last_t = t;
      return;
    }

    if (estadoBolsa == FIN_BOLSA && alarmaBajaEmitida && !detencionProgramada) {
      estadoAlarma = SIN_ALARMA;
      tiempo_desvio = 0;
      double e = t - last_t;

      if (e < 0.0) {
        e = 0.0;
      }

      if (!std::isinf(sigma)) {
        sigma = std::max(0.0, sigma - e);
      }

      last_t = t;
      return;
    }

    estadoAlarma = SIN_ALARMA;
    tiempo_desvio = 0;
    sigma = INFINITY;
    last_t = t;
    return;
  }
}

Event controlador_bomba::lambda(double t) {
  static double valor;

  if (estadoBolsa == FIN_BOLSA && !alarmaBajaEmitida) {
    valor = 1;
    return Event(&valor, ALARMA_BAJA_PUERTO);
  }

  if (estadoBolsa == FIN_BOLSA && alarmaBajaEmitida && !detencionProgramada) {
    valor = 1;
    return Event(&valor, DETENER_BOMBA);
  }

  if (fase == INFUNDIENDO && caudalObjetivo > 0 && estadoAlarma == SIN_ALARMA &&
      estadoBolsa != FIN_BOLSA) {
    valor = caudalObjetivo;
    return Event(&valor, AJUSTAR_CAUDAL);
  }

  if (fase == CORRIGIENDO && estadoAlarma == SIN_ALARMA) {
    valor = 1;
    return Event(&valor, ALARMA_MEDIA_PUERTO);
  }

  if (fase == CORRIGIENDO && estadoAlarma == ALARMA_MEDIA) {
    valor = 1;
    return Event(&valor, ALARMA_CRITICA_PUERTO);
  }

  if (fase == DETENIDA && caudalObjetivo == 0) {
    valor = 1;
    return Event(&valor, DETENER_BOMBA);
  }

  valor = 0;
  return Event(&valor, 0);
}

void controlador_bomba::exit() {}
