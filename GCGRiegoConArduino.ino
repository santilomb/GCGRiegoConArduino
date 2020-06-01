#define PIN_SENSOR_HUMEDAD_1 8
#define PIN_SENSOR_HUMEDAD_2 9
#define PIN_SENSOR_HUMEDAD_3 10
#define PIN_VALVULA_1 2
#define PIN_VALVULA_2 3
#define PIN_VALVULA_3 4
#define PIN_BOMBA 5
#define PIN_TANQUE_BAJO 11
#define PIN_TANQUE_ALTO 12
#define PIN_BOTON_1 7
#prueba GIT

enum Estados_enum {INICIANDO, LIBRE, SENSANDO, REGANDO};

unsigned long millisActuales;
unsigned long millisAnteriores = 0;
long intervalo = 1000;        //intervalo entre cada ejecucion de la maquina de estados

uint8_t estado = INICIANDO;


int valorSensorHumedad1 = 0;
int valorSensorHumedad2 = 0;
int valorSensorHumedad3 = 0;
int valorTanqueBajo = 0;
int valorTanqueAlto = 0;
int valorBoton1 = 0;

//temporizadores en segundos
int timerIniciando = 5; 
int timerRiego = 5; 
int timerLibre = 10;


int tiempoMinimoEntreRiego1 = 10;
int tiempoMinimoEntreRiego2 = 20;
int tiempoMinimoEntreRiego3 = 5;

unsigned long uiltimoRiego1 = 0;  
unsigned long uiltimoRiego2 = 0;  
unsigned long uiltimoRiego3 = 0;  

int requiereRiego1 = 0;
int requiereRiego2 = 0;
int requiereRiego3 = 0;

// Variables para trabajar con el debounce del boton
int valorAnteriorBoton1 = 0;   //El valor anterior del boton_1

unsigned long ultimoTiempoDebounce = 0;  
unsigned long debounceDelay = 50;  

// Esta variable guarda que se oprimio el boton hasta que la app haga algo con el. Luego se vuelve a 0.
int boton1 = 0;

void setup(){
  Serial.begin(115200);
  // Definicion inputs
  pinMode(PIN_SENSOR_HUMEDAD_1, INPUT);
  pinMode(PIN_SENSOR_HUMEDAD_2, INPUT);
  pinMode(PIN_SENSOR_HUMEDAD_3, INPUT);
  pinMode(PIN_TANQUE_BAJO, INPUT);
  pinMode(PIN_TANQUE_ALTO, INPUT);
  pinMode(PIN_BOTON_1, INPUT);

  //definicion Outputs
  pinMode(PIN_VALVULA_1, OUTPUT);
  pinMode(PIN_VALVULA_2, OUTPUT);
  pinMode(PIN_VALVULA_3, OUTPUT);
  pinMode(PIN_BOMBA, OUTPUT);
}




 
void loop(){
  // En el loop lo unico que hacemos controlar para que se ejecute una vez por intervalo la maquina de estados finitos

  // Marco los millis actuales con el valor de la funcion Millis
  millisActuales = millis();

  //Verifico botones fuera de la maquina de estados
  verifico_botones();

  // Si llegamos al intervalo de tiempo desde la ultima ejecucion 
  // ejecutamos la maquina de estados finitos y ponemos el valor de la ultima ejecucion (millisAnteriores) con el valor actual
  if ((millisActuales - millisAnteriores) >= intervalo){
    millisAnteriores = millisActuales;

    Serial.print("Time: ");
    Serial.print(millisActuales/1000);
    Serial.print("s - ");
    Serial.print("Estado: ");
    Serial.println(estado);
    Serial.print("valorTanqueBajo: ");
    Serial.print(valorTanqueBajo);
    Serial.print(" - valorTanqueAlto: ");
    Serial.println(valorTanqueAlto);

    maquina_estados_run();

  }
}



void verifico_botones()
{
  // BOTON 1
  int reading = digitalRead(PIN_BOTON_1);
  if (reading != valorAnteriorBoton1) {
    ultimoTiempoDebounce = millis();
  }
  if ((millis() - ultimoTiempoDebounce) > debounceDelay) {
    if (reading != valorBoton1) {
      valorBoton1 = reading;
      // only toggle the LED if the new button state is HIGH
      if (valorBoton1 == HIGH) {
        boton1 = 1;
        Serial.println("   boton1 -> nuevo valor: 1");
      }
    }
  }
  valorAnteriorBoton1 = reading;

  // BOTON 2
}


 
void maquina_estados_run() 
{
  switch(estado)
  {
    case INICIANDO:
      //Estado inicial en el que vamos a testear todo antes de ir al estado LIBRE
      iniciando();
      break;

    case LIBRE:
      //Estado por defecto de la maquina. 
      // cada tanto iremos a sensar los controles y de ahi regaremos. El resto del tiempo estaremos aca.

      if (timerLibre > 0 ){
        if (boton1 == 1){
          estado = SENSANDO;
          boton1 = 0;
          Serial.println("   boton1 -> nuevo valor: 0");
        }
        timerLibre--;
      }else{
        estado = SENSANDO;
        timerLibre = 10;
      }

      
      break;
 
    case SENSANDO:
      //Cada tanto tiempo verificaremos los niveles de agua y seguramente de humedad y regaremos.
      sensando();
      break;
 
    case REGANDO:
      //Cuando desde SENSANDO haga falta regar vendremos aca e iremos regando segun digamos.
      regando();
      break;
  }
}
 

