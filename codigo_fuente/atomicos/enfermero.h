#if !defined ENFERMERO_HPP
#define ENFERMERO_HPP

#include "event.h"
#include "simulator.h"
#include "stdarg.h"

class enfermero : public Simulator {
private:
  // Parametros
  int modo;            // 0 deterministico, 1 uniforme, 2 poisson/exponencial
  double tiempoFijo;   // deterministico
  double minTiempo;    // uniforme
  double maxTiempo;    // uniforme
  double lambdaTiempo; // exponencial

  // Estado
  double sigma;
  bool yendo;

  // Salida
  double confirmacion;

  // Funciones auxiliares
  double siguiente_tiempo();
  double uniforme(double a, double b);
  double exponencial(double lambdaTiempo);

public:
  enfermero(const char *n) : Simulator(n) {};
  void init(double t, ...);
  double ta(double t);
  void dint(double t);
  void dext(Event x, double t);
  Event lambda(double t);
  void exit();
};

#endif
