/*    millis tutorials from https://www.baldengineer.com/arduino-millis-examples.html
 * V1 Simulator https://wokwi.com/projects/360518778598814721
 *    Voltage to time -working
 *    OLED- working
 *    switch reading - working
 *    led on/off with ! -working
 *    stepping through segments - working
 *
 * V2 added Phase_duration(). iterating through array with "for" loop
 *      breaking out of function when value achieved
 *
 * V3 Change to TFT display
 *    decimal readout on display to minutes and  seconds
 */

/* For ESP32 Dev board (only tested with ILI9341 display)
// The hardware SPI can be mapped to any pins

//#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15  // Chip select control pin
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could connect to RST pin)
//#define TFT_RST  -1  // Set TFT_RST to -1 if display RESET is connected to ESP32 board RST
*/

//libraies
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define BTN_PIN 5
#define TFT_DC 2
#define TFT_CS 15
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


//*****Pin information************************
// pin allocation
const int outpin = LED_BUILTIN; 
const int potPin = 34;
const int SwitchPin = 13;

// variable for storing the potentiometer value
int potValue = 0;
int Mapped_Time =0;


//******Time variables*********
int  run_time = 30;
unsigned long duration ;
int dispaly_duration_m =0;
int dispaly_duration_s =0;
// Grab snapshot of current time, this keeps all timing
// consistent, regardless of how much code is inside the next if-statement
unsigned long currentMillis;
unsigned long pastMillis;

// Tracks the last time event fired
unsigned long previousSegmentMillis=0;
unsigned long previousToggleMillis=0;

//Segment times
 unsigned long Segment_Time ;
 unsigned long Segment1_Time ;
 unsigned long Segment2_Time ;
 unsigned long Segment3_Time ;
 unsigned long Segment4_Time ;

// On and Off Times (as int, max=32secs)
 unsigned long onTime = 500;
 unsigned long offTime = 500;

// Interval is how long we wait
int outpin_interval = onTime;
int Segment_invervals = 10;
unsigned long Segment_duration;
unsigned long phase_duration;

//**********States****************
// Used to track if outpin should be on or off
boolean outpin_state = true;
boolean Switch_state = false;
boolean Switch_been_pressed = false;
boolean delayStart_state = false;
boolean Toggle1_state = false;
boolean Toggle2_state = false;
boolean Toggle3_state = false;
boolean Toggle4_state = false;
boolean ToggleOff_state = false;

//*****************************

String functionName;
//***********************************
//*****Setup***************

// Usual Setup Stuff
void setup() {
  Serial.begin(4800);
  pinMode(outpin, OUTPUT);
  pinMode(SwitchPin, INPUT_PULLUP);

  tft.begin();
  tft.setRotation(1);

  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("Hello");

 delay(2000);
}
//***********************
//*****loop**************
void loop() {
  
   // Grab snapshot of current time, this keeps all timing
  // consistent, regardless of how much code is inside the next if-statement
  currentMillis = millis();

  potValue = analogRead(potPin);
  Mapped_Time = map(potValue, 0, 4095, 0, (run_time *60));
  Switch_state = digitalRead(SwitchPin);
  duration = Mapped_Time*( 1000); //minutes duration in milli
  dispaly_duration_m = (Mapped_Time / ( 60)); //minutes duration in decimal
  dispaly_duration_s = (Mapped_Time % ( 60)); //seconds duration in decimal
  

    if (Switch_state == false)
      {
       // duration = Mapped_Time*(60* 1000); //minutes duration in milli
        Segment_Time = (duration /4);
        Phase_duration();
        onTime = phase_duration;
        offTime = phase_duration;
        pastMillis = millis();
      
        Switch_been_pressed = true;
       
       
      }

  if (Switch_been_pressed)
      {
        digitalWrite(outpin, outpin_state);
        delayStart();
        Display_OLED();
      }

  else {
        Display_Start_OLED();
       }  
 
   Serial_Print();
   
}
//**********************************
void delayStart()
  {
    if (currentMillis - pastMillis >= phase_duration )
      {choose_Segment();
       delayStart_state = false;
      }
    else{delayStart_state = true;}
  }

