//TinyGSM
#if !defined(ESP8266)
#error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif
// These define's must be placed at the beginning before #include "TimerInterrupt_Generic.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG         1
#define _TIMERINTERRUPT_LOGLEVEL_     0

#include "TimerInterrupt_Generic.h"

#define PWR_A6 5
#define RST_A6 4
#define RELE 12
#define TIMER_INTERVAL_MS        1000
#define TIMER_RELE        3
#define OPEN_COUNT        5

static uint32_t started = false;
ESP8266Timer ITimer;
volatile uint32_t lastMillis = 0;
uint32_t timeRele = 0, countOpen = 0;
bool onRele = 0;

void IRAM_ATTR TimerHandler()
{
  static bool toggle = false;

  //Serial1.println ("TimerHandler");
  if (started == 1)
  {
    timeRele++;
    if (timeRele == TIMER_RELE) {
      started = 2;
      digitalWrite(RELE, LOW);
      Serial1.println ("End RELE ph1");
      timeRele = 0;

    }

  } else if (started == 2) {

    if (digitalRead(RELE)) {
      digitalWrite(RELE, LOW);
    } else {
      digitalWrite(RELE, HIGH);
    }

    countOpen++;
    if (countOpen == OPEN_COUNT) {
      started = 0;
      countOpen = 0;
      timeRele = 0;
      digitalWrite(RELE, LOW);
      Serial1.println ("End RELE ph2");
    }
  }
}

void setup() {
  //pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.println("start");
  Serial.begin(115200);
  Serial.println("115200 set");
  Serial.swap(); // переносим на другие пины
  Serial.println("swap");
  Serial.flush(); // чистим буфер

  Serial1.begin(115200); //инициализируем второй uart (пин светодиода)
  Serial1.println("serial1_start");
  //Serial1.setDebugOutput(true);
  //Serial1.println("serial1_debug true");


  Serial1.println("Start GSM");
  //включаем питание
  pinMode(PWR_A6, OUTPUT); //питание
  pinMode(RST_A6, OUTPUT); // ресет
  pinMode(RELE, OUTPUT);

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
  delay(3000);

  //Serial1.println("wait GSM");

  Serial.println("AT");
  delay(3000);

  reSendDebug();


  // Interval in microsecs
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    lastMillis = millis();
    Serial.print(F("Starting  ITimer OK, millis() = ")); Serial.println(lastMillis);
  }
  else
    Serial.println(F("Can't set ITimer correctly. Select another freq. or interval"));
}

//в строке curStr будем хранить текущую строку, которую передает нам плата
String currStr = "";
int updateTime = 0;

// the loop function runs over and over again forever
void loop() {

  if (!Serial.available())
    return;

  // Считываем очередной символ с платы
  char currSymb = Serial.read();

  if ('\r' == currSymb) {
    // Получен символ перевода строки, это значит, что текущее
    // сообщение от платы завершено и мы можем на него отреагировать.
    // Если текущая строка - это RING, то значит, нам кто-то звонит
    if (!currStr.compareTo("RING")) {
      //кокетничаем 3 секунды, чтобы дать услышать звонящему гудок
      delay(3000);
      //посылаем команду на поднятие трубки
      //Serial.flush();
      Serial.println("AT+CLCC");
      delay (100);
      String number;
      while (Serial.available() > 0) {
        number += (char)Serial.read();
        if (Serial.available() <= 0) {
          Serial1.println(number);
        }
      }
      
      //Открытие домофона
      if (started == 0) {
        delay (1000);
        //Serial.flush();
        Serial.println("AT+CHUP");
        delay (100);
        Serial1.println ("End ring");
        started = 1;
        digitalWrite(RELE, HIGH);
      }

    }
    currStr = "";
  } else if (currSymb != '\n') {
    // Дополняем текущую команду новым сиволом
    // При этом игнорируем второй символ в последовательности переноса
    // строки: \r\n
    currStr += String(currSymb);
  }

  /*
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
    delay(1000);                      // Wait for a second
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
    delay(2000);                      // Wait for two seconds (to demonstrate the active low LED)
    //Serial.println("loop");
    Serial1.println("loop");
  */


}

void reSendDebug() {
  String str;
  while (Serial.available() > 0) {
    str += (char)Serial.read();
    if (Serial.available() <= 0) {
      Serial1.println(str);
    }
  }
}
