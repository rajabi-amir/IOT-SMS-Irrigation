#include <Sim800l.h>
#include <SoftwareSerial.h>
#include <iostream>
#include <string>
Sim800l Sim800l;

#define timeMinuts 60
// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;
char *text1 = "Water Opened";
char *text2 = "Water Closed";
char *status_text1 = "Water Is Open";
char *status_text2 = "Water Is Close";
char *cNumber;
String textSms, numberSms;
const int relayPin = 5;
bool error;
bool Status = true;
ICACHE_RAM_ATTR void waterOpen()
{
  // Serial.println("Water Is Open");
  startTimer = true;
  lastTrigger = millis();
}

void setup()
{
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, Status);
  Serial.begin(9600);
  Sim800l.begin();
  error = Sim800l.delAllSms();
  // Set relay pin as interrupt, assign interrupt function and set RISING mode
  attachInterrupt(digitalPinToInterrupt(relayPin), waterOpen, FALLING);
}

void loop()
{
  textSms = Sim800l.readSms(1);
  if (textSms.indexOf("OK") != -1)
  {
    if (textSms.length() > 7)
    {
      numberSms = Sim800l.getNumberSms(1);
      Serial.println(numberSms);
      textSms.toUpperCase();
      //convert string numberSms to char
      cNumber = const_cast<char *>(numberSms.c_str());
      if (textSms.indexOf("OPEN") != -1)
      {
        Status = false;
        // Serial.println("WATER IS OPEN");
        digitalWrite(relayPin, Status);
        error = Sim800l.sendSms(cNumber, text1);
      }
      else if (textSms.indexOf("CLOSE") != -1)
      {
        Status = true;
        startTimer = false;
        // Serial.println("WATER IS CLOSE");
        digitalWrite(relayPin, Status);
        error = Sim800l.sendSms(cNumber, text2);
      }
      else if (textSms.indexOf("STATUS") != -1)
      {
        if (Status)
          error = Sim800l.sendSms(cNumber, status_text2);
        else
          error = Sim800l.sendSms(cNumber, status_text1);
      }
      else
      {
        Serial.println("Not Valid");
      }
      error = Sim800l.delAllSms();
    }
  }
  // Current time
  now = millis();
  // Turn off the LED after the number of seconds defined in the minutes variable
  if (startTimer && (now - lastTrigger > (timeMinuts * 1000 * 60)))
  {
    // Serial.println("Water stopped...");
    Status = true;
    digitalWrite(relayPin, HIGH);
    error = Sim800l.sendSms(cNumber, text2);
    startTimer = false;
  }
}
