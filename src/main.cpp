// Daan Landsman s1194532
// Lennart Gjaltema sxxxxxxx
#include <Arduino.h>

int main(void){

  return 0;
}

// De git werkt!! xx Lennart 18/10/2024 15:03
// Dat is heel mooi XOXO Daan 28/10/2024 10:06


//testcode gameshield heeft code van het geluid

/* Testcode voor de arduino gameshield

Met deze code kun je snel even testen ofdat je arduino gameshield goed in elkaar is gezet.
De snelheid van het knipperende display is gelinkt aan de stand van de potentiometer.
De IR LED knippert met dezelfde snelheid als het 7 segment display. Dit kan je zien met de meeste telefooncamera's
De buzzer kan getest worden door op de knop te drukken. Deze werkt met beide io 3 en 5 tegelijk, dus je kan de jumper op beide posities gebruiken

*/


#include <Wire.h>



#define addr  0x21  //PCF8574
#define addrb 0x39    //PCF8574AN

void writePattern(uint8_t b);
void sinit(void){

  init();
  
  
  pinMode(A1,INPUT_PULLUP);
  pinMode(6,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(5,OUTPUT);

  Wire.begin();

  DDRD  |= 1<<5
        |  1<<3;

  //Timer related shenanigans
  TCCR0B  =  1<<1    //CS01 = 1
          |   1<<6;   //FOC0B = 1

  TCCR0A  =   0<<1   //WGM01 = 1
          ;  //COMOB0 =1

  OCR0A = 255;
  OCR0B = 1;

    //Timer related shenanigans
  TCCR2B  =  1<<1    //CS01 = 1
          |   1<<0
          |   1<<6;   //FOC0B = 1

  TCCR2A  =   0<<1   //WGM01 = 1
          ;  

  OCR2A = 255;
  OCR2B = 1;

  


}

void writePattern(uint8_t b){

  Wire.beginTransmission(addr);
  Wire.write(b);
  Wire.endTransmission();

  Wire.beginTransmission(addrb);
  Wire.write(b);
  Wire.endTransmission();
}

int main(void){

  sinit();
  Serial.begin(9600);


  uint32_t patternCounter = millis();

  enum pattern{
    FULL,
    EMPTY
  } pat = EMPTY;

  enum bState{
    OPEN,
    DEBOUNCING
  } bState = OPEN;

  enum bTone{
    ON,
    OFF,
  } bTone = OFF;
  uint32_t toneOff = 0;
  uint32_t debouncing = millis();
  while(true){
    unsigned int pot = analogRead(A0);

    if(patternCounter+(0.005*pot*pot+100) < millis()){
      Serial.println(digitalRead(A1));
      patternCounter = millis();
      switch(pat){
        case FULL:
          digitalWrite(6,LOW);
          writePattern(0);
          pat = EMPTY;
          break;
        case EMPTY:
          digitalWrite(6,HIGH);
          writePattern(~0u);
          pat = FULL;
          break;
      }
    }    

    if(     !digitalRead(A1)
        &&  (bState==OPEN)
        &&  (debouncing < millis())  
        ){
      bState = DEBOUNCING;
      bTone = ON;
      toneOff = millis() + 5000;
      TCCR0A |= 1<<4;; //Turn on Pin 5
      TCCR2A |= 1<<4;

    //  tone(3,440,400);
      // tone(5,440,440);
      debouncing = millis()+30;
    }

    if(   digitalRead(A1) 
      &&  (bState == DEBOUNCING) 
      &&  (debouncing < millis())){
        debouncing = millis()+30;
        bState = OPEN;
      } 


    if(toneOff < millis() && bTone == ON){
      bTone = OFF;
      TCCR0A &= ~(1<<4);
      TCCR2A &= ~(1<<4);

    }

  }

}