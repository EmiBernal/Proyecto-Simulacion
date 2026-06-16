// actuador de la bomba
#if !defined ACTUADOR_BOMBA_HPP
#define ACTUADOR_BOMBA_HPP

#include "event.h"
#include "simulator.h"
#include "stdarg.h"

class actuador_bomba : public Simulator {
  double valor_actual_bomba;
  double valor_entrante;
  double sigma;

public:
  actuador_bomba(const char *n) : Simulator(n) {};
  void init(double t, ...);
  double ta(double t);
  void dint(double);
  void dext(Event, double);
  Event lambda(double);
  void exit();
};

#endif
