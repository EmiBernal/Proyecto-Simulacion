// modulo de alarmas
#if !defined MODULO_ALARMAS_HPP
#define MODULO_ALARMAS_HPP

#include "event.h"     // Representa eventos de entrada/salida
#include "simulator.h" // Simulator es la clase padre de los atomicos
#include "stdarg.h"

// Defino el enumerado Fase
enum FaseAlarma {
  PASIVO = 0,
  NOTIFICANDO = 1,
  ESPERANDO_CONFIRMACION = 2,
  REPITIENDO = 3
};

// Defino el enumerado Tipo
enum Tipo { NINGUNA = 0, BAJA = 1, MEDIA = 2, CRITICA = 3 };

class modulo_alarmas : public Simulator {
  FaseAlarma fase;
  Tipo tipo;
  double sigma;
  double last_t;

public:
  modulo_alarmas(const char *n) : Simulator(n) {};
  void init(double t, ...);
  double ta(double t);
  void dint(double);
  void dext(Event, double);
  Event lambda(double);
  void exit();
};

#endif
