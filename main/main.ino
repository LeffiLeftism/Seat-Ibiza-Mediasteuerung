#include "voltage_transformer.h"
#include "relais.h"
#include "timer.h"
#define DEBUG

//Globale variables
//Voltage Transformer
VoltageTransformer IV(A0,900, 469);          //Ignitionvoltate IV
VoltageTransformer BV(A1,815, 459);          //Boardvoltage BV
VoltageTransformer BAT2V(A6,900, 469);       //Battery 2 voltate BAT2V
VoltageTransformer CAPV(A7, 850, 469);       //Capacitorvoltate CAPV
//Relais
Relais BAT2V_CAPV(2);                         //Connection Battery 2 to Capacitor
Relais BV_BAT2V(3);                           //Connection Boardvoltage to Battery 2
Relais BAT2V_PS(4);                           //Connection Battery 2 to PowerSupply
Relais CAP(5);                                //Ref Voltage for AMP
Relais AMP(6);                                //Ref Voltage for AMP
Relais LoadLamp(7);                           //Lamp to load the CAP
//Timer
Timer T_BAT2(uint32_t(600000));               //Timer to load BAT2 within 10 Minutes
Timer T_Ignition(uint32_t(20000));            //Timer to deactivate all after ignition is off

void setup() {
  // put your setup code here, to run once:
  #ifdef DEBUG 
    Serial.begin(9600);
    Serial.println("Setup");
  #endif
  //Initialize voltage transformer
  IV.init();
  BV.init();
  BAT2V.init();
  CAPV.init();
  //Initialize relais
  BAT2V_CAPV.init();
  BV_BAT2V.init();
  BAT2V_PS.init();
  CAP.init();
  AMP.init();
  LoadLamp.init();
  //Initialize timer
  T_BAT2.init();
  T_Ignition.init();
}

void loop() {
  #ifdef DEBUG 
    Serial.println("------------------");
    printVoltages();    
    delay(100);
  #endif
  // put your main code here, to run repeatedly:
  T_BAT2.update();
  T_Ignition.update();
  checkIgnition();
}

#ifdef DEBUG
  void printVoltages() {
    //Serial.print("PIN");
    //Serial.print(" | ");
    //Serial.print("R1  "); 
    //Serial.print(" | ");
    //Serial.print("R2  ");
    //Serial.print(" | ");
    //Serial.print("Read");
    //Serial.print(" | ");
    //Serial.print("Fac");
    //Serial.print(" | ");
    //Serial.print("Volt");
    //Serial.println();
    //uint16_t t1 = IV.getRaw();
    //uint16_t t2 = BV.getRaw();
    //delay(500);
    Serial.print(IV.get());
    Serial.print(" | ");
    Serial.print(BV.get());
    Serial.print(" | ");
    Serial.print(BAT2V.get());
    Serial.print(" | ");
    Serial.print(CAPV.get());
    Serial.println();
    delay(3000);
  }
#endif

void checkIgnition() {
  if(IV.get() > 10.5) { //Check if Ignition is turned on
    checkup();
    T_Ignition.reset();
  } else if (!T_Ignition.isStartet()) {
    #ifdef DEBUG
      Serial.println("Timer Started Ignition");
    #endif
    T_Ignition.start();
  } else if (T_Ignition.isFinished()){
    #ifdef DEBUG
      Serial.println("Timer Finished Ignition");
    #endif
    resetRelais();
    #ifdef DEBUG
      Serial.println("Timer Reset Ignition");
    #endif
    T_Ignition.reset();
  }
}

void checkup(){
  if ((BV.get() < 12.8)) { //Check if Engine is running -> voltage around 14.2V estimated
    // Do if engine is not running
    #ifdef DEBUG 
      Serial.println("BV not OK");
    #endif
    delay(50);
    goto reset;
  }
  if ((BAT2V.get() < 11)) { //Check if BAT2 is loaded enought (min 11.5V)
    // Do if BAT2 is too empty
    #ifdef DEBUG 
      Serial.println("BAT2V not OK");
    #endif
    if (!T_BAT2.isStartet()) {
      #ifdef DEBUG 
        Serial.println("Timer Started");
      #endif
      BV_BAT2V.on();
      T_BAT2.start();
      goto end;
    }
  }
  if (T_BAT2.isStartet()) { //Check if Timer for BAT2 is startet
    #ifdef DEBUG 
      Serial.println("Check Timer");
    #endif
    if (T_BAT2.isFinished()) {
      #ifdef DEBUG 
        Serial.println("Timer Finished");
      #endif
      BV_BAT2V.off();
      #ifdef DEBUG
        Serial.println("Timer Reset");
      #endif
      T_BAT2.reset();
    }
    goto end;
  }
  BV_BAT2V.on();
  BAT2V_PS.on();
  CAP.on();
  if ((CAPV.get() < 10.5)) { //Check if CAP is loaded enought (min 11.5V)
    // Do if CAP is too empty
    #ifdef DEBUG 
      Serial.println("CAPV not OK");
    #endif
    //ERROR somehow: sound? led?
    //Later: load CAP
    if (!LoadLamp.get()) {
      #ifdef DEBUG
        Serial.println("LoadLamp on");
      #endif
      LoadLamp.on();
    }
    goto end;
  } else if (LoadLamp.get()) {
      #ifdef DEBUG
        Serial.println("LoadLamp off");
      #endif
    LoadLamp.off();
  }
  BAT2V_CAPV.on();
  AMP.on();
  #ifdef DEBUG 
    Serial.println("AMP on");
  #endif
  goto end;
  reset:
  resetRelais();
  end:
  #ifdef DEBUG 
    Serial.println("END");
  #endif
  ;
}

void resetRelais() {
  #ifdef DEBUG 
    Serial.println("Reset Relais");
  #endif
  LoadLamp.off();  
  AMP.off();  
  CAP.off();
  BAT2V_CAPV.off();
  BV_BAT2V.off();
  BAT2V_PS.off();
  //The system should shutdown if ignition is off, because no power

}