void iniciando(){
  Serial.print("   timerIniciando:");
  Serial.println(timerIniciando);

  if (timerIniciando > 0){
    if( timerIniciando % 2 ) { 
      digitalWrite(PIN_VALVULA_1, HIGH);
      digitalWrite(PIN_VALVULA_2, HIGH);
      digitalWrite(PIN_VALVULA_3, HIGH);
      digitalWrite(PIN_BOMBA, HIGH);

    } else {
      digitalWrite(PIN_VALVULA_1, LOW);
      digitalWrite(PIN_VALVULA_2, LOW);
      digitalWrite(PIN_VALVULA_3, LOW);
      digitalWrite(PIN_BOMBA, LOW);
    }
    timerIniciando--;
  }else {
    estado = LIBRE;
    timerIniciando = 5;
    digitalWrite(PIN_VALVULA_1, LOW);
    digitalWrite(PIN_VALVULA_2, LOW);
    digitalWrite(PIN_VALVULA_3, LOW);
    digitalWrite(PIN_BOMBA, LOW);
  }      
}

void sensando(){
  valorSensorHumedad1 = digitalRead(PIN_SENSOR_HUMEDAD_1);
  valorSensorHumedad2 = digitalRead(PIN_SENSOR_HUMEDAD_2);
  valorSensorHumedad3 = digitalRead(PIN_SENSOR_HUMEDAD_3);
  valorTanqueAlto = digitalRead(PIN_TANQUE_ALTO);
  valorTanqueBajo = digitalRead(PIN_TANQUE_BAJO);

  estado = LIBRE;
  // Si el tanque esta vacio no hago nada, sino verifico si el tanque esta por revalsar o algun sensor de humedad pide agua.
  if (valorTanqueBajo != LOW){
    if (valorTanqueAlto == HIGH) {
      estado = REGANDO;
    }

    if (valorSensorHumedad1 == LOW && (millis() - uiltimoRiego1) > tiempoMinimoEntreRiego1){
      requiereRiego1 = 1;
      estado = REGANDO;
    }

    if (valorSensorHumedad2 == LOW && (millis() - uiltimoRiego2) > tiempoMinimoEntreRiego2){
      requiereRiego2 = 1;
      estado = REGANDO;
    }

    if (valorSensorHumedad3 == LOW && (millis() - uiltimoRiego3) > tiempoMinimoEntreRiego3){
      requiereRiego3 = 1;
      estado = REGANDO;
    }
  }
}

void regando(){
  Serial.print("   timerRiego:");
  Serial.println(timerRiego);

  if (timerRiego > 0){
    if( requiereRiego1 == 1 ) { 
      digitalWrite(PIN_VALVULA_1, HIGH);
      digitalWrite(PIN_BOMBA, HIGH);
    }
    if( requiereRiego2 == 1 ) { 
      digitalWrite(PIN_VALVULA_2, HIGH);
      digitalWrite(PIN_BOMBA, HIGH);
    }
    if( requiereRiego3 == 1 ) { 
      digitalWrite(PIN_VALVULA_3, HIGH);
      digitalWrite(PIN_BOMBA, HIGH);
    }
    if ((requiereRiego1 + requiereRiego2 + requiereRiego3) == 0 && valorTanqueAlto == HIGH){
      digitalWrite(PIN_VALVULA_1, HIGH);
      digitalWrite(PIN_VALVULA_2, HIGH);
      digitalWrite(PIN_VALVULA_3, HIGH);
      digitalWrite(PIN_BOMBA, HIGH);
    }
    timerRiego--;
  }else {
    estado = LIBRE;
    timerRiego = 5;
    digitalWrite(PIN_VALVULA_1, LOW);
    digitalWrite(PIN_VALVULA_2, LOW);
    digitalWrite(PIN_VALVULA_3, LOW);
    digitalWrite(PIN_BOMBA, LOW);
    requiereRiego1 = 0;
    requiereRiego2 = 0;
    requiereRiego3 = 0;
  }      
}
