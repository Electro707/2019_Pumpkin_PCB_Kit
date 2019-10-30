/************************************************************************************************************************
 *  Touch Pumpkin Attiny85 Code
 *  By Jamal Bouajjaj
 *
 *  This program controls the LEDs on the 2019 UNH Makerspace Touch Pumpkin soldering kit. This program will wait for
 *    a capacitive touch to reach above a threshold, in which case it will switch the LED mode to fading, blinking, and
 *    off sequentially.
************************************************************************************************************************/
#include <CapacitiveSensor.h>     // Import the library
#define LEDPIN PB1                // Define the LED pin
#define CAPSENSE_SEND PB4         // Define the capacitive touch send pin
#define CAPSENSE_REC  PB3         // Define the capacitive touch recieve pin

int mode = 0;                     // Store the current LED mode
bool last_state = false;          // Store the last capacitive touch state ( pressed or not pressed)
int pwm_val = 0;                  // Store the PWM value during fading mode
bool pwm_direction = false;       // Store the directions of the changing PWM during fading mode ( fade up or down)
int old_millis = 0;               // Store the previous time (millis()) during blinking and fading mode
int current_time = 0;             // Store the current time (millis()) for comparision with old_millis

CapacitiveSensor   cs = CapacitiveSensor(CAPSENSE_SEND, CAPSENSE_REC);   // Create the CapacitiveSensor class

void setup()                    
{
   cs.set_CS_AutocaL_Millis(0xFFFFFFFF);     // Turn off autocalibrate on the capacitive touch channel
   pinMode(PB1, OUTPUT);                      // Set the LED pin as an output
   // The following code is to save power on unused preiferals
   ACSR |= (1<<ACD);              // Turn off Analog Comparator
   ADCSRA &= (1<<ADEN);           // Turn off ADC
   PRR |= (1<<PRADC);             // Shut down ADC
}

void loop()                    
{
    long cs_val =  cs.capacitiveSensor(30);     // Detect the arbritrary capacitance on the copper pad

    // If above threshold and previous state is off, increment the operating mode
    if(cs_val > 110){
      if(last_state == false){    // If pressed and the previous state is off, change the mode
        last_state = true;
        pwm_val = 0;
        pwm_direction = 0;
        mode++;
        if(mode > 2){
          mode = 0;
        }
        digitalWrite(LEDPIN, 0);
        if(mode == 2){
          digitalWrite(LEDPIN, 1);
        }
        old_millis = millis();
        delay(150);
      }
    }
    else{   // If the previous press state is on and the pad isn't pressed, revert the previous pressed state to false
      if(last_state == true){
        last_state = false;
        delay(50);
      }
    }

    // Drive the LEDs based on the current operating mode
    switch(mode){
      case 0:       // Mode 0 <- Off
        digitalWrite(LEDPIN, 0);
      break;
      
      case 1:       // Mode 1 <- Fading
        current_time = millis();
        if(current_time - old_millis >= 20){
          old_millis = millis();
          if(pwm_direction){
            if(pwm_val == 0){
              pwm_direction = false;
            }
            else{
              pwm_val--;
            }
          }
          else{
            if(pwm_val >= 100){
              pwm_direction = true;
            }
            else{
              pwm_val++;
            }
          }
        }
        analogWrite(LEDPIN, pwm_val);
       
      break;
      case 2:     // Mode 2 <- Blinking
        current_time = millis();
        if(current_time - old_millis >= 1000){
          digitalWrite(LEDPIN, !digitalRead(LEDPIN));
          old_millis = millis();
        }
      break;
    }
}
