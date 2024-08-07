#include <OneButton.h>


#define DEBUGMODE 1

#if DEBUGMODE == 1
#define debug(x) Serial.println(x)
#else
#define debug(x)
#endif

#define HIGHBEAM_PIN 15
#define UPMOTOR_PIN 5
#define DOWNMOTOR_PIN 18
#define RIGHT_SEIN_PIN 21
#define LEFT_SEIN_PIN 19
#define HORN_PIN 23
#define KEYCONTACT_PIN 2
#define MOTORSENSOR_PIN 22
#define SELLECTOR 4
#define RELAY_HIGHBEAM_PIN 12
#define RELAY_LOWBEAM_PIN 13
#define RELAY_LEFTSIGNAL_PIN 32
#define RELAY_RIGHTSIGNAL_PIN 33
#define RELAY_HORN_PIN 25
#define RELAY_UPMOTOR_PIN 26
#define RELAY_DOWNMOTOR_PIN 27


String mode = "default";
bool trigger_change_mode = false;
unsigned long start_pressed_timer_selector, end_pressed_timer_selector, total_pressed_timer_selector;
unsigned long curenttime, lasthighbeam, lastsein, lasthorn = 0;
bool isOnHighBeam, isOnsein, isOnHorn;
int freqSeindefault = 5;
int freqHighBeamdefault = 0;
int freqHorndefault = 0;
int freqSeinagresive = 10;
int freqHighBeamagresive = 20;
int freqHornagresive = 20;
int freqSeinemergency = 10;
int freqHighBeamemergency = 30;
int freqHornemergency = 30;
int freqSeinquiete = 5;
int freqHighBeamquiete = 0;
int freqHornquiete = 0;

int operationalMode = 0;


void IRAM_ATTR handleD2Low();

void shutdownSequence() {
  debug("shutting down ");



  // Add your shutdown sequence code here

  // Enter deep sleep and wake up on D2 high

  esp_sleep_enable_ext0_wakeup((gpio_num_t)KEYCONTACT_PIN, 1);
  esp_deep_sleep_start();
}

void startSequence() {
  debug("starting motor ");

  digitalWrite(RELAY_RIGHTSIGNAL_PIN, LOW);
  delay(100);

  digitalWrite(RELAY_LEFTSIGNAL_PIN, LOW);
  digitalWrite(RELAY_LOWBEAM_PIN, LOW);

  delay(100);
  digitalWrite(RELAY_RIGHTSIGNAL_PIN, HIGH);
  digitalWrite(RELAY_LEFTSIGNAL_PIN, HIGH);
  digitalWrite(RELAY_LOWBEAM_PIN, HIGH);
  delay(100);
  digitalWrite(RELAY_RIGHTSIGNAL_PIN, LOW);
  digitalWrite(RELAY_LEFTSIGNAL_PIN, LOW);
  digitalWrite(RELAY_LOWBEAM_PIN, LOW);
  delay(100);
  digitalWrite(RELAY_RIGHTSIGNAL_PIN, HIGH);
  digitalWrite(RELAY_LEFTSIGNAL_PIN, HIGH);
  digitalWrite(RELAY_LOWBEAM_PIN, HIGH);
  delay(1000);

  // Add your start sequence code here
  // Re-attach the interrupt for the next shutdown
  attachInterrupt(digitalPinToInterrupt(KEYCONTACT_PIN), handleD2Low, FALLING);
  operationalMode = 1;
}

