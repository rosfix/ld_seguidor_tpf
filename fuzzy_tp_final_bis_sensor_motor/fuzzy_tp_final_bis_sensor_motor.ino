#include <Fuzzy.h>
#include <AFMotor.h>

// Fuzzy
Fuzzy *difuso = new Fuzzy();

// FuzzyInput: distancia
FuzzySet *cerca = new FuzzySet(0, 2, 50, 70);
FuzzySet *distante = new FuzzySet(90, 150, 250, 330);
FuzzySet *lejos = new FuzzySet(270, 320, 350, 350);

// FuzzyInput: velocidad
FuzzySet *lento = new FuzzySet(0, 5, 30, 35);
FuzzySet *crucero = new FuzzySet(25, 40, 60, 80);
FuzzySet *corrida = new FuzzySet(70, 85, 120, 150);

// FuzzyOutput: avance
FuzzySet *acercar = new FuzzySet(0, 2, 20, 50);
FuzzySet *seguir = new FuzzySet(25, 40, 60, 70);
FuzzySet *atrapar = new FuzzySet(55, 80, 100, 100);



const int EchoPin = 2;
const int TriggerPin = 13;

AF_Stepper motor1(256, 1);       

AF_Stepper motor2(256, 2);   

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
  lejosYcrucero->joinWithAND(lejos, crucero);
  FuzzyRuleConsequent *captura = new FuzzyRuleConsequent();
  captura->addOutput(atrapar);
  FuzzyRule *regla3 = new FuzzyRule(3, lejosYcrucero, captura);
  difuso->addFuzzyRule(regla3);

    FuzzyRuleAntecedent *cercaYcrucero = new FuzzyRuleAntecedent();
 cercaYcrucero->joinWithAND(cerca, crucero);
  FuzzyRuleConsequent *cruzarcerca = new FuzzyRuleConsequent();
  cruzarcerca->addOutput(acercar);
  FuzzyRule *regla4 = new FuzzyRule(4, cercaYcrucero, cruzarcerca);
  difuso->addFuzzyRule(regla4);
  
    FuzzyRuleAntecedent *cercaYcorrida = new FuzzyRuleAntecedent();
  cercaYcorrida->joinWithAND(cerca, corrida);
  FuzzyRuleConsequent *correrCerca = new FuzzyRuleConsequent();
  correrCerca->addOutput(seguir);
  FuzzyRule *regla5 = new FuzzyRule(5, cercaYcorrida, correrCerca );
  difuso->addFuzzyRule(regla5);
  
    FuzzyRuleAntecedent *distanteYlento = new FuzzyRuleAntecedent();
  distanteYlento->joinWithAND(distante, lento);
  FuzzyRuleConsequent *sigilo = new FuzzyRuleConsequent();
  sigilo->addOutput(acercar);
  FuzzyRule *regla6 = new FuzzyRule(6, distanteYlento, sigilo);
  difuso->addFuzzyRule(regla6);
  
    FuzzyRuleAntecedent *distanteYcorrida = new FuzzyRuleAntecedent();
  distanteYcorrida->joinWithAND(distante, corrida);
  FuzzyRuleConsequent *sepierde = new FuzzyRuleConsequent();
  sepierde->addOutput(atrapar);
  FuzzyRule *regla7 = new FuzzyRule(7,distanteYcorrida, sepierde);
  difuso->addFuzzyRule(regla7);
  
    FuzzyRuleAntecedent *lejosYlento = new FuzzyRuleAntecedent();
  lejosYlento->joinWithAND(lejos, lento);
  FuzzyRuleConsequent *nomevio = new FuzzyRuleConsequent();
  nomevio->addOutput(seguir);
  FuzzyRule *regla8 = new FuzzyRule(8, lejosYlento, nomevio);
  difuso->addFuzzyRule(regla8);

     FuzzyRuleAntecedent *lejosYcorrida = new FuzzyRuleAntecedent();
  lejosYcorrida->joinWithAND(lejos, corrida);
  FuzzyRuleConsequent *seva = new FuzzyRuleConsequent();
  seva->addOutput(seguir);
  FuzzyRule *regla9 = new FuzzyRule(9, lejosYcorrida, seva);
  difuso->addFuzzyRule(regla9);
  
   Serial.begin(9600);
   pinMode(TriggerPin, OUTPUT);
   pinMode(EchoPin, INPUT);

   motor1.setSpeed(70); //rpm
   motor2.setSpeed(70); // rpm
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


  int pasos=avanceMotor*3;
  Serial.print(" Pasos:");
  Serial.println(pasos);


   
 for (int paso = 0; paso <= pasos; paso++) {
   motor1.step(1, FORWARD, DOUBLE);
   motor2.step(1, BACKWARD, DOUBLE);
  }

  

}

void velocidad() {

   cm = ping(TriggerPin, EchoPin);
   vel=(cm-cma);
   
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
