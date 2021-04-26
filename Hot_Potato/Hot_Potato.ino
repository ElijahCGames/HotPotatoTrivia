// LCD 
#include <LiquidCrystal.h>          //the liquid crystal library contains commands for printing to the display
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);   // tell the RedBoard what pins are connected to the display

// Time values
unsigned long previousMillis = 0;
const long interval = 1000; 

// Team times
int tb = 60;
int tr = 60;

// States
bool playing = false;
bool onBlue = true;
bool canAccept = true;

// Button
bool redKey = true;

// LED states
int bState = LOW;
int rState = LOW;

// Audio pin
int audioPin = 4;

// Bytes
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
  
  // Pin Modes
  pinMode(7, INPUT);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(audioPin,OUTPUT);

  // Serial Setup
  Serial.begin(9600);
  Serial.setTimeout(100);

  // Bytes for LCD
  lcd.createChar(1,u1);
  lcd.createChar(2,u2);
  lcd.createChar(3,u3);
  lcd.createChar(4,u4);
  lcd.createChar(5,u5);
  lcd.createChar(7,u7);
}

void loop() {
  // If Playing, run the game
  // If not, wait for red button
  if (playing){
    PlayGame();
  }else{
    // When red button pushed, start game.
    if(digitalRead(7) == HIGH){
      playing=true;
      redKey = false;
      tb = 60;
      tr = 60;

      tone(audioPin,300,300);
      bState = HIGH;
    }
  }
  Render();
}

// Plays the game, switching between each on control
void PlayGame(){
  if (Serial.available() > 0) {
    String IncomingByte = Serial.readString();
    float ib = IncomingByte.toFloat();

    // Buzz for Red
    if(ib > .7 && onBlue == true){
      onBlue = false;
      tone(audioPin,400,100);
      bState = LOW;
      rState = HIGH;

      canAccept = false;

    // Buzz for Blue
    }else if (ib < -0.7 && onBlue == false){
      onBlue = true;
      tone(audioPin,350,100);
      delay(150);
      tone(audioPin,400,100);
      bState = HIGH;
      rState = LOW;

      canAccept = false;
    }
  }

  // If the player got something wrong, push to take away their time
  if(digitalRead(7) == HIGH && redKey == true){
    redKey = false;
    tone(audioPin,200,200);
    takeAwayTime();
    
  }
  if(digitalRead(7) == LOW && redKey == false){
    redKey = true;
  }

  // Subtracts time from player
  subtractTime();

  // Ends games
  if(tb <= 0){
    Win(false);
  }

  if(tr <=0){
    Win(true);
  }
}

// Substract time on each tick
void subtractTime(){
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval){
    previousMillis = currentMillis;
    lcd.clear();
    
    if(onBlue){
      tb = tb - 1;
    }else{
      tr = tr - 1;
    }
  }
}

// Takes away time from the players
void takeAwayTime(){
  if(onBlue){
    if(tb < 5){
      Win(false);
      return;
    }
    tb -= 5;
  }else{
    if(tr < 5){
      Win(true);
      return;
    }
    tr -= 5;
    
  }
  
}

// Renders LCD and LED
void Render(){
  digitalWrite(2,bState);
  digitalWrite(3,rState);
  
  lcd.setCursor(2,0);
  writeNum(tr);

  lcd.setCursor(13,0);
  writeNum(tb);
}

// Win
void Win(bool blueWin){
  tone(audioPin,300,100);
  delay(150);
  tone(audioPin,350,100);
  delay(150);
  tone(audioPin,350,200);
  
  playing = false;
  if(blueWin){
    bState = HIGH;
    rState = LOW;
    
  }else{
    bState = LOW;
    rState = HIGH;
  }
}

// Writes upside down text on LCD
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
