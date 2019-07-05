#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Mouse.h>


RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

byte recievedArray[9];

int buttonModeFlag= 0;
int switchValue;
int yValue,xValue;
int scrollNumber;
int rightClick, leftClick;
int mouseClickFlag = 0;
int leftClickFlag = 0;
int rightClickFlag = 0;

void setup() {
  Serial.begin(115200);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Mouse.begin();
}

union
{
  byte b[1];
  int i;
}singleConverter;
union
{
  byte b[2];
  int i;
}converter;

void sticko_mode() {
    
      converter.b[0] = recievedArray[1];
      converter.b[1] = recievedArray[2];
      xValue = converter.i;

      converter.b[0] = recievedArray[3];
      converter.b[1] = recievedArray[4];
      yValue = converter.i;

      singleConverter.b[0] = recievedArray[5];
      switchValue = singleConverter.i;

      Serial.print("Mode: Sticko");
      Serial.print('\n');
      Serial.print("X: ");
      Serial.println(xValue);
      Serial.print("Y: ");
      Serial.println(yValue);
      Serial.print("SW: ");
      Serial.println(switchValue);
      Serial.print('\n');

      if(xValue !=0)
        Mouse.move(xValue,0, 0);
      if(yValue !=0)
        Mouse.move(0,yValue,0);

      if((switchValue == 0) && (!mouseClickFlag)) {
        mouseClickFlag = 1;
        Mouse.press(MOUSE_LEFT);
      }
      else if((switchValue) && (mouseClickFlag)) {
        mouseClickFlag = 0;
        Mouse.release(MOUSE_LEFT);
      }
     
}

void sensor_mode() {
    
      converter.b[0] = recievedArray[1];
      converter.b[1] = recievedArray[2];
      xValue = converter.i;

      converter.b[0] = recievedArray[3];
      converter.b[1] = recievedArray[4];
      yValue = converter.i;

      converter.b[0] = recievedArray[5];
      converter.b[1] = recievedArray[6];
      scrollNumber = converter.i;
      
      singleConverter.b[0] = recievedArray[7];
      leftClick = singleConverter.i;

      singleConverter.b[0] = recievedArray[8];
      rightClick = singleConverter.i;

      Serial.print("Mode: Sensor");
      Serial.print('\n');
      Serial.print("X: ");
      Serial.println(xValue);
      Serial.print("Y: ");
      Serial.println(yValue);
      Serial.print("Scroll: ");
      Serial.println(scrollNumber);
      Serial.print("L: ");
      Serial.println(leftClick);
      Serial.print("R: ");
      Serial.println(rightClick);
      Serial.print('\n');

      scrollNumber = scrollNumber * -1;
      if(xValue !=0)
        Mouse.move(xValue,0, 0);
      if(yValue !=0)
        Mouse.move(0,yValue,0);
      if(scrollNumber !=0)
        Mouse.move(0,0,scrollNumber);

      if((leftClick == 0) && (!leftClickFlag)) {
        leftClickFlag = 1;
        Mouse.press(MOUSE_LEFT);
      }
      else if((leftClick) && (leftClickFlag)) {
        leftClickFlag = 0;
        Mouse.release(MOUSE_LEFT);
      }
      
      if((rightClick == 0) && (!rightClickFlag)) {
        rightClickFlag = 1;
        Mouse.press(MOUSE_RIGHT);
      }
      else if((rightClick) && (rightClickFlag)) {
        rightClickFlag = 0;
        Mouse.release(MOUSE_RIGHT);
      }
}
void loop() {
  if (radio.available()) {
    while (radio.available()) {
      radio.read(&recievedArray, sizeof(recievedArray));
      
      singleConverter.b[0] = recievedArray[0];
      buttonModeFlag = singleConverter.i;

        if( buttonModeFlag == 1)
          sticko_mode();
        else
          sensor_mode();
    }
  }
}
