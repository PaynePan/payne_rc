/*   Serial Receiver     */
/*   Payne.Pan 2017. 12  */

#include <Servo.h> 
#define MAX_SERVO  6
Servo myservo[MAX_SERVO+1];  // create servo object to control a servo

boolean stringComplete = false;  // whether the string is complete
static int dataBufferIndex = 0;
boolean stringOverflow = false;
char charOverflow = 0;

#define BUF_LEN 128
char serialData[BUF_LEN] = "";
char serialBuffer[BUF_LEN] = "";

uint8_t ch3_1500 = 0;

uint8_t ch3_failsafe_1500_jumper(void) {
  pinMode(8, OUTPUT);  
  pinMode(9, INPUT); 
  digitalWrite(9, HIGH); // pull up
  digitalWrite(8, HIGH); 
  delayMicroseconds(1);
  if ( digitalRead(9) == HIGH)
  {
    digitalWrite(8, LOW); 
    delayMicroseconds(1);
    if (digitalRead(9) == LOW) 
    {
      pinMode(9, OUTPUT);
      return  1; 
    }
  }
  pinMode(9, OUTPUT);
  return  0; 
}

void setup(void) {
  ch3_1500 = ch3_failsafe_1500_jumper();
  for ( int i=1; i<=MAX_SERVO; i++) {
    myservo[i].attach(i+1);  // attaches the servo on pin (2-*) to the servo object 
  } 
  Serial.begin(57600);
  delay(50);
}

void setServo(int no, int pos)
{
  if ( pos < 500)
  	return;

  if ( no >= 1 && no <= MAX_SERVO)
    myservo[no].writeMicroseconds(pos);
}

int lastLoopUsefulTime;
unsigned long lastSignalTime = 0;

void loop(void) {
  if ( millis() - lastSignalTime > 300) { // failsafe 
    lastSignalTime = millis();
    //reset servo
    for ( int i=1; i<=MAX_SERVO; i++) {
          myservo[i].writeMicroseconds(1500);
    }
    if ( !ch3_1500 )
          myservo[3].writeMicroseconds(960);
    
  }
  serial_receive();

} // end loop()

void serialEvent() {
  while (Serial.available() > 0 ) {
    char incomingByte = Serial.read();

    /*
    if (stringOverflow) {
      serialBuffer[dataBufferIndex++] = charOverflow;  // Place saved overflow byte into buffer
      serialBuffer[dataBufferIndex++] = incomingByte;  // saved next byte into next buffer
      stringOverflow = false;                          // turn overflow flag off
    } 
    else if (dataBufferIndex > BUF_LEN) {
      stringComplete = true;        // Send this buffer out to radio
      stringOverflow = true;        // trigger the overflow flag
      charOverflow = incomingByte;  // Saved the overflow byte for next loop
      dataBufferIndex = 0;          // reset the bufferindex
      break; 
    } 
    else */ if(incomingByte=='\n' || dataBufferIndex == BUF_LEN-1 ){
      serialBuffer[dataBufferIndex] = 0; 
      stringComplete = true;
      break;
    } 
    else {
      serialBuffer[dataBufferIndex++] = incomingByte;
      serialBuffer[dataBufferIndex] = 0; 
    }          
  } // end while()
} // end serialEvent()

void serial_receive(void){
  if (stringComplete) { 
    strcpy(serialData,serialBuffer); 
    int pos = 0;
    while ( serialData[pos] == '#') {
      String servo_no="";
      String servo_val="";
      pos++;
      while( isDigit(serialData[pos])) {
        servo_no += serialData[pos];
        pos++;
      }
      if ( serialData[pos] == 'P') {
        pos++;
        while( isDigit(serialData[pos])) {
          servo_val += serialData[pos];
          pos++;
        }
      }
      setServo(servo_no.toInt(), servo_val.toInt());
      lastSignalTime = millis();
    }
    stringComplete = false;
    dataBufferIndex = 0;
  } // endif
} // end serial_receive()
