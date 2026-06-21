# Código fuente — Controlador Simplificado de Bomba de Infusión

Implementación de los modelos atómicos DEVS en PowerDEVS (C++).
La descripción completa de la implementación, decisiones de diseño
y consideraciones relevantes se encuentra en la Sección 10 del informe
(`informe.pdf`).

## Herramientas utilizadas
- PowerDEVS (simulador de eventos discretos basado en DEVS)
- C++ (atómicos implementados según la especificación de la Sección 8)
- Python (GnuPlot para los graficos)

## Estructura
- `generador_ordenes.{h,cpp}` — entrada de ordenes medicas al sistema
- `controlador_bomba.{h,cpp}` — componente central de decisión
- `actuador_bomba.{h,cpp}` — actuación física sobre el caudal
- `sensor_flujo.{h,cpp}` — muestreo periódico del caudal
- `modulo_alarmas.{h,cpp}` — gestión y repetición de notificaciones
- `registro_eventos.{h,cpp}` — historial de eventos del sistema
- `bolsa.{h,cpp}` — consumo progresivo de medicación
- `enfermero.{h,cpp}` — simulación de respuesta del personal médico

## Instalación
Copiar los archivos `.h`/`.cpp` a `/PowerDEVS/atomics/bomba/`
(crear la carpeta si no existe) y reconstruir el modelo desde PowerDEVS.
