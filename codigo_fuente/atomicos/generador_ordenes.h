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

  // Defino los parametros para las distribuciones que use
  int modo;            // 0 deterministico, 1 uniforme, 2 poisson/exponencial
  double tiempoFijo;   // Deterministico
  double minTiempo;    // Uniforme
  double maxTiempo;    // Uniforme
  double lambdaTiempo; // Para usar en la exponencial

  double siguiente_caudal();
  double siguiente_tiempo();

  double uniforme(double a, double b);
  double exponencial(double lambda);

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