void choose_Segment()
{
    //calculate segment times
    Segment1_Time = Segment_Time + (pastMillis -phase_duration);
    Segment2_Time = Segment1_Time + Segment_Time;
    Segment3_Time = Segment2_Time + Segment_Time;
    Segment4_Time = Segment3_Time + Segment_Time;

  if ((unsigned long)(currentMillis - previousSegmentMillis) >= Segment4_Time)
    {   toggle_Off();  }
  
   if ((unsigned long)(currentMillis - previousSegmentMillis) >= Segment3_Time 
        && (unsigned long)(currentMillis - previousSegmentMillis) <= Segment4_Time)
    {   toggle_Output4();  }
   

  if ((unsigned long)(currentMillis - previousSegmentMillis) >= Segment2_Time
         && (unsigned long)(currentMillis - previousSegmentMillis) <= Segment3_Time)
    {   toggle_Output3();  }
   
  
    if ((unsigned long)(currentMillis - previousSegmentMillis) >= Segment1_Time 
        && (unsigned long)(currentMillis - previousSegmentMillis) <= Segment2_Time)
    {   toggle_Output2();  }

  if ((unsigned long)(currentMillis - previousSegmentMillis) >= 0 
        && (unsigned long)(currentMillis - previousSegmentMillis) <= Segment1_Time)
    {
         toggle_Output1(); 
        //previousSegmentMillis = currentMillis;
    }


}

void toggle_Output1()
  {  
    functionName = "toggle_Output1"; 
    Toggle1_state = true;
    // Compare to previous capture to see if enough time has passed
    if ((unsigned long)(currentMillis - previousToggleMillis) >= outpin_interval)
    {
    // Change wait interval, based on current outpin state
    if (outpin_state) 
      {
         // outpin is currently on, set time to stay off
         outpin_interval = offTime;
      }
    else 
      {
         // outpin is currently off, set time to stay on
         outpin_interval = onTime;
      }
      // Toggle the outpin's state
      outpin_state = !(outpin_state);
      // Save the current time to compare "later"
      previousToggleMillis = currentMillis;
   }
  }

  void toggle_Output2()
  {  
    functionName = "toggle_Output2"; 
    Toggle2_state = true;
    // Compare to previous capture to see if enough time has passed
    if ((unsigned long)(currentMillis - previousToggleMillis) >= outpin_interval)
    {
    // Change wait interval, based on current outpin state
    if (outpin_state) 
      {
         // outpin is currently on, set time to stay off
         outpin_interval = (offTime / 2);
      }
    else 
      {
         // outpin is currently off, set time to stay on
         outpin_interval = (onTime );
      }
      // Toggle the outpin's state
      outpin_state = !(outpin_state);
      // Save the current time to compare "later"
      previousToggleMillis = currentMillis;
   }
  }

   void toggle_Output3()
  {  
    functionName = "toggle_Output3"; 
    Toggle3_state = true;
    // Compare to previous capture to see if enough time has passed
    if ((unsigned long)(currentMillis - previousToggleMillis) >= outpin_interval)
    {
    // Change wait interval, based on current outpin state
    if (outpin_state) 
      {
         // outpin is currently on, set time to stay off
         outpin_interval = offTime;
      }
    else 
      {
         // outpin is currently off, set time to stay on
         outpin_interval = (onTime / 2);
      }
      // Toggle the outpin's state
      outpin_state = !(outpin_state);
      // Save the current time to compare "later"
      previousToggleMillis = currentMillis;
   }
  }

  void toggle_Output4()
  {  
    functionName = "toggle_Output4"; 
    Toggle4_state = true;
    // Compare to previous capture to see if enough time has passed
    if ((unsigned long)(currentMillis - previousToggleMillis) >= outpin_interval)
    {
    // Change wait interval, based on current outpin state
    if (outpin_state) 
      {
         // outpin is currently on, set time to stay off
         outpin_interval = (offTime /3);
      }
    else 
      {
         // outpin is currently off, set time to stay on
         outpin_interval = (onTime / 3);
      }
      // Toggle the outpin's state
      outpin_state = !(outpin_state);
      // Save the current time to compare "later"
      previousToggleMillis = currentMillis;
   }
  }

  void toggle_Off()
  {  
    functionName = "toggle_Off"; 
    ToggleOff_state = true;
    outpin_state = false;
  }

void Phase_duration()
{
  unsigned long Phase_min = 20000;
  unsigned long Phase_max = 30000;

  boolean result_true = false;
  
  int PhaseArray[9] = {10,9,8,7,6,5,4,3,2};

  for (int i = 0; i<9; i++ )
  {
    unsigned long result = Segment_Time/PhaseArray[i];
    Serial.print(PhaseArray[i]);Serial.print(" -"); Serial.println(result);
    if (result >= Phase_min && result <= Phase_max )
        {
         phase_duration = result;
         result_true = true;
         //Serial.print("phase_duration "); Serial.println(phase_duration);
        //Serial.println("******** ");
        }

    if (result_true)
      {
        phase_duration = result;
        break;
      }     
 }

}


