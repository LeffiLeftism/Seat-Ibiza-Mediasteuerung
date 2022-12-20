#include "voltage_transformer.h"
#include "relais.h"
#include "timer.h"
//#define DEBUG

//Globale variables
VoltageTransformer IV(A0,1000, 490);          //Ignitionvoltate IV
VoltageTransformer BV(A1,1000, 490);          //Boardvoltage BV
VoltageTransformer BAT2V(A6,1000, 490);       //Battery 2 voltate BAT2V
VoltageTransformer CAPV(A7, 1000, 490);       //Capacitorvoltate CAPV
Relais BV_BAT2V(2);                           //Connection Boardvoltage to Battery 2
Relais BAT2V_CAPV(3);                         //Connection Battery 2 to Capacitor
Relais BAT2V_PS(4);                           //Connection Battery 2 to PowerSupply
Relais CAPV_AMP(5);                           //Connection Capacitor to Amplifier
Timer T_BAT2(10*60*1000);                     //Timer to load BAT2 within 10 Minutes
Timer T_Ignition(1*60*1000);                  //Timer to deactivate all after ignition is off

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
  BV_BAT2V.init();
  BAT2V_CAPV.init();
  BAT2V_PS.init();
  CAPV_AMP.init();
  //Initialize timer
  T_BAT2.init();
  T_Ignition.init();
}

void loop() {
  #ifdef DEBUG 
    Serial.println("------------------");
  #endif
  // put your main code here, to run repeatedly:
  T_BAT2.update();
  checkIgnition();
}

void checkIgnition() {
  if(IV.get() > 11.5) { //Check if Ignition is turned on
    checkup();
  } else if (!T_Ignition.isStartet()) {
    T_Ignition.start();
  } else if (T_Ignition.isFinished()){
    resetRelais();
  }
}

void checkup(){
  if (BV.get() < 13.5) { //Check if Engine is running -> voltage around 14.2V estimated
    // Do if engine is not running
    #ifdef DEBUG 
      Serial.println("BV not OK");
    #endif
    delay(50);
    goto reset;
  }
  if (BAT2V.get() < 11.5) { //Check if BAT2 is loaded enought (min 11.5V)
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
      T_BAT2.reset();
    }
    goto end;
  }
  BV_BAT2V.on();
  BAT2V_PS.on();
  if (CAPV.get() < 11.5) { //Check if CAP is loaded enought (min 11.5V)
    // Do if CAP is too empty
    #ifdef DEBUG 
      Serial.println("CAPV not OK");
    #endif
    //ERROR somehow: sound? led?
    //Later: load CAP
    delay(5000);
    goto end;
  }
  BAT2V_CAPV.on();
  CAPV_AMP.on();
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
  CAPV_AMP.off();
  BAT2V_CAPV.off();
  BV_BAT2V.off();
  BAT2V_PS.off();
  //The system should shutdown, because no power
}