// sensor flujo
#if !defined SENSOR_FLUJO_HPP
#define SENSOR_FLUJO_HPP

#include "event.h"     // Representa eventos de entrada/salida
#include "simulator.h" // Simulator es la clase padre de los atomicos
#include "stdarg.h"

class sensor_flujo : public Simulator {
  double caudal;
  double sigma;
  double last_t;

public:
  sensor_flujo(const char *n) : Simulator(n) {};
  void init(double t, ...);
  double ta(double t);
  void dint(double);
  void dext(Event, double);
  Event lambda(double);
  void exit();
};

#endif
