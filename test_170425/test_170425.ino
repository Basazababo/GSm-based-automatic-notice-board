#include "Adafruit_FONA.h"
#include <LiquidCrystal.h>  // LCD
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);   // LCD uses these pins

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 13
#define GSM_detected 10
#define GSM_not_detected 11

char replybuffer[255]; // this is a large buffer for replies
#include <SoftwareSerial.h>
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
boolean debug = 1; 
String line1 = "ANNOUNCEMENT: ";
String line2;
int MSG_TO_DISPLAY = 10;

int LCD_CHAR = 16;
int LCD_ROWS = 4;
int stringStart, stringStop = 0;
int scrollCursor = LCD_CHAR;
boolean gsm_status =1;

void setup()
{
  Serial.begin(115200);
  pinMode(GSM_detected,OUTPUT);
  pinMode(GSM_not_detected,OUTPUT);
    //-------------HOME PAGE-----------------------------------------
  lcd.begin(LCD_CHAR, LCD_ROWS);   //LCD with 16 characters and 4 lines
  lcd.setCursor(1, 0);
   lcd.print("DEUS and ALEX "); 
   lcd.setCursor(4, 1); //set lcd curser to 2nd line
  lcd.print("AUTOMATIC");
   lcd.setCursor(3, 2);     //set lcd curser to 3rd line
  lcd.print(line1);
   lcd.setCursor(6, 3);     //set lcd curser to 3rd line
  lcd.print("BOARD");
  delay(3000); // wait for 5000ms =5 seconds
  
  //--------------------------------------------

  fonaSerial->begin(4800);
  if ( fona.begin(*fonaSerial)) {
    Serial.println("Software serial established");
  }
 
  char imei[15] = {0}; // MUST use a 16 character buffer for IMEI!
  uint8_t imeiLen = fona.getIMEI(imei);
  if (imeiLen > 0) {
    digitalWrite(GSM_detected,HIGH);
    digitalWrite(GSM_not_detected,LOW);
    
    Serial.println("GSM Module is detected");
    lcd.clear();
    lcd.setCursor(4, 0); //set lcd curser to 2nd line
    lcd.print("WELCOME"); 
    lcd.setCursor(3, 1);     //set lcd curser to 3rd line
    lcd.print("GSM MODEM");
    lcd.setCursor(3, 2);     //set lcd curser to 3rd line
    lcd.print("CONNECTED");
    gsm_status =1;
    delay(3000); // wait for 5000ms =5 seconds
    
  }
  else
  {
    digitalWrite(GSM_detected,LOW);
    digitalWrite(GSM_not_detected,HIGH);
    Serial.println("GSM modem not detected, Check your SIM car/Wiring or Antenna");
    lcd.clear();
    lcd.setCursor(3, 0); //set lcd curser to 2nd line
    lcd.print("WELCOME"); 
    lcd.setCursor(1, 1);     //set lcd curser to 3rd line
    lcd.print("GSM MODEM");
    lcd.setCursor(2, 2);     //set lcd curser to 3rd line
    lcd.print("NOT CONNECTED");
    gsm_status = 0;
    delay(3000); // wait for 5000ms =5 seconds
  }
}


void loop() 
{
  if (gsm_status ==1)
  {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print(line1);//lcd.print(smsn);
    display_sms();
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("GSM Disconnected");
  }
  delay(500);
}

//////////////////////////////////////////end of void loop///////////////////////////////////////////////////////

void display_sms()
{
  flushSerial();
     uint8_t smsn;
     uint8_t smsnum =fona.getNumSMS() ;
     uint16_t smslen;
     
     for (int8_t smsn=1; smsn<=smsnum; smsn++) 
     {
      if (debug==1)
      { Serial.print("SMS Number #:"); Serial.println(smsn); }

      // Retrieve SMS sender address/phone number.
      if (! fona.getSMSSender(smsn, replybuffer, 250)) 
      {
         if (debug==1){ Serial.println("Failed to fetch sender phone number!");}
      }
       
       if (debug==1){ Serial.print("FROM ############: "); Serial.println(replybuffer);}

      // Retrieve SMS value.
      if ( fona.readSMS(smsn, replybuffer, 250, &smslen)) 
      { 
      lcd.setCursor(0,3);
      lcd.print(smsn);lcd.print("/");lcd.print(smsnum);//lcd.print(smsn);
      
      if (debug==1){ Serial.print("++SMS content of sms number: "); Serial.print(smsn); Serial.print(" is: ");Serial.println(replybuffer); } 
            if (debug==1)
      { Serial.print("++Size of the sms is: ");Serial.print(smsn);Serial.print(" is: ");Serial.print(" ("); Serial.print(smslen); Serial.println(F(") bytes *****")); }
      
      char buffer[16];
      fona.getTime(buffer, 16);  // make sure replybuffer is at least 23 bytes!, i reduce this to fit on the screen
      if (debug==1) 
      { 
        Serial.print(F("Time = ")); Serial.println(buffer); }
      
    
      
      for (int i=0; i<=strlen(replybuffer); i++)
      {
        line2=replybuffer;
        lcd.setCursor(scrollCursor, 2);
        lcd.print(line2.substring(stringStart,stringStop));
       // delay(200);
        
        if(stringStart == 0 && scrollCursor > 0 )
        {
          scrollCursor--;
          stringStop++;   
        } 
        
        else if (stringStart == stringStop)
        {
          stringStart = stringStop = 0;
          scrollCursor = LCD_CHAR;
          lcd.clear();
         // smsn++;
        } 
        
        else if (stringStop == line2.length() && scrollCursor == 0)
        {   
          stringStart++;
        } 
        else 
        {
          stringStart++;
          stringStop++;  
          }
        delay(300);
      }
      }
      
      else
      {
        if (debug==1) {Serial.println("Failed to catch sms content");}
      } 

  flushSerial();   // flush input

  Serial.println("===========================new message=================");
  while (fona.available()) 
  {
    Serial.write(fona.read());  
  }
 
}

}

//////////////////////////////////////////////////////end of display msg function//////////////////////////////////

void flushSerial() 
{
    while (Serial.available()) 
    Serial.read();
   // lcd.write(Serial.read());
}

char readBlocking() 
{
  while (!Serial.available());
  return Serial.read();
}

uint16_t readnumber() 
{
  uint16_t x = 0;
  char c;
  while (! isdigit(c = readBlocking())) {
    //Serial.print(c);
  }
  Serial.print(c);
  x = c - '0';
  while (isdigit(c = readBlocking())) {
    Serial.print(c);
    x *= 10;
    x += c - '0';
  }
  return x;
}
  
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout) {
  uint16_t buffidx = 0;
  boolean timeoutvalid = true;
  if (timeout == 0) timeoutvalid = false;
  
  while (true) {
    if (buffidx > maxbuff) {
      //Serial.println(F("SPACE"));
      break;
    }

    while(Serial.available()) {
      char c =  Serial.read();

      //Serial.print(c, HEX); Serial.print("#"); Serial.println(c);

      if (c == '\r') continue;
      if (c == 0xA) {
        if (buffidx == 0)   // the first 0x0A is ignored
          continue;
        
        timeout = 0;         // the second 0x0A is the end of the line
        timeoutvalid = true;
        break;
      }
      buff[buffidx] = c;
      buffidx++;
    }
    
    if (timeoutvalid && timeout == 0) {
      //Serial.println(F("TIMEOUT"));
      break;
    }
    delay(1);
  }
  buff[buffidx] = 0;  // null term
  return buffidx;
}
