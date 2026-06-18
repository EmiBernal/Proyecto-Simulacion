#ifndef BOLSA_H
#define BOLSA_H

#include "event.h"
#include "simulator.h"
#include "stdarg.h"

class bolsa : public Simulator {
private:
  // Parametros
  double volumenInicial; // ml
  double umbralFinBolsa; // ml
  double periodo;        // segundos

  // Estado
  double volumenActual; // ml
  double caudalActual;  // ml/h
  double sigma;
  double last_t;
  bool finBolsaEmitido;

  // Funciones auxiliares
  void actualizarVolumen(double dt);

public:
  bolsa(const char *n) : Simulator(n) {};
  void init(double, ...);
  double ta(double t);
  void dint(double t);
  void dext(Event x, double t);
  Event lambda(double t);
  void exit();
};

#endif
