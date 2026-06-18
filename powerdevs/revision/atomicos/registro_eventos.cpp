#include "registro_eventos.h"
#include <cmath>
#include <fstream>

const int ALARMA_BAJA_PUERTO = 0;
const int ALARMA_MEDIA_PUERTO = 1;
const int ALARMA_CRITICA_PUERTO = 2;
const int DETENER_BOMBA_PUERTO = 3;
const int CONFIRMACION_ENFERMERO_PUERTO = 4;
const int FIN_BOLSA_PUERTO = 5;
const int AJUSTAR_CAUDAL_PUERTO = 6;

const char *LOG_FILE =
    "/home/emi/programacion/unrc/carrera/2026/1erCuatri/proyectos/"
    "proyecto_simulacion_prom/powerdevs/registro_eventos.txt";

void registro_eventos::init(double t, ...) {
  cantAlarmasBajas = 0;
  cantAlarmasMedia = 0;
  cantAlarmasCriticas = 0;
  cantDetenciones = 0;
  sigma = INFINITY;

  std::ofstream log(LOG_FILE);
  log << "Registro de eventos\n";
  log.close();
}

double registro_eventos::ta(double t) { return sigma; }

void registro_eventos::dint(double t) { sigma = INFINITY; }

void registro_eventos::dext(Event x, double t) {
  std::ofstream log(LOG_FILE, std::ios::app);

  if (x.port == ALARMA_BAJA_PUERTO) {
    cantAlarmasBajas++;
    log << "t=" << t << " Alarma baja registrada\n";
  }

  if (x.port == ALARMA_MEDIA_PUERTO) {
    cantAlarmasMedia++;
    log << "t=" << t << " Alarma media registrada\n";
  }

  if (x.port == ALARMA_CRITICA_PUERTO) {
    cantAlarmasCriticas++;
    log << "t=" << t << " Alarma critica registrada\n";
  }

  if (x.port == DETENER_BOMBA_PUERTO) {
    cantDetenciones++;
    log << "t=" << t << " Detencion de bomba registrada\n";
  }

  if (x.port == AJUSTAR_CAUDAL_PUERTO) {
    double caudal = *(double *)x.value;
    log << "t=" << t << " Ajuste de caudal registrado: " << caudal << "\n";
  }

  if (x.port == CONFIRMACION_ENFERMERO_PUERTO) {
    double v = *(double *)x.value;

    if (v == 1) {
      log << "t=" << t << " Confirmacion de enfermero registrada\n";
    }
  }

  if (x.port == FIN_BOLSA_PUERTO) {
    double v = *(double *)x.value;

    if (v == 1) {
      log << "t=" << t << " Fin de bolsa registrado\n";
    }
  }

  sigma = INFINITY;
  log.close();
}

Event registro_eventos::lambda(double t) {
  static double valor = 0;
  return Event(&valor, 0);
}

void registro_eventos::exit() {
  std::ofstream log(LOG_FILE, std::ios::app);

  log << "\n--- RESUMEN ---\n";
  log << "Alarmas bajas: " << cantAlarmasBajas << "\n";
  log << "Alarmas medias: " << cantAlarmasMedia << "\n";
  log << "Alarmas criticas: " << cantAlarmasCriticas << "\n";
  log << "Detenciones: " << cantDetenciones << "\n";

  log.close();
}
