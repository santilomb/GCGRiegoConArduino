#define PIN_SENSOR_LUZ 8
#define PIN_VALVULA_1 2
#define PIN_VALVULA_1 2
#define PIN_VALVULA_2 3
#define PIN_VALVULA_3 4
#define PIN_BOMBA 5
#define PIN_BOTON_1 7

enum Estados_enum {INICIANDO, LIBRE, REGANDO, REGADO};

unsigned long millisActuales;
unsigned long millisAnteriores = 0;
long intervalo = 1000;        //intervalo entre cada ejecucion de la maquina de estados

uint8_t estado = INICIANDO;

int valorBoton1 = 0;

// etapas del estado REGANDO
int faseRiego = 0;

//temporizadores en segundos
int timerIniciando = 5; 
int timerRiego = 0; 
int tiempoRiego1 = 10;
int tiempoRiego2 = 20;
int tiempoRiego3 = 5;
int timerEstadoRegado = 72000;  //Intevalo de tiempo luego del estado REGANDO en donde no se mide el sensor de LUZ  72000=20hs

// Variables para trabajar con el debounce del boton
int valorAnteriorBoton1 = 0;   //El valor anterior del boton_1

unsigned long ultimoTiempoDebounce = 0;  
unsigned long debounceDelay = 50;  

// Esta variable guarda que se oprimio el boton hasta que la app haga algo con el. Luego se vuelve a 0.
int boton1 = 0;
int valorSensorLuz = 0;

void setup(){
  Serial.begin(115200);
  // Definicion inputs
  pinMode(PIN_SENSOR_LUZ, INPUT);
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

    Serial.println("-------------------------------------------------------------------");
    Serial.print("Time: ");
    Serial.print(millisActuales/1000);
    Serial.print("s - ");
    Serial.print("Estado: ");
    Serial.println(estado);
    Serial.println("-------------------------------------------------------------------");

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
        Serial.println("      @@@@@@@@@@@@@@@@@@@@@");
        Serial.println("      @  boton1 APRETADO  @");
        Serial.println("      @@@@@@@@@@@@@@@@@@@@@");
      }
    }
  }
  valorAnteriorBoton1 = reading;

}


 
void maquina_estados_run() 
{
  Serial.println("      #######################################");
  Serial.print("        MAQUINA DE ESTADO =   ");
  Serial.println(estado);
  Serial.println("      #######################################");
  switch(estado)
  {
    case INICIANDO:
      //Estado inicial en el que vamos a testear todo antes de ir al estado LIBRE
      iniciando();
      break;

    case LIBRE:
      //Estado por defecto de la maquina. 
      // en este estado se sensara el sensor de luz, si se enciende pasaremos a REGANDO. Tambien iremos a REGANDO si se preciona el boton.
      sensando();

      if (boton1 == 1 || valorSensorLuz == 1 ){
        estado = REGANDO;
        boton1 = 0;
      }
    
      break;
 
    case REGANDO:
      //Cuando desde SENSANDO haga falta regar vendremos aca e iremos regando segun digamos.
      regando();
      break;

    case REGADO:
      //Luego de REGANDO voy a REGADO por un tiempo aproximado a 24hs donde intentaremos 
      //lograr que el sensor de luz active solo una vez al dia el riego
      //el boton podra hacer regar cuantas veces quiera tanto en LIBRE como en REGADO
      if (timerEstadoRegado > 0 ){
        if (boton1 == 1 ){
          estado = REGANDO;
          boton1 = 0;
        }
        timerEstadoRegado--;
      }else{
        estado = LIBRE;
        timerEstadoRegado = 72000;
      }
      break;
  }
}
 

void iniciando(){
  Serial.print("   timerIniciando:");
  Serial.println(timerIniciando);

  if (timerIniciando > 0){
    if( timerIniciando % 2 ) { 
      digitalWrite(PIN_VALVULA_1, LOW);
      digitalWrite(PIN_VALVULA_2, LOW);
      digitalWrite(PIN_VALVULA_3, LOW);
      digitalWrite(PIN_BOMBA, LOW);

    } else {
      digitalWrite(PIN_VALVULA_1, HIGH);
      digitalWrite(PIN_VALVULA_2, HIGH);
      digitalWrite(PIN_VALVULA_3, HIGH);
      digitalWrite(PIN_BOMBA, HIGH);
    }
    timerIniciando--;
  }else {
    estado = LIBRE;
    timerIniciando = 5;
    digitalWrite(PIN_VALVULA_1, HIGH);
    digitalWrite(PIN_VALVULA_2, HIGH);
    digitalWrite(PIN_VALVULA_3, HIGH);
    digitalWrite(PIN_BOMBA, HIGH);
  }      
}

void sensando(){
  valorSensorLuz = digitalRead(PIN_SENSOR_LUZ);
  if (valorSensorLuz == 1){
    Serial.println("      @@@@@@@@@@@@@@@@@@@@@@@@@");
    Serial.println("      @  Sensor LUZ activado  @");
    Serial.println("      @@@@@@@@@@@@@@@@@@@@@@@@@");
  }
}

void regando(){
  Serial.println("            ................................");
  Serial.print("                 REGANDO FASE  =   ");
  Serial.println(faseRiego);
  Serial.print("                 TIEMPO RESTANTE FASE  =   ");
  Serial.println(timerRiego);
  Serial.println("            ................................");
  switch (faseRiego)
  {
  case 0:
    timerRiego = tiempoRiego1;
    faseRiego = 1;
    break;
  case 1:
    digitalWrite(PIN_VALVULA_1, LOW);
    digitalWrite(PIN_BOMBA, LOW);
    timerRiego--;
    if (timerRiego == 0) {
      timerRiego = tiempoRiego2;
      faseRiego = 2;
    }
    break;
  case 2:
    digitalWrite(PIN_VALVULA_2, LOW);
    digitalWrite(PIN_BOMBA, LOW);
    timerRiego--;
    if (timerRiego == 0) {
      timerRiego = tiempoRiego3;
      faseRiego = 3;
    }
    break;
  case 3:
    digitalWrite(PIN_VALVULA_3, LOW);
    digitalWrite(PIN_BOMBA, LOW);
    timerRiego--;
    if (timerRiego == 0) {
      faseRiego = 4;
    }
    break;
  case 4:
    estado = REGADO;
    digitalWrite(PIN_VALVULA_1, HIGH);
    digitalWrite(PIN_VALVULA_2, HIGH);
    digitalWrite(PIN_VALVULA_3, HIGH);
    digitalWrite(PIN_BOMBA, HIGH);
    faseRiego = 0;
    break;
  default:
    break;
  }
}
