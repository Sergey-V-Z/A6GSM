//TinyGSM 

#include <iarduino_GSM.h>

#define PWR_A6 5
#define RST_A6 4
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.println("start");
  Serial.begin(115200);
  Serial.println("115200 set");
  Serial.swap(); // переносим на другие пины
  Serial.println("swap");
  Serial.flush(); // чистим буфер
  
  Serial1.begin(115200); //инициализируем второй uart (пин светодиода)
  Serial1.println("serial1_start");
  Serial1.setDebugOutput(true);
  Serial1.println("serial1_debug true");


  Serial1.println("Start GSM");
  //включаем питание
  pinMode(PWR_A6, OUTPUT); //питание
  pinMode(RST_A6, OUTPUT); // ресет
  digitalWrite(RST_A6, LOW); //Default settings for RST
  digitalWrite(PWR_A6, HIGH); // Default settings for PWR

  Serial1.println("Reset GSM");
  //Reset A6 so it initiates
  digitalWrite(RST_A6, HIGH);
  delay(1000);
  digitalWrite(RST_A6, LOW);
  
  
  // After delay, remove power from the enable pin on the A6
  delay(3000);
  digitalWrite(PWR_A6, LOW);

  //Serial1.println("wait GSM");

  Serial.println("AT");
  if (Serial.available()){
    Serial1.write(Serial.read());
  }
  
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  // but actually the LED is on; this is because
  // it is active low on the ESP-01)
  delay(1000);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(2000);                      // Wait for two seconds (to demonstrate the active low LED)
  //Serial.println("loop");
  Serial1.println("loop");
}