void IRAM_ATTR handleD2Low() {
  operationalMode = -1;

  digitalWrite(RELAY_RIGHTSIGNAL_PIN, LOW);
  delay(100);

  digitalWrite(RELAY_LEFTSIGNAL_PIN, LOW);
  digitalWrite(RELAY_LOWBEAM_PIN, LOW);

  delay(100);
  digitalWrite(RELAY_RIGHTSIGNAL_PIN, HIGH);
  digitalWrite(RELAY_LEFTSIGNAL_PIN, HIGH);
  digitalWrite(RELAY_LOWBEAM_PIN, HIGH);
  delay(100);
  digitalWrite(RELAY_RIGHTSIGNAL_PIN, LOW);
  digitalWrite(RELAY_LEFTSIGNAL_PIN, LOW);
  digitalWrite(RELAY_LOWBEAM_PIN, LOW);
  delay(100);
  digitalWrite(RELAY_RIGHTSIGNAL_PIN, HIGH);
  digitalWrite(RELAY_LEFTSIGNAL_PIN, HIGH);
  digitalWrite(RELAY_LOWBEAM_PIN, HIGH);
  delay(100);
  digitalWrite(RELAY_RIGHTSIGNAL_PIN, LOW);
  digitalWrite(RELAY_LEFTSIGNAL_PIN, LOW);
  digitalWrite(RELAY_LOWBEAM_PIN, LOW);
  delay(100);
  digitalWrite(RELAY_RIGHTSIGNAL_PIN, HIGH);
  digitalWrite(RELAY_LEFTSIGNAL_PIN, HIGH);
  digitalWrite(RELAY_LOWBEAM_PIN, HIGH);
  delay(100);
  digitalWrite(RELAY_RIGHTSIGNAL_PIN, LOW);
  digitalWrite(RELAY_LEFTSIGNAL_PIN, LOW);
  digitalWrite(RELAY_LOWBEAM_PIN, LOW);
  delay(100);
  digitalWrite(RELAY_RIGHTSIGNAL_PIN, HIGH);
  digitalWrite(RELAY_LEFTSIGNAL_PIN, HIGH);
  digitalWrite(RELAY_LOWBEAM_PIN, HIGH);

  delay(1000);


  detachInterrupt(digitalPinToInterrupt(KEYCONTACT_PIN));
  shutdownSequence();
}




OneButton SellectorBtn(SELLECTOR, false, false);




void setup() {
  Serial.begin(115200);
  setupPin();
  initiateHigh();
  SellectorBtn.setLongPressIntervalMs(800);
  SellectorBtn.attachClick(clickSellector);
  SellectorBtn.attachDoubleClick(doubleclickSellector);
  SellectorBtn.attachLongPressStart(longPressStartSellector);
  SellectorBtn.attachLongPressStop(longPressStopSellector);


  xTaskCreatePinnedToCore(
    loop1,             // Function to run
    "ReadAndAcction",  // Name of the task
    10000,             // Stack size (in bytes)
    NULL,              // Parameters
    1,                 // Task priority
    NULL,              // Task handle
    1                  // Core ID (0 for Core 0, 1 for Core 1)
  );

  // Check if the ESP32 woke up from deep sleep
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
    startSequence();
  } else {
    attachInterrupt(digitalPinToInterrupt(KEYCONTACT_PIN), handleD2Low, FALLING);
  }
}



