# Raydungeon
Una aplicación gráfica enfocada al análisis de algoritmos de generación mazmorras
## Instrucciones de uso
Desde la pantalla de inicio de la aplicación se puede seleccionar desde la lista de algoritmos de la izquierda el algoritmo a probar.

Elegir este algoritmo añadirá una pestalla especifica para el algoritmo. En esa pestaña se puede ajustar los parametros de ejecución del mismo. 

Haciendo click en el botón *Run* se ejecutará el algoritmo. Por defecto usará unos parametros por defecto. Para cambiar el valor de los parametros a ejecutar se puede hacer click en la *checkbox* y se puede seleccionar un valor en la casilla que tiene a la derecha. Para saber que hace cada parametro, se puede hacer *hover* sobre el icono *(?)* y se motrará una descripción.

Es recomendable pulsar la *checkbox* de *Resolve*, esta opción ajusta si calcular un camino usando A* entre la entrada y la salida de la mazmorra.

Haciendo uso del botón *Batch run* se pueden generar lotes de mazmorras usando los mismos parametros.

Al lado de los parametros, una vez ejecutado una algoritmo, se mostrarán sus estadísticas. Si se hace uso de la generación por lotes, se puede ver una tabla con los valores de todas las generaciones.

Cuando ejecute un algoritmo, se puede hacer desmarcar el *checkbox* *Hide view* para mostrar la mazmorra generada. En esta sección se puede ver tanto la mazmorra, como el punto de inicio y fin, y si se ha calculado, el camino entre ambos puntos. Además si se ha usado la generación por lotes, se pueden ver distintos mapas de densidad haciendo click en el desplegable *View mode*.

Hay cuatro mapas de densidad disponibles. El mapa de densidad de mazmorra, muestra la cantidad de casillas generadas con suelo. El mapa de densidad de caminos, muestra la cantidad de caminos que se han generado en una casilla. Y los mapas de densidad de entradas y salidas, que muestran la cantidad de entradas y salidas en cada casilla. Estos mapas muestran un gráfico 3D indicando la cantidad de estos sucesos en cada casilla en el total de generaciones indicadas a la hora de ejecutar la generación por lotes.

## Instalación
En la sección de releases, se puede descargar un comprimido con el binario y la estructura de carpetas. Ejecutando el binario se puede abrir la aplicación

## Construcción
El único requisito de la aplicación es *Raylib 5.1*, en el momento de escritura, no existe un *release* de la versión 5.1 de esta librería, por esa razón es necesario construirlo manualmente, se puede hacer siguiendo los pasos en el respositorio oficial (https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux).


1. Clonar el respositotio

```
git clone https://github.com/YarasAtomic/dungeonAnalysis
```

2. Acceder al directorio
```
cd dungeonAnalysis/code
```

3. Compilar
```
make
```