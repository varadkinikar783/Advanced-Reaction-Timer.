#include <Arduino.h>
#include <EEPROM.h>

uint8_t ledPins[4] = {4, 5, 6, 3}; //4-6 == reaction leds. 3[3] == goled.
uint8_t buttonPins[5] = {8, 9, 10, 11, 12}; // 8-10[0-2] == reaction buttons. 11[3] == start button, 12[4] == history button
uint8_t state = 0; // 0 == prompt, 1 == start/history button press, 2 == countdown, 3 == reaction light up and capture, 
                    //4 == saveHistory, 5 == call history, 6 == cheated.
uint8_t BT = 60;
uint8_t count = 0;

unsigned long lastDebounceTime[5] = {0, 0, 0, 0, 0};
unsigned long randomInterval;
unsigned long randomLed;
unsigned long cheatStartTime = 0;
unsigned long reactionStart;
unsigned long reactionEnd;
unsigned long result;

bool currentState[5] = {HIGH, HIGH, HIGH, HIGH, HIGH};
bool preState[5] = {HIGH, HIGH, HIGH, HIGH, HIGH}; 

void prompt();
void answerPrompt(unsigned long now);
void countdown(unsigned long now);
void cheatCheck(unsigned long now);
void react(unsigned long randomLed, unsigned long now);
void saveResult(unsigned long result);
void showResult();

void setup(){
    if(EEPROM.read(0) != 60){
        for(int i = 1; i <= 255; i++){
            EEPROM.write(i, 0);
        }
        EEPROM.write(0, 60);
    }
    Serial.begin(115200);
    while(!Serial){ ; }

    for(int i = 0; i <= 3; i++){
        pinMode(ledPins[i], OUTPUT);
    }
    for(int i = 0; i <= 4; i++){
        pinMode(buttonPins[i], INPUT_PULLUP);
    }
    randomSeed(analogRead(A0));
}

void loop(){
    unsigned long now = millis();
    if(state == 0){
        prompt();
    }
    else if(state == 1){
        answerPrompt(now);
    }
    else if(state == 2){
        countdown(now);
    }
    else if(state == 3){
        react(randomLed, now);
    }
    else if(state == 4){
        saveResult(result);
    }
    else if(state == 5){
        showResult();
    }
    else if(state == 6){
        cheatCheck(now);
    }
}

void prompt(){
    Serial.println("What would you like to do?:");
    Serial.println("1. Check reaction time.(press right handside button)");
    Serial.println("                OR              ");
    Serial.println("2. Check your past reaction speeds");
    state = 1;
}

void answerPrompt(unsigned long now){
    for(int i = 3; i <= 4; i++){
        currentState[i] = digitalRead(buttonPins[i]);

        if(preState[i] != currentState[i]){
            lastDebounceTime[i] = now;
            preState[i] = currentState[i]; 
        }
        if((now - lastDebounceTime[i]) >= 70){
           if(currentState[i] == LOW){
                if(i == 3){
                    //Serial.println("Start button pressed");
                    Serial.println("Test begins in: ");
                    state = 2; 
                    break;
                }
                else if(i == 4){
                    //Serial.println("History button pressed.");
                    state = 5; 
                    break;
                }
            }
        }
    }
}

void countdown(unsigned long now){
    for(int i = 3; i >=1; i --){
        Serial.println(i);
        delay(1000);
    }
    randomInterval = random(800, 5001);
    
    randomLed = random(0, 3); 
    
    digitalWrite(ledPins[3], HIGH); // Turn on Go LED
    cheatStartTime = millis();
    state = 6;
}

void cheatCheck(unsigned long now){
    if(millis() - cheatStartTime <= randomInterval){
        for(int i = 0; i <= 2; i++){
            currentState[i] = digitalRead(buttonPins[i]);

            if(preState[i] != currentState[i]){
                lastDebounceTime[i] = now;
                preState[i] = currentState[i];
            }

            if((now - lastDebounceTime[i]) >= 70){
                if(currentState[i] == LOW){
                    Serial.println("Pressed too early.");
                    digitalWrite(ledPins[3], LOW);
                    state = 0;
                    break; 
                }
            }
        }
    }
    else{
        digitalWrite(ledPins[randomLed], HIGH); 
        reactionStart = millis();
        state = 3;
    }
}

void react(unsigned long randomLed, unsigned long now){
    if(now - reactionStart >= 50){
        digitalWrite(ledPins[randomLed], LOW);
    }

    for(uint8_t i = 0; i <= 2; i++){
        currentState[i] = digitalRead(buttonPins[i]);

        if(preState[i] != currentState[i]){
            lastDebounceTime[i] = now;
            preState[i] = currentState[i]; 
        }

        if((now - lastDebounceTime[i]) >= 70){
            if(currentState[i] == LOW && i == randomLed){
                reactionEnd = millis();
                digitalWrite(ledPins[3], LOW);
                digitalWrite(ledPins[randomLed], LOW);
                result = reactionEnd - reactionStart;
                Serial.print("Your time is: ");
                Serial.print(result);
                Serial.println(" ms.");
                state = 4;
                break;
            }
            else if(currentState[i] == LOW && i != randomLed){
                Serial.println("You pressed the wrong button.");
                digitalWrite(ledPins[3], LOW);
                digitalWrite(ledPins[randomLed], LOW);
                state = 0;
                break;
            }
         }
    }
}

void saveResult(unsigned long result){
    count = EEPROM.read(1);
    if(count > 10){
        count = 0;
        EEPROM.write(1, count);
    }
    EEPROM.put(2+count*sizeof(unsigned long), result);
    EEPROM.write(1, count+1);
    state = 0;
}

void showResult(){
    Serial.println("--- Past Reaction Speeds ---");
    
    for(int i = 0; i < 10; i++){
        unsigned long t;
        EEPROM.get(2 + (i * sizeof(unsigned long)), t);
        if(t != 0){
            Serial.print(i + 1);
            Serial.print(". ");
            Serial.print(t);
            Serial.println(" ms");
        }
    }
    delay(500);
    state = 0; 
}