void loop1(void *pvParameters) {
  while (true) {
    vTaskDelay(1);

   if (digitalRead (KEYCONTACT_PIN)){
    handleLightBehavior();
   }else{
    initiateHigh();
   }
      
  }
}
void handleLightBehavior() {

  if (mode.equalsIgnoreCase("default")) {
    digitalWrite(RELAY_LOWBEAM_PIN, LOW);

    if (digitalRead(HIGHBEAM_PIN)) {
      digitalWrite(RELAY_HIGHBEAM_PIN, LOW);
      isOnHighBeam = true;


    } else {
      digitalWrite(RELAY_HIGHBEAM_PIN, HIGH);
      isOnHighBeam = false;
    }


    if (digitalRead(RIGHT_SEIN_PIN)) {
      curenttime = millis();
      if ((curenttime - lastsein) >= (1000 / freqSeindefault)) {
        isOnsein = !isOnsein;
        digitalWrite(RELAY_RIGHTSIGNAL_PIN, isOnsein ? LOW : HIGH);
        lastsein = curenttime;
      }
    } else {
      digitalWrite(RELAY_RIGHTSIGNAL_PIN, HIGH);
    }


    if (digitalRead(LEFT_SEIN_PIN)) {
      curenttime = millis();
      if ((curenttime - lastsein) >= (1000 / freqSeindefault)) {
        isOnsein = !isOnsein;
        digitalWrite(RELAY_LEFTSIGNAL_PIN, isOnsein ? LOW : HIGH);
        lastsein = curenttime;
      }
    } else {
      digitalWrite(RELAY_LEFTSIGNAL_PIN, HIGH);
    }

    if (digitalRead(HORN_PIN)) {
      digitalWrite(RELAY_HORN_PIN, LOW);
    } else {
      digitalWrite(RELAY_HORN_PIN, HIGH);
    }
  }

  if (mode.equalsIgnoreCase("agresive")) {
    digitalWrite(RELAY_LOWBEAM_PIN, LOW);

    if (digitalRead(HIGHBEAM_PIN)) {
      curenttime = millis();
      if ((curenttime - lasthighbeam) >= (1000 / freqHighBeamagresive)) {
        isOnHighBeam = !isOnHighBeam;
        digitalWrite(RELAY_HIGHBEAM_PIN, isOnHighBeam ? LOW : HIGH);
        lasthighbeam = curenttime;
      }
    } else {
      digitalWrite(RELAY_HIGHBEAM_PIN, HIGH);
    }






    if (digitalRead(RIGHT_SEIN_PIN)) {
      curenttime = millis();
      if ((curenttime - lastsein) >= (1000 / freqSeinagresive)) {
        isOnsein = !isOnsein;
        digitalWrite(RELAY_RIGHTSIGNAL_PIN, isOnsein ? LOW : HIGH);
        lastsein = curenttime;
      }
    } else {
      digitalWrite(RELAY_RIGHTSIGNAL_PIN, HIGH);
    }


    if (digitalRead(LEFT_SEIN_PIN)) {
      curenttime = millis();
      if ((curenttime - lastsein) >= (1000 / freqSeinagresive)) {
        isOnsein = !isOnsein;
        digitalWrite(RELAY_LEFTSIGNAL_PIN, isOnsein ? LOW : HIGH);
        lastsein = curenttime;
      }
    } else {
      digitalWrite(RELAY_LEFTSIGNAL_PIN, HIGH);
    }

    if (digitalRead(HORN_PIN)) {
      curenttime = millis();
      if ((curenttime - lasthorn) >= (1000 / freqHornagresive)) {
        isOnHorn = !isOnHorn;
        digitalWrite(RELAY_HORN_PIN, isOnHorn ? LOW : HIGH);
        lasthorn = curenttime;
      }
    } else {
      digitalWrite(RELAY_HORN_PIN, HIGH);
    }
  }

  if (mode.equalsIgnoreCase("emergency")) {
    digitalWrite(RELAY_LOWBEAM_PIN, LOW);

    if (digitalRead(HIGHBEAM_PIN)) {
      curenttime = millis();
      if ((curenttime - lasthighbeam) >= (1000 / freqHighBeamemergency)) {
        isOnHighBeam = !isOnHighBeam;
        digitalWrite(RELAY_HIGHBEAM_PIN, isOnHighBeam ? LOW : HIGH);
        lasthighbeam = curenttime;
      }
    } else {
      digitalWrite(RELAY_HIGHBEAM_PIN, HIGH);
    }

    if (digitalRead(RIGHT_SEIN_PIN) || digitalRead(LEFT_SEIN_PIN)) {
      curenttime = millis();
      if ((curenttime - lastsein) >= (1000 / freqSeinemergency)) {
        isOnsein = !isOnsein;
        digitalWrite(RELAY_RIGHTSIGNAL_PIN, isOnsein ? LOW : HIGH);
        digitalWrite(RELAY_LEFTSIGNAL_PIN, isOnsein ? LOW : HIGH);
        lastsein = curenttime;
      }
    } else {
      digitalWrite(RELAY_RIGHTSIGNAL_PIN, HIGH);
      digitalWrite(RELAY_LEFTSIGNAL_PIN, HIGH);
    }

    if (digitalRead(HORN_PIN)) {
      curenttime = millis();
      if ((curenttime - lasthorn) >= (1000 / freqHornemergency)) {
        isOnHorn = !isOnHorn;
        digitalWrite(RELAY_HORN_PIN, isOnHorn ? LOW : HIGH);
        lasthorn = curenttime;
      }
    } else {
      digitalWrite(RELAY_HORN_PIN, HIGH);
    }
  }

  if (mode.equalsIgnoreCase("quite")) {
    digitalWrite(RELAY_LOWBEAM_PIN, HIGH);


    if (digitalRead(HIGHBEAM_PIN)) {
      digitalWrite(RELAY_HIGHBEAM_PIN, LOW);
      digitalWrite(RELAY_LOWBEAM_PIN, LOW);
    } else {
      digitalWrite(RELAY_HIGHBEAM_PIN, HIGH);
      digitalWrite(RELAY_LOWBEAM_PIN, HIGH);
    }

    if (digitalRead(RIGHT_SEIN_PIN)) {
      curenttime = millis();
      if ((curenttime - lastsein) >= (1000 / freqSeinquiete)) {
        isOnsein = !isOnsein;
        digitalWrite(RELAY_RIGHTSIGNAL_PIN, isOnsein ? LOW : HIGH);
        lastsein = curenttime;
      }
    } else {
      digitalWrite(RELAY_RIGHTSIGNAL_PIN, HIGH);
    }


    if (digitalRead(LEFT_SEIN_PIN)) {
      curenttime = millis();
      if ((curenttime - lastsein) >= (1000 / freqSeinquiete)) {
        isOnsein = !isOnsein;
        digitalWrite(RELAY_LEFTSIGNAL_PIN, isOnsein ? LOW : HIGH);
        lastsein = curenttime;
      }
    } else {
      digitalWrite(RELAY_LEFTSIGNAL_PIN, HIGH);
    }



    if (digitalRead(HORN_PIN)) {
      digitalWrite(RELAY_HORN_PIN, LOW);
    } else {
      digitalWrite(RELAY_HORN_PIN, HIGH);
    }
  }
}

