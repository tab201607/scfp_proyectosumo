#include <XboxSeriesXControllerESP32_asukiaaa.hpp>

// Required to replace with your xbox address
XboxSeriesXControllerESP32_asukiaaa::Core xboxController("14:cb:65:c1:26:2e");

// any xbox controller
//XboxSeriesXControllerESP32_asukiaaa::Core xboxController;

unsigned long leftjoyvert = 0;
unsigned long rightjoyvert = 0;

char currentstateL = 0; // 0 - no moviendo, 1 para adelante, 2 para atras
char currentstateR = 0;

const char forward = 1;
const char backward = 2;
const char still = 0;

const char switchdelay = 25;

const long thresholdvalue = 12232; // lo utilizamos para determinar si se activa el movimiento
const long thresholdhigh = 32768 + thresholdvalue;
const long thresholdlow = 32768 - thresholdvalue;

const long powerreducer = 50;

unsigned long pwmcontroll = 0;
unsigned long pwmcontrolr = 0;

const int PWMFREQ = 500;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting NimBLE Client");
  xboxController.begin();
  
  pinMode(2, OUTPUT); //Usamos el LED para indicar conexion

  pinMode(22, OUTPUT); //Pin adelante izq
  pinMode(21, OUTPUT); //Pin atras izq
  pinMode(19, OUTPUT); //Pin adelante der
  pinMode(18, OUTPUT); //Pin atras der

  ledcSetup(0, PWMFREQ, 8); //configuramos primer canal PWM con 8 bits y 500Hz
  ledcAttachPin(23, 0); // conectamos el pin 23

   ledcSetup(1, PWMFREQ, 8); //configuramos primer canal PWM con 8 bits y 500Hz
  ledcAttachPin(5, 1); //conectamos el pin 5
  
}

void loop() {
  xboxController.onLoop();
  if (xboxController.isConnected()) { // revision conexion
      digitalWrite(2, HIGH); //encendemos una luz que muestra que se conecto
    
      // Grabamos los valores del control de xbox
      leftjoyvert = (xboxController.xboxNotif.joyLVert);
      rightjoyvert = (xboxController.xboxNotif.joyRVert);

     
// control motor derecho     

      
      
      if (rightjoyvert < thresholdlow) //Adelante motor derecho
          { 
            if (currentstateR == backward) { //Pausamos un momento antes de cambiar la direccion
              digitalWrite(19, LOW);
              digitalWrite(18, LOW);
              delay(switchdelay);}

            if (xboxController.xboxNotif.btnRB) {
              pwmcontrolr = 255;
              Serial.print("max power");
      }
            else {              pwmcontrolr = 255 - map(rightjoyvert, 0, thresholdlow, powerreducer, 180);
              }
              
            
            digitalWrite(19, HIGH);
          digitalWrite(18, LOW);
          ledcWrite(1, pwmcontrolr);
          
          currentstateR = forward;
          }
      else if (rightjoyvert > thresholdhigh) //Atras motor derecho
          {
            if (currentstateR == forward) { //Pausamos un momento antes de cambiar la direccion
              digitalWrite(19, LOW);
              digitalWrite(18, LOW);
              delay(switchdelay);}


             if (xboxController.xboxNotif.btnRB) {
              pwmcontrolr = 255;
              Serial.print("max power");
      }           
            else {pwmcontrolr = map(rightjoyvert, thresholdhigh, 65536, 75, 255 - powerreducer);}
          
            digitalWrite(19, LOW);
          digitalWrite(18, HIGH);
          ledcWrite(1, pwmcontrolr);
          currentstateR = backward;
          }
      else // apagar motor
          {digitalWrite(19, LOW);
          digitalWrite(18, LOW);
          currentstateR = still;
          }

//control motor izquierdo

      if (leftjoyvert < thresholdlow) //Adelante motor izquierdo
          {
            if (currentstateL == backward) { //Pausamos un momento antes de cambiar la direccion
              digitalWrite(22, LOW);
              digitalWrite(21, LOW);
              delay(switchdelay);}            

            if (xboxController.xboxNotif.btnLB) {
              Serial.print("max power");
              pwmcontroll = 255;
            }
            else   {
            pwmcontroll = 255 - map(leftjoyvert, 0, thresholdlow, powerreducer, 180);}
            digitalWrite(22, LOW);
          digitalWrite(21, HIGH);
          ledcWrite(0, pwmcontroll);
          currentstateL = forward;}
      else if (leftjoyvert > thresholdhigh) //Atras motor derecho
          {
            if (currentstateL == forward) { //Pausamos un momento antes de cambiar la direccion
              digitalWrite(22, LOW);
              digitalWrite(21, LOW);
              delay(switchdelay);}   


            if (xboxController.xboxNotif.btnLB) {
              Serial.print("max power");
              pwmcontroll = 255;
            }
            else {pwmcontroll = map(leftjoyvert, thresholdhigh, 65536, 75, 255 - powerreducer);}
            
            digitalWrite(22, HIGH);
          digitalWrite(21, LOW);
          ledcWrite(0, pwmcontroll);
          currentstateL = backward;}
      
      else // apagar motor
          {digitalWrite(22, LOW);
          digitalWrite(21, LOW);
          currentstateR = still;}

      
      
      Serial.println("Address: " + xboxController.buildDeviceAddressStr());
      Serial.print("Left: ");
      Serial.print(pwmcontroll);
      Serial.print(" | Right: ");
      Serial.println(pwmcontrolr);
    }
   else {
    digitalWrite(2, LOW); //encendemos una luz que muestra que se conecto
    Serial.println("not connected");
    if (xboxController.getCountFailedConnection() > 2) {
      ESP.restart();
    }
  }
  Serial.println("at " + String(millis()));
  delay(500);
}
