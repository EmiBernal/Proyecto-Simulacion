// controlador de la bomba
#if !defined CONTROLADOR_BOMBA_HPP
#define CONTROLADOR_BOMBA_HPP

#include "event.h"
#include "simulator.h"
#include "stdarg.h"

// Defino el enumerado Fase
enum Fase { DETENIDA = 0, INFUNDIENDO = 1, CORRIGIENDO = 2 };

// Defino el enumerado del Estado bolsa
enum EstadoBolsa { BOLSA_NORMAL = 0, FIN_BOLSA = 1 };

// Defino el enumerado del Estado alarma
enum EstadoAlarma {
  SIN_ALARMA = 0,
  ALARMA_BAJA = 1,
  ALARMA_MEDIA = 2,
  ALARMA_CRITICA = 3
};

class controlador_bomba : public Simulator {
  Fase fase;
  double caudalObjetivo;
  double caudalReal;
  EstadoBolsa estadoBolsa;
  EstadoAlarma estadoAlarma;
  double tiempo_desvio;
  double tiempo_bolsa;
  double sigma;
  double last_t; // Para poder calcular el e

public:
  controlador_bomba(const char *n) : Simulator(n) {};
  void init(double t, ...);
  double ta(double t);
  void dint(double);
  void dext(Event, double);
  Event lambda(double);
  void exit();
};

#endif