void loop() {
  SellectorBtn.tick();



  handlemotor();
  if (trigger_change_mode) {
    debug(mode);
    trigger_change_mode = false;
  }
}

void handlemotor() {

  if (digitalRead(UPMOTOR_PIN) && !digitalRead(MOTORSENSOR_PIN)) {
    digitalWrite(RELAY_UPMOTOR_PIN, LOW);


  } else {
    digitalWrite(RELAY_UPMOTOR_PIN, HIGH);
  }

  if (digitalRead(DOWNMOTOR_PIN)) {
    digitalWrite(RELAY_DOWNMOTOR_PIN, LOW);


  } else {
    digitalWrite(RELAY_DOWNMOTOR_PIN, HIGH);
  }
}

void clickSellector() {
  mode = "agresive";
  trigger_change_mode = true;
}
void doubleclickSellector() {
  mode = "emergency";
  trigger_change_mode = true;
}

void longPressStartSellector() {
  start_pressed_timer_selector = millis();
  Serial.println("start");
}
void longPressStopSellector() {
  end_pressed_timer_selector = millis();

  total_pressed_timer_selector = end_pressed_timer_selector - start_pressed_timer_selector;
  debug(total_pressed_timer_selector);
  if (total_pressed_timer_selector >= 800 and total_pressed_timer_selector < 3000) {
    mode = "default";
    trigger_change_mode = true;

    return;
  }
  if (total_pressed_timer_selector >= 3000) {
    mode = "quite";
    trigger_change_mode = true;
    return;
  }
}
