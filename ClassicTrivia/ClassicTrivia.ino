
//  Trivia 
// Uses the controller as a buzzer
// Which side is tilts is a buzz
include <LiquidCrystal.h>          //the liquid crystal library contains commands for printing to the display
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);   // tell the RedBoard what pins are connected to the display

// Time for flashes
unsigned long previousMillis = 0;
const long interval = 1000;  

// Current time for each team
int tb = 120;
int tr = 120;

// Start time
int startTb = 120;
int startTr = 120;

// Buzzer States
bool buzzerOpen = false;

bool buzzed = false;
bool blueBuzzed = true;
bool firstBuzz = true;

// Button Keys
bool redKey = true;
bool greenKey = true;

//Button States
int bState = LOW;
int rState = LOW;

// Pin for audio
int audioPin = 4;

// Bytes for writing Number upside down
byte u1[8] = {
  B01110,
  B00100,
  B00100,
  B00100,
  B00100,
  B00110,
  B00100,
  B00000,
};

byte u2[8] = {
  B11111,
  B00010,
  B00100,
  B01000,
  B10000,
  B10001,
  B01110,
  B00000,
};

byte u3[8] = {
  B01110,
  B10001,
  B10000,
  B01000,
  B00100,
  B01000,
  B11111,
  B00000,
};

byte u4[8] = {
  B01000,
  B01000,
  B11111,
  B01001,
  B01010,
  B01100,
  B01000,
  B00000,
};

byte u5[8] = {
  B01110,
  B10001,
  B10000,
  B10000,
  B01111,
  B00001,
  B11111,
  B00000,
};

byte u7[8] = {
  B00010,
  B00010,
  B00010,
  B00100,
  B01000,
  B10000,
  B11111,
  B00000,
};


void setup() {

  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear();                      //clear the display

  // Initilizes pins
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(7,INPUT);
  pinMode(6,INPUT);
  pinMode(4,OUTPUT);

  // Serial Setup
  Serial.begin(9600);
  Serial.setTimeout(100);

  // Set Numbers for the LCD
  lcd.createChar(1,u1);
  lcd.createChar(2,u2);
  lcd.createChar(3,u3);
  lcd.createChar(4,u4);
  lcd.createChar(5,u5);
  lcd.createChar(7,u7);
}

// Loop
void loop() {
 // Buzzer avaliable for buzzes
 if(buzzerOpen){
  if(buzzed){
    // Buzz in
    SetAnswer();
  }else{
    // Wait for controller 
    ReadSerial();
  }

  // Countdown (based on states)
  Countdown();

 //Wait for red button
 }else{
    if(digitalRead(7) == HIGH && redKey == true){
      ClearBuffer();

      startTb = tb;
      startTr = tr;
      
      buzzerOpen=true;

      tone(audioPin,300,100);
      delay(150);
      tone(audioPin,350,100);
    }
 }

  // Reset buttons on pushes
  Buttons();
  // Renders lights and LCD
  Render();
}

// Waits for controller 
// Sets proper states based on direction an state
void ReadSerial(){
   if (Serial.available() > 0) {

    String IncomingByte = Serial.readString();
    float ib = IncomingByte.toFloat();

    // On Second Buzz
    if(!firstBuzz){
      // If blue buzzed first
      if(blueBuzzed){
        // When red buzzes
        if(ib>.5){
          blueBuzzed = false;
          buzzed = true;
          tone(audioPin,400,100);
          rState = HIGH;
          bState = LOW;
        }
      // If red buzzed first
      }else{
        // When blue buzzes
        if(ib<-.5){
          blueBuzzed = true;
          buzzed = true;

          tone(audioPin,350,100);

          rState = LOW;
          bState = HIGH;
        }
      }
    }else{
      // If waiting for the first buzz
      if(ib>.5){
        // Red Buzzed
        blueBuzzed = false;
        buzzed = true;
        
        tone(audioPin,400,100);
        
        rState = HIGH;
        bState = LOW;
      }
      if(ib<-.5){
        // Blue Buzzed
        blueBuzzed = true;
        buzzed = true;
        tone(audioPin,350,100);
        rState = LOW;
        bState = HIGH;
      }
    }
  }
}