void Serial_Print()
  {
    Serial.print("Function "); Serial.println(functionName);
    //Serial.print("Switch_state "); Serial.println(Switch_state);
    Serial.print("Switch_been_pressed "); Serial.println(Switch_been_pressed);
    //Serial.print("potValue "); Serial.println(potValue);
    //Serial.print("pinFloat "); Serial.println(pinFloat);
    
    
    Serial.print("Mapped_Time "); Serial.println(Mapped_Time);
    Serial.print("duration "); Serial.println(duration);
    Serial.print("phase_duration "); Serial.println(phase_duration);
    Serial.print("pastMillis "); Serial.println(pastMillis);
    Serial.print("delay "); Serial.println(currentMillis - pastMillis);
    Serial.print("delayStart_state "); Serial.println(delayStart_state);
    
    Serial.print("Segment_Time "); Serial.println(Segment_Time);
    //Serial.print("Segment1_Time "); Serial.println(Segment1_Time);
    //Serial.print("Segment2_Time "); Serial.println(Segment2_Time);
    //Serial.print("Segment3_Time "); Serial.println(Segment3_Time);
    //Serial.print("Segment4_Time "); Serial.println(Segment4_Time);
    Serial.print("currentMillis "); Serial.println(currentMillis);
    //Serial.print("previousSegmentMillis "); Serial.println(currentMillis - previousSegmentMillis);
    //Serial.print("previousToggleMillis ");Serial.println(currentMillis - previousToggleMillis);
    //Serial.print("onTime ");Serial.println(onTime);
    // Serial.print("offTime ");Serial.println(offTime);
    Serial.print("outpin_interval ");Serial.println(outpin_interval);
    Serial.print("outpin_state ");Serial.println(outpin_state);
    Serial.print("--------------- ");
    
  }

//one option of dispaly
/*
  void Display_OLED()
  {
     tft.fillScreen(ILI9341_BLACK);
     tft.setTextColor(ILI9341_WHITE);
     tft.setTextSize(3);
     tft.setCursor(0, 0);
     if (outpin_state)
    {
       tft.invertDisplay(true);
    }
  else{  
       tft.invertDisplay(false);
      }
   tft.setCursor(0, 0);  tft.print("Func ");
   tft.setCursor(130,0 ); tft.print(functionName);
   tft.setCursor(0, 40);  tft.print("Switch pressed ");
   tft.setCursor(130,40 ); tft.print(Switch_been_pressed);
   tft.setCursor(0, 60);  tft.print("Time ");
   tft.setCursor(134,60 ); tft.print(currentMillis);
   tft.setCursor(0, 85);  tft.print("outpin ");
   tft.setCursor(130,85 ); tft.print(outpin_state);
   //tft.display();
  delay(20); 
  }
*/

//second option of display
void Display_OLED()
  {
     tft.fillScreen(ILI9341_BLACK);
     tft.setTextColor(ILI9341_WHITE);
     tft.setTextSize(3);
     tft.setCursor(0, 0);
     if (outpin_state)
    {
       tft.invertDisplay(true);
    }
  else{  
       tft.invertDisplay(false);
      }
   tft.setCursor(0, 0);  tft.print("Func ");
   //tft.setCursor(130,0 ); 
   tft.println(functionName);
   //tft.setCursor(0, 40);  
   tft.print("Switch pressed ");
   //tft.setCursor(130,40 ); 
   tft.println(Switch_been_pressed);
   //tft.setCursor(0, 60);  
   tft.print("Time ");
   //tft.setCursor(134,60 ); 
   tft.println(currentMillis);
   //tft.setCursor(0, 85); 
    tft.print("outpin ");
   //tft.setCursor(130,85 ); 
   tft.println(outpin_state);
   //tft.display();
 //delay(20); 
  }

  void Display_Start_OLED()
  {
   tft.fillScreen(ILI9341_BLACK);
   tft.setTextColor(ILI9341_WHITE);
   tft.setTextSize(4);      // Normal 1:1 pixel scale
   tft.setCursor(0, 0);  tft.println("Time ");
   /* added for testing only
   //tft.setCursor(24,20 ); 
   tft.println(pinFloat);
   //tft.setCursor(24,20 );
   tft.println(potValue); 
   tft.println(Mapped_Time);
   //tft.setCursor(4,40 ); 
   */
   tft.println("");
   tft.print(dispaly_duration_m);
   tft.print(":");
   tft.println(dispaly_duration_s);
  
  
   //tft.display();
  delay(10); 
  }
  
