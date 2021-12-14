/*
  This is the code to run the planting module in Mr. Shepherd's ABE 404 Class.

 
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for four lines of 20 characters
LiquidCrystal_I2C lcd(0x27, 20, 4);

//declare various useful variables
const int limitpin = 12;
bool isPlanting = 0;
float spacing = 0.2;
const int sleeppin = 7;
bool awake = 0;

//rotary encoder declarations
const int button = 4;
const int rotA = 3;
const int rotB = 2;

//Menu declarations
String mainMenu[] = {
  "      Main Menu",
  "Spacing: " + String(spacing),
  "Run",
  "Table Run",
  "Reset",
  "next thing",
  "haha sex",
  "jibberish",
  "more shit"
};



void setup() {

  //Initiate LCD screen
  lcd.begin();
  lcd.backlight();

  //Start asleep
  stopmotor();
  
  //Sets the speed sensor input
  pinMode(limitpin, INPUT);

  //sets the sleep pin
  pinMode(sleeppin, OUTPUT);

  //Rotary encoder inputs
  pinMode(button,INPUT_PULLUP);
  pinMode(rotA,INPUT_PULLUP);
  pinMode(rotB,INPUT_PULLUP);
  
  //This sets pin 9 up for custom pulsing
  pwmSetup();

  //FOR DEBUGGING
  Serial.begin(9600);
}

void loop() {

  //This main loop essentially runs the main menu. browseMainMenu() allows you to choose a main menu option, which sends you to one of the three functions below.
  switch (browseMainMenu()){
    case 1:
      //This function allows you to change the planter's spacing setting
      spacingset();
      break;
    case 2:
      //This is the demonstration function. It takes speed data and uses it to control the speed of the seed meter.
      run();
      break;
    case 3:
      //This is the function that allows us to simulate moving at a constant speed. It is used to calibrate the device.
      tableRun();
      break;
  }
}

void spacingset(){

  //This section prints the display
  lcd.clear();
  lcd.setCursor (0,0);
  lcd.print("    Spacing Set");
  lcd.setCursor (0,1);
  lcd.print("Spacing: " + String(spacing));
  lcd.setCursor (0,2);
  lcd.print("Set Spacing? ");
  lcd.setCursor (0,3);
  lcd.print("Return");

  //specify starting cursor position
  int selected = 1;

  lcd.blink();

  do{

    //Don't exit the page
    if (selected < 1){selected = 1;}
    if (selected > 3){selected = 3;}

    //Set the cursor to the selected location
    lcd.setCursor (0,selected);

    //switch selection using rotary encoder
    switch (waitForAction()){
      case 'a':
        selected --;
        break;
      case 'b':
        selected ++;
        break;
      case 'x':
        //When the encoder is pressed, it either returns you to the main menu or lets you change the spacing setting
        switch (selected){
          case 2:
            //Update display
            lcd.setCursor (0,2);
            lcd.print("Set Speed: " + String(spacing));
            //This tag lets you bypass the switchcase with a goto so that the rotary encoder can continually change the spacing setting until it is again pressed.
            TableSpacingSetter:
            //This third and final switch actually does the variable changes
            switch (waitForAction()){
              case 'a':
                spacing = spacing -.02;
                //Update display
                lcd.setCursor (0,2);
                lcd.print("Set Spacing: " + String(spacing));
                delay(5);
                goto TableSpacingSetter;
                break;
              case 'b':
                spacing = spacing +.02;
                //Update display
                lcd.setCursor (0,2);
                lcd.print("Set Spacing: " + String(spacing));
                delay(5);
                goto TableSpacingSetter;
                break;
              case 'x':
                lcd.setCursor (0,1);
                lcd.print("Spacing: " + String(spacing));
                lcd.setCursor (0,2);
                lcd.print("Set Spacing?       ");
                lcd.setCursor (0,1);
                mainMenu[1] = "Spacing: " + String(spacing);
                break;
              }
          case 3:
            return;

      }
      break;
    }

    //This is really important lol
    delay(100);
  }
  while (1);
}

void tableRun(){

  //Start out stationary
  float velocity = 0;

  //Write the display
  lcd.clear();
  lcd.setCursor (0,0);
  lcd.print("    Table Run");
  lcd.setCursor (0,1);
  lcd.print("Speed: " + String(velocity));
  lcd.setCursor (0,2);
  lcd.print("Set Speed? ");
  lcd.setCursor (0,3);
  lcd.print("Start     Return");

  //specify starting position
  int selected = 1;

  lcd.blink();

  do{

    //Don't exit the page
    if (selected < 1){selected = 1;}
    if (selected > 4){selected = 4;}

    //This allows you to use the return option
    if (selected<4){
      lcd.setCursor (0,selected);
    }
    else{
      lcd.setCursor (10,3);
    }
    
    //switch selection using rotary encoder
    switch (waitForAction()){
      case 'a':
        selected --;
        break;
      case 'b':
        selected ++;
        break;
      case 'x':
        switch (selected){
          case 2:
            //Update display
            lcd.setCursor (0,3);
            lcd.print("Start     Return");
            lcd.setCursor (0,2);
            lcd.print("Set Speed: " + String(velocity));
            stopmotor();
            TableSpeedSetter:
            switch (waitForAction()){
              case 'a':
                velocity = velocity -.05;
                //Update display
                lcd.setCursor (0,2);
                lcd.print("Set Speed: " + String(velocity));
                delay(5);
                goto TableSpeedSetter;
                break;
              case 'b':
                velocity = velocity +.05;
                //Update display
                lcd.setCursor (0,2);
                lcd.print("Set Speed: " + String(velocity));
                delay(5);
                goto TableSpeedSetter;
                break;
              case 'x':
                lcd.setCursor (0,1);
                lcd.print("Speed: " + String(velocity));
                lcd.setCursor (0,2);
                lcd.print("Set Speed?       ");
                lcd.setCursor (0,1);
                break;
              }
              break;
            case 4:
              stopmotor();
              return;
              break;
            case 3:
              if (awake == 1){
              stopmotor(); 
              lcd.setCursor (0,3);
              lcd.print("Start     Return");
              }
              else {
              setspeed(velocity,spacing);
              lcd.setCursor (0,3);
              lcd.print("Stop      Return");
              }
              break;
      }
      break;
    }
    //This is really important lol
    delay(100);
  }
  while (1);
}


void run(){
  //This sets up the display with labels
  lcd.clear();
  lcd.setCursor (0,0);
  lcd.print("Speed: ");
  lcd.setCursor (0,1);
  lcd.print("Spacing: ");
  lcd.setCursor (0,2);
  lcd.print("Skip %: ");
  lcd.setCursor (0,3);
  lcd.print("Planting: OFF");
  
  setspeed(6,.3);
  delay(1200);
  stopmotor();
  do{
    float velocity = getSpeed();
    //set the speed of the meter to the speed of the wheel
    setspeed(velocity,spacing);
    //Serial.println("HI!");
    delay(5);
    
    //Sends information to the display
    //velocity float, seed rate int, skip % float, planting boolean
    //Update numbers
    lcd.setCursor (7,0);
    lcd.print(velocity);
    lcd.setCursor (9,1);
    lcd.print(spacing);
    lcd.setCursor (8,2);
    lcd.print(0);
    lcd.setCursor (10,3);
  
    //Write whether planter is planting
    if (awake == 1)
    {
      lcd.print("ON ");
    }
    else
    {
      lcd.print("OFF");
    }
      delay(50);
    }
    
  while (1);
}

//this function returns the estimated speed of the wheel
float getSpeed(){
  //get initial time
  float start = millis();
  float finish = 0;
  //intialize counter to something that returns zero when wheel is stationary
  int count = -2;

  //this counts the number of times the clicker clicks in a second
  do{
  count++;
  pulseIn(limitpin,HIGH,1000000);
  finish = millis();
  delay(5);
  }
  while (finish-start<1000);
  Serial.println(count);
  delay(5);
  //this does math and reports the velocity in m/s
  float velocity = count/10.;
  if (velocity > 3){velocity = 3;}
  return (velocity);
}

void setspeed(float velocity, float space){
  //takes speed between 0-3 m/s and rate between 0.2-0.4 m apart where rate is distance
  if (velocity == 0){stopmotor();}
  else {startmotor();}
  
  //This should be the seed drop rate
  float frequency = velocity/space;

  //ICR1 is the number the clock counts to between pulses
  //This constant supplies the correct clock top for the given seed frequency
  ICR1 = 43000/frequency;
  Serial.println("The frequency is ");
  Serial.println(frequency);
  delay(5);
  //This is the pulse width, nothing I've seen indicates that it's particularly important
  OCR1A = 40;
}

//This function does a ton of stuff that I don't fully understand, but it currently works
void pwmSetup(){
//This is the pwm stuff, fiddling is not advised

  // Stop Timer/Counter1
  TCCR1A = 0;  // Timer/Counter1 Control Register A
  TCCR1B = 0;  // Timer/Counter1 Control Register B
  TIMSK1 = 0;   // Timer/Counter1 Interrupt Mask Register
 
  ICR1 = 62000;  // setting initial count cap (why I do not know)

  OCR1A = 0;  // Default Pin 9 to 0% PWM
  OCR1B = 0;  // Default Pin 10 to 0% PWM

  // Set to Timer/Counter1 to Waveform Generation Mode 14: Fast PWM with TOP set by ICR1
  TCCR1A |= (1 << WGM11);
  TCCR1B |= (1 << WGM13) | (1 << WGM12);

  // Set clock prescale to 8 for correct PWM frequency range for step motors
  TCCR1B |= (1 << CS11);
  
  // Enable PWM on Pin 9
  TCCR1A |= (1 << COM1A1);
  pinMode(9, OUTPUT);
}

//This function waits for a signal from the rotary encoder and returns what the signal is
char waitForAction(){
  do
  {
    //Get values.. they are flipped because, as pullup, they default to HIGH
    bool buttonpressed = !digitalRead(button);
    bool rotAActive = !digitalRead(rotA);
    bool rotBActive = !digitalRead(rotB);
    
    if (buttonpressed) 
    { 
      delay(50);
      buttonpressed = !digitalRead(button);
      if (buttonpressed == 1) 
      {
        delay(200);
        return 'x';
      }
      buttonpressed = 0;
      rotAActive = 0;
      rotBActive = 0;
      break;
    }
  
    if (rotAActive)
    {
      do
      {
        delay(1);
      }
      while (!digitalRead(rotB)==0);
      buttonpressed = 0;
      rotAActive = 0;
      rotBActive = 0;
      return 'a';
      break;
    }
  
    if (rotBActive)
    {
      do
      {
        delay(1);
      }
      while (!digitalRead(rotA)==0);
      buttonpressed = 0;
      rotAActive = 0;
      rotBActive = 0;
      return 'b';
      break;
    }
    
    delay(1);
  }
  while(1);
}

//This takes your location on the main menu and prints the menu for you
void printmain(int location, int cursepos){

  lcd.clear();
  
  //print current screen
  for (int i = 0;i<4;i++)
  {
    lcd.setCursor (0,i);
    lcd.print(mainMenu[i+location]);
  }

  //place cursor correctly
  lcd.setCursor(0,cursepos);
}

int browseMainMenu()
{

  //specify starting position and declare verily
  int selected = 1;
  int place = 0;

  //This prints the main menu
  printmain(place, selected);
  
  lcd.blink();
  
  do
  {

    //switch selection using rotary encoder
    switch (waitForAction()){
      case 'a':
        selected --;
        break;
      case 'b':
        selected ++;
        break;
      case 'x':
        lcd.noBlink();
        return selected+place;
        break;
    }
  
    //Scroll when at the edge of the screen
    if (selected > 3){
      selected = 3;
      place ++;
    }
    else if (selected < 0){
      selected = 0;
      place--;
    }
  
    //Don't scroll outside the menu, I can't read japanese
    if (place > 5){place = 5;}
    if (place < 0){place = 0;}
    
    printmain(place,selected);
  
    //Don't scroll too fast
    delay(100);
  }
  while (1);
}

void stopmotor(){
  //I'm pretty sure this will do the trick
  digitalWrite(sleeppin,LOW); 
  awake = 0;
}

void startmotor(){
  //Start motor
  digitalWrite(sleeppin,HIGH);
  awake = 1;
}
