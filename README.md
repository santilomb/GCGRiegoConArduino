# GCGRiegoConArduino
Proyecto con el grupo del trabajo para generar un regador que posee 3 circuitos independientes, una bomba, un tanque con sensores de nivel alto y bajo y alguna otra cosita. Se ira definiendo ya que es un proyecto que arranco para aprender y sin estar del todo cerrado.

## Actualmente hay dos abordajes diferentes

- [GCGRiegoConArduino.ino](https://github.com/santilomb/GCGRiegoConArduino/blob/master/GCGRiegoConArduino.ino "GCGRiegoConArduino.ino") (Dic 2019): Este es script del abordaje original con 3 circuitos, bomba, sensores de humedad, y sensores de nivel de tanque de agua. Con el pasar del tiempo los sensores empezaron a fallar y surgio el segundo abordaje.

- [GCGRiegoConArduino_simplificado.ino](https://github.com/santilomb/GCGRiegoConArduino/blob/master/GCGRiegoConArduino_simplificado.ino "GCGRiegoConArduino_simplificado.ino") (Nov 2020): En esta segunda solucion se quitaron los sensores de humedad dañados y los sensores de tanque y se agrego un sensor de luz. 
La maquina de estados que posee el programa luego de iniciar queda en estado LIBRE donde verifica el sensor de luz y un boton. Si alguno de los dos enciende pasa a estado REGANDO donde cada circuito se riega de forma independiente para mantener la presión de la bomba. El tiempo de REGANDO de cada circuito ese establece de forma independiente. 
Una vez que termino la etapa de REGANDO pasa a una instancia de REGADO donde NO se sensa el sensor de Luz y solo se puede activar el riego mediante el botn. Esta estapa REGADO dura 72000seg (20hs). La idea es prevenir que el sensor de luz active mas de una vez al dia el riego


Simulación en tinkercad.com de la primer idea
https://www.tinkercad.com/things/kisXjxzuSdu-gcgriegoconarduino/


![Diagrama del circuito de la primer idea](https://github.com/santilomb/GCGRiegoConArduino/blob/master/diagrama.png)


