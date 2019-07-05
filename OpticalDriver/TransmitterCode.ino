#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <avr/pgmspace.h>


const int X_pin = A0;                       // Analog output of horizontal joystick pin
const int Y_pin = A1;                       // Analog output of vertical joystick pin
const int selButton = 2;                    // select button pin of joystick
const int buttonPin = 3;                    // allows to switch between sticko mode and sensor mode
const int leftButton = 5;                   // button pin for left click in sensor mode
const int rightButton = 6;                  // button pin for right click in sensor mode

int buttonState = 0;                        // intital value for button mode
int buttonModeFlag = 0;                     // create flag if button is 0 or 1

byte stickoTransmitArray[6];                // create arrays for each mode for data to sent in
byte sensorTransmitArray[9];                  

int xZero, yZero;                           // initial values during setup
int yValue, xValue;                         // stores current anaolg output of each axis
int switchValue;                            // stores current digital output of joystick switch
int joystick_sense = 200;                   // joystick sensivtity 

// Transciever settings
RF24 radio(7,8);                            //CSN, CE pins
const byte address[6] = "00001";            // how the 2 transcievers will communicate

// Sensor settings
int sensor_sense = 20;                      // sensitivity for sensor
int x_sense, y_sense;                       // value after adjust for sensitivity 
int leftClick, rightClick;                  // stores output for left and right buttons
int scrollValue,scrollNumber;               // using the y-axis of joystick for scrolling
int scroll_sense = 500;                     // value for scrolling using y-axis joystick

void setup()
{
  Serial.begin(115200);
  
  // Button setup
  pinMode(buttonPin, INPUT);                 // set button to switch mode
  pinMode(leftButton, INPUT);                // set button for left clicking
  pinMode(rightButton, INPUT);              // set button for right clicking

  // Joystick setup
  pinMode(X_pin, INPUT);                    // setting analog pins as inputs
  pinMode(Y_pin, INPUT);
  pinMode(selButton, INPUT);                // set button select pin as input
  digitalWrite(selButton, HIGH);            // set button select as high
  delay(500);                               // create delay for outputs
  xZero = analogRead(X_pin);                // read inital values
  yZero = analogRead(Y_pin); 

  // Sensor setup
  pinMode (ncs, OUTPUT);
  pinMode(Motion_Interrupt_Pin, INPUT);
  digitalWrite(Motion_Interrupt_Pin, HIGH);
  attachInterrupt(9, UpdatePointer, FALLING);
  SPI1.begin();
  SPI1.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
  performStartup();  
  delay(5000);
  dispRegisters();
  initComplete=9;

  // Transciever setup for transmitting
  radio.begin();
  radio.openWritingPipe(address);     
  radio.setPALevel(RF24_PA_MIN);            //set the power amplifer level
  radio.stopListening();                    // sets it as transmitter
  
}

union                                       // struct that converts integers to bytes
{
  int i;
  byte b[2];
}converter;

void sensor_mode(){                         // sensor mode
  
  leftClick = !digitalRead(leftButton);     // invert it for clicking logic on reciever code
  rightClick = !digitalRead(rightButton);
  scrollValue = analogRead(Y_pin) - yZero;  // get y value of joystick  
  
  currTime = millis();
  
  if(currTime > timer){    
    timer = currTime + 2000;
    }
    
  if(currTime > pollTimer){
    UpdatePointer();
    xydat[0] = convTwosComp(xydat[0]);
    xydat[1] = convTwosComp(xydat[1]);
      if(xydat[0] != 0 || xydat[1] != 0){
        }
    pollTimer = currTime + 20;
    }

    xValue = xydat[0];
    yValue = xydat[1];

    x_sense = xValue / sensor_sense;
    y_sense = yValue / sensor_sense;
    scrollNumber = scrollValue / scroll_sense;
  
  converter.i = buttonModeFlag;
  sensorTransmitArray[0] = converter.b[0];

  converter.i = x_sense;
  sensorTransmitArray[1] = converter.b[0];
  sensorTransmitArray[2] = converter.b[1];

  converter.i = y_sense;
  sensorTransmitArray[3] = converter.b[0];
  sensorTransmitArray[4] = converter.b[1];

  converter.i = scrollNumber; 
  sensorTransmitArray[5] = converter.b[0];
  sensorTransmitArray[6] = converter.b[1];

  converter.i = leftClick;
  sensorTransmitArray[7] = converter.b[0];

  converter.i = rightClick;
  sensorTransmitArray[8] = converter.b[0];


  radio.write(&sensorTransmitArray,sizeof(sensorTransmitArray));
}

void sticko_mode()                          // joystick mode 
{
  yValue = analogRead(Y_pin) - yZero;       // get x and y values, stops joystick from returning to original position 
  xValue = analogRead(X_pin) - xZero;
  switchValue = digitalRead(selButton);    // read base state

  x_sense = xValue / joystick_sense;
  y_sense = yValue / joystick_sense;

  converter.i = buttonModeFlag;              // converts modeflag, x & y and switch values to bytes and then stores it in byte array
  stickoTransmitArray[0] = converter.b[0];
  
  converter.i = x_sense;
  stickoTransmitArray[1] = converter.b[0];
  stickoTransmitArray[2] = converter.b[1];

  converter.i = y_sense;
  stickoTransmitArray[3] = converter.b[0];
  stickoTransmitArray[4] = converter.b[1];

  converter.i = switchValue;
  stickoTransmitArray[5] = converter.b[0];
  
  radio.write(&stickoTransmitArray,sizeof(stickoTransmitArray));    // send array through transciever
  
}

void loop(){
  buttonState = digitalRead(buttonPin);                 // get values from button
  
  if( (buttonState == HIGH) && (!buttonModeFlag)) {     // check if button =1 and flag is 0
    buttonModeFlag = 1;
    delay(200);
  }
  else if ( (buttonState == HIGH) && (buttonModeFlag)){ // check if button = 0 and flag is 1
    buttonModeFlag = 0;
    delay(200);
  }

  if( buttonModeFlag == 0){                 // switch to sensor if flag = 0
    sensor_mode();
  }
  else{
    sticko_mode();                          // switch to sticko if flag = 1
  }
}
