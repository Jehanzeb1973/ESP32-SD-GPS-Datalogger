

// Libraries for SD card
#include <FS.h>
#include <SPI.h>
#include <SD.h>

//Library for GPS
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#define RXD1 04
#define TXD1 27
#define LED_Pin 02

  // Define CS pin for the SD card module
#define SD_CS 22

String GPSData="";//Raw GPS data
String dataMessage="";
String dataMessage1="";//GPS  data
String FileName="";
bool FileCreated=false;
bool FileNameCreated=false;
bool GPSValid=false;

 HardwareSerial MySerial1(1);


 TinyGPSPlus gps;

void setup() {

   
  // Note the format for setting a serial port is as follows: Serial1.begin(baud-rate, protocol, RX pin, TX pin);
  Serial.begin(115200);
  
    MySerial1.begin(4800, SERIAL_8N1, RXD1, TXD1);//Serial 1 for GPS 1
  
  
  // Initialize SD card
  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
    
  }

  // If the data.txt file doesn't exist
  // Create a file on the SD card 
  File file = SD.open("/RawData.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/Rawdata.txt", "Reading Data \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();
  pinMode(LED_Pin,OUTPUT);
  digitalWrite(LED_Pin,HIGH);
delay(2000);
digitalWrite(LED_Pin,LOW);
}

void loop() { //Choose Serial1 or Serial2 as required

 
while (MySerial1.available()>0)
 {
 GPSData+=char(MySerial1.peek());
 gps.encode(MySerial1.read());
   }
   
  // gps.encode(GPSData[0]);
   filldataMessage1();

  dataMessage=dataMessage1+'\n';
  Serial.println(GPSData);
   Serial.println(dataMessage);
  if(gps.location.isValid()&&gps.date.isValid()&&gps.time.isValid())
  {
    GPSValid=true;
if(!FileCreated)  CreateFile();
    digitalWrite(LED_Pin,HIGH);
  }
  else 
  {
    GPSValid=false;
    digitalWrite(LED_Pin,LOW);
  }
    Serial.println(dataMessage);
  if(GPSValid)  logSDCard();
  GPSData="";
  dataMessage="";
  dataMessage1="";

 
delay(500);
}

void filldataMessage1()
{
   
  dataMessage1+=" Location: ";
  if (gps.location.isValid())
  {    

    dataMessage1+=gps.location.lat();
       dataMessage1+=",";
         dataMessage1+=gps.location.lng();
  }
  else
  {

     dataMessage1+="INVALID ";
  }
  dataMessage1+=" Altitude: ";
  if (gps.altitude.isUpdated())
  {
   // Serial.print(gps.altitude.feet(), 6);
    dataMessage1+=gps.altitude.feet();
  }
  else
  {
    dataMessage1+=" INVALID ";
  }
  dataMessage1+=" Speed: ";
 if (gps.speed.isUpdated())
  {
  //  Serial.print(gps.speed.kmph(), 6);
    dataMessage1+=gps.speed.kmph();
  }
  else
  {
     dataMessage1+=" INVALID ";
  }
  dataMessage1+="  Date/Time: ";
  if (gps.date.isValid())
  {
   
     dataMessage1+=gps.date.month();
    dataMessage1+="/";
      dataMessage1+=gps.date.day();
    dataMessage1+="/";
    dataMessage1+=gps.date.year();
    
  }
  else
  {
     dataMessage1+=" INVALID ";
  }


  dataMessage1+=" ";
  if (gps.time.isValid())
  {
       
   // if (gps.time.hour() < 10) Serial.print(F("0"));
   // Serial.print(gps.time.hour());
    dataMessage1+=gps.time.hour();
   // Serial.print(F(":"));
    dataMessage1+=":";
   // if (gps.time.minute() < 10) Serial.print(F("0"));
    //Serial.print(gps.time.minute());
    dataMessage1+=gps.time.minute();
    //Serial.print(F(":"));
    dataMessage1+=":";
  //  if (gps.time.second() < 10) Serial.print(F("0"));
  //  Serial.print(gps.time.second());
    dataMessage1+=gps.time.second();
    //Serial.print(F("."));
    dataMessage1+=".";
   // if (gps.time.centisecond() < 10) Serial.print(F("0"));
   // Serial.print(gps.time.centisecond());
    dataMessage1+=gps.time.centisecond();
   
  }
  else
  {
      
    dataMessage1+=" INVALID ";
  }

}



// Write the sensor readings on the SD card
void logSDCard() { 
 appendFile(SD,"/RawData.txt",GPSData.c_str());
 appendFile(SD, &FileName[0], dataMessage.c_str());
   
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
//  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
  //  Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void CreateFile(void)
{
  FileName="";
    
      if (gps.date.day()<10) FileName=FileName+"0"+gps.date.day();
      else FileName=FileName+gps.date.day();
      if (gps.date.month()<10) FileName=FileName+"0"+gps.date.month();
      else FileName=FileName +gps.date.month();

      if(gps.time.hour()<10)  FileName=FileName+"_"+"0"+gps.time.hour();
      else FileName=FileName+"_"+gps.time.hour();
    if(gps.time.minute()<10)  FileName=FileName+"0"+gps.time.minute();
    else FileName+=gps.time.minute();
           FileName="/"+FileName+".txt";
            FileNameCreated=true;
    File file = SD.open(FileName);
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    writeFile(SD,&FileName[0], "Reading Data \r\n");
    FileCreated=true;
  }
    
  
}
