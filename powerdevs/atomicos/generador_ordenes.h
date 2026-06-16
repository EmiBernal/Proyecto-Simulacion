// generador de ordenes medicas
#if !defined GENERADOR_ORDENES_HPP
#define GENERADOR_ORDENES_HPP

#include "event.h"
#include "simulator.h"
#include "stdarg.h"

class generador_ordenes : public Simulator {
  double caudalObjetivo;
  double sigma;
  int indice;

  double siguiente_caudal();
  double siguiente_tiempo();

public:
  generador_ordenes(const char *n) : Simulator(n) {};
  void init(double t, ...);
  double ta(double t);
  void dint(double);
  void dext(Event, double);
  Event lambda(double);
  void exit();
};

#endif
