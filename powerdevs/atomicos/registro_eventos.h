// registro de eventos
#if !defined REGISTRO_EVENTOS_HPP
#define REGISTRO_EVENTOS_HPP

#include "event.h"
#include "simulator.h"
#include "stdarg.h"

class registro_eventos : public Simulator {
  int cantAlarmasBajas;
  int cantAlarmasMedia;
  int cantAlarmasCriticas;
  int cantDetenciones;
  double sigma;

public:
  registro_eventos(const char *n) : Simulator(n) {};
  void init(double t, ...);
  double ta(double t);
  void dint(double);
  void dext(Event, double);
  Event lambda(double);
  void exit();
};
#endif
