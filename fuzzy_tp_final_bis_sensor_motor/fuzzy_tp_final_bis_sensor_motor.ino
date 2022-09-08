#include <Fuzzy.h>
#include <AFMotor.h>

// Fuzzy
Fuzzy *difuso = new Fuzzy();

// FuzzyInput: distancia
FuzzySet *cerca = new FuzzySet(0, 30, 30, 40);
FuzzySet *distante = new FuzzySet(35, 70, 70, 90);
FuzzySet *lejos = new FuzzySet(80, 150, 150, 200);

// FuzzyInput: velocidad
FuzzySet *lento = new FuzzySet(0, 10, 10, 20);
FuzzySet *crucero = new FuzzySet(10, 40, 40, 60);
FuzzySet *corrida = new FuzzySet(50, 70, 70, 100);

// FuzzyOutput: avance
FuzzySet *acercar = new FuzzySet(0, 5, 5, 10);
FuzzySet *seguir = new FuzzySet(7, 20, 20, 50);
FuzzySet *atrapar = new FuzzySet(50, 70, 70, 100);



const int EchoPin = 2;
const int TriggerPin = 13;

AF_Stepper motor(256, 1);       

void setup() {

  // FuzzyInput
  FuzzyInput *distancia = new FuzzyInput(1);

  distancia->addFuzzySet(cerca);
  distancia->addFuzzySet(distante);
  distancia->addFuzzySet(lejos);
  difuso->addFuzzyInput(distancia);

  FuzzyInput *velocidad = new FuzzyInput(2);

  velocidad->addFuzzySet(lento);
  velocidad->addFuzzySet(crucero);
  velocidad->addFuzzySet(corrida);
  difuso->addFuzzyInput(velocidad);

 // FuzzyOutput
  FuzzyOutput *avance = new FuzzyOutput(1);

  avance->addFuzzySet(acercar);
  avance->addFuzzySet(seguir);
  avance->addFuzzySet(atrapar);
  difuso->addFuzzyOutput(avance);

//R1: IF {distancia IS cerca} AND {velocidad IS lento} THEN {avance IS acercar} weigth=1.0
//R2: IF {distancia IS distante} AND {velocidad IS crucero} THEN {avance IS seguir} weigth=1.0
//R3: IF {distancia IS lejos} AND {velocidad IS crucero} THEN {avance IS atrapar} weigth=1.0

  FuzzyRuleAntecedent *cercaYlento = new FuzzyRuleAntecedent();
  cercaYlento->joinWithAND(cerca, lento);
  FuzzyRuleConsequent *acercamiento = new FuzzyRuleConsequent();
  acercamiento->addOutput(acercar);
  FuzzyRule *regla1 = new FuzzyRule(1, cercaYlento, acercamiento);
  difuso->addFuzzyRule(regla1);

  FuzzyRuleAntecedent *distanteYcrucero = new FuzzyRuleAntecedent();
  distanteYcrucero->joinWithAND(distante, crucero);
  FuzzyRuleConsequent *seguimiento = new FuzzyRuleConsequent();
  seguimiento->addOutput(acercar);
  FuzzyRule *regla2 = new FuzzyRule(2, distanteYcrucero, seguimiento);
  difuso->addFuzzyRule(regla2);

  FuzzyRuleAntecedent *lejosYcrucero = new FuzzyRuleAntecedent();
  lejosYcrucero->joinWithAND(distante, crucero);
  FuzzyRuleConsequent *captura = new FuzzyRuleConsequent();
  seguimiento->addOutput(atrapar);
  FuzzyRule *regla3 = new FuzzyRule(3, lejosYcrucero, captura);
  difuso->addFuzzyRule(regla3);
  
   Serial.begin(9600);
   pinMode(TriggerPin, OUTPUT);
   pinMode(EchoPin, INPUT);

   motor.setSpeed(50); // 50 rpm
}

int cm=0;
int cma=0;
int vel=0;

void loop() {
   velocidad();

  difuso->setInput(1, cm);
  difuso->setInput(2, vel);
  difuso->fuzzify();

  float avanceMotor = difuso->defuzzify(1);
   Serial.print(" Avance:");
   Serial.println(avanceMotor);

   motor.step(avanceMotor*40, FORWARD, SINGLE);
}

void velocidad() {

   cm = ping(TriggerPin, EchoPin);
   vel=cm-cma;
   
   Serial.print("D:");
   Serial.print(cm);
   Serial.print(" V:");
   Serial.println(vel);
   cma=cm;
   delay(1000);
}

int ping(int TriggerPin, int EchoPin) {
   long duration, distanceCm;
   
   digitalWrite(TriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
   delayMicroseconds(4);
   digitalWrite(TriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
   delayMicroseconds(10);
   digitalWrite(TriggerPin, LOW);
   
   duration = pulseIn(EchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos

   
   distanceCm = duration * 10 / 292/ 2;   //convertimos a distancia, en cm
   return distanceCm;
}