// Gets answer from the buttons
void SetAnswer(){

  // Red Button
  if(digitalRead(7) == HIGH && redKey == true){
    redKey = false;
    tone(audioPin,200,100);
    if(firstBuzz){
       // First buzz wrong
       Serial.println("Buzz 1 Wrong");
       firstBuzz = false;
       buzzed = false;
    }else{
      // Second buzz wrong
      Serial.println("Buzz 2 Wrong");
      firstBuzz = true;
      buzzed = false;
      buzzerOpen = false;

      bState = LOW;
      rState = LOW;
    }
   
    
  }

  // Green Button
  // Resets the time on the proper team
  if(digitalRead(6) == HIGH && greenKey == true){
    greenKey = false;
    buzzerOpen = false;
    buzzed = false;
    firstBuzz = true;
    tone(audioPin,450,100);

    // Resets
    if(blueBuzzed){
      tb = startTb;
    }else{
      tr = startTr;
    }

    bState = LOW;
    rState = LOW;
  }
  
}

// Countdowns values
void Countdown(){
  unsigned long currentMillis = millis();

  // A Tick
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    lcd.clear();

    // If not buzzed
    // Countdown both teams
    if(!buzzed){
      if(!firstBuzz){
        if(blueBuzzed){
          tb = tb - 1;

          if(bState == LOW){
            bState = HIGH;
            tone(audioPin,150,100);
          }else{
            bState = LOW;
            tone(audioPin,100,100);
          }
        }else{
          tr = tr - 1;

          if(rState == LOW){
            rState = HIGH;
            tone(audioPin,150,100);
          }else{
            rState = LOW;
            tone(audioPin,100,100);
          }
        }
      }else{
        tb = tb - 1;
        tr = tr - 1;
      }
    // If buzzed, countdown buzzed team
    }else{
      if(blueBuzzed){
        tb = tb - 1;
      }else{
        tr = tr - 1;
      }    
    }

    // If a team gets to zero, win game
    if(tr <= 0){
      EndGame(true);
    }
    if(tb <= 0){
      EndGame(false);
    }
  }
}

// Resets buttons
void Buttons(){
  if(digitalRead(6) == LOW && greenKey == false){
    greenKey = true;
  }
  
  if(digitalRead(7) == LOW && redKey == false){
    redKey = true;
  }
}

// Render LCD and LEDs
void Render(){
  digitalWrite(2,bState);
  digitalWrite(3,rState);

  lcd.setCursor(2,0);
  writeNum(tr);

  lcd.setCursor(13,0);
  writeNum(tb);
}

// Ends game, freezes countdown
void EndGame(bool blueWin){

  tone(audioPin,300,100);
  delay(150);
  tone(audioPin,350,100);
  delay(150);
  tone(audioPin,350,200);
  
  buzzerOpen = false;
  if(blueWin){
    tb = 100;
    tr = 0;

    bState = HIGH;
    rState = LOW;
  }else{
    tb = 0;
    tr = 100;

    bState = LOW;
    rState = HIGH;
  }
  
}

// Clears the serial buffer to not read in the middle of a string
void ClearBuffer(){
  while(Serial.available() > 0){
    Serial.read();
  }
}

// Writes a num to the LCD
void writeNum(int num){
  if(num < 10){
    if(num == 8 || num == 0){
      lcd.print(String(num));
    }
    else if(num == 6){
      lcd.print("9");
    }
    else if(num == 9){
      lcd.print("6");
    }else{
      lcd.write(byte(num));
    }
  }else{
    String sn = String(num);
    for(int i = sn.length()-1; i>-1;i--){
      writeNum(sn.charAt(i) - '0');
    }
  }
}
