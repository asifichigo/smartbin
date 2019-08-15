#include <ESP8266WiFi.h> 
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <FirebaseArduino.h>


#define FIREBASE_HOST "lifeeasy-c8ce6.firebaseio.com"
#define FIREBASE_AUTH "jrH1NKjlN8w8e3FBDXfu5XDN1EFWdm94fXlGJHY7"

StaticJsonBuffer<200> jsonBuffer; //Set buffer for json object to be created
JsonObject& bin = jsonBuffer.createObject(); // create json object utilizing previously defined buffer

//time_now represents time since garbage last collected
//dumpedDuration is duration of uncollected garbage
unsigned long time_now = 0, dumpedDuration = 0;

//period is threshold for duration of uncollected garbage. 
//Garbage if not collected for given period might start producing foul smell, thus needs immediate treatment
int period = 5000;// set threshold period  Eg. 172800000ms for 2 days

int trigPin = 5;    // Trigger
int echoPin = 4;    // Echo

double perc,cm,diff,duration;


/* To read UltraSonic sensor reading */
double getUsReading()
{ 
  double curReading = 0.0;
  
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse.
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW); 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  duration = pulseIn(echoPin, HIGH); 
  // Convert the time into a distance in cm
  curReading = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343 
  // limit the range of US sensor reading to deal with unambigous result if occurs. Change as per the requirements.  
  if(curReading >=2.0 && curReading <= 400.0)  
  {
    //Return the current depth of the trash inside the bin
    return curReading;    
  }
  return 0.0;
}


//To calculate percentage of bin's capactiy filled.
double percentageFilled(double binHeight, double trashHeight)
{
  // use current reading to calculate percentage of bin filled. 
  diff = binHeight-trashHeight; // 'diff' tells overall height occupied by trash.
  return ((double)diff/binHeight)*100; // 'perc' represents percentage of the Garbage Bin filled.  
}


//To check if bin cleaned or not
bool isBinCleaned()
{ 
  if(perc >= 10)
  {
    bin["cleaned"] = 0;
    //Serial.println("Bin to be cleaned\n");
    return false;
  }
  else{
    bin["cleaned"] = 1;
    //Serial.println("Adequate capacity available\n");
    return true;
  }
}


//To throw alert to corporation/user accordingly
void throwAlert()
{
  dumpedDuration = (unsigned long)(millis() - time_now); //Duration of uncollected garbage
  if(!isBinCleaned() && (dumpedDuration > period || perc > 70.0)){
          //if garbage not collected for given period or the bin filled more than 70%  
          bin["alert"] = 1;//Alert to be thrown
          Serial.println("Bin need to be cleaned immediately\n");          
   }
  else if(isBinCleaned() && (dumpedDuration > period)){
          time_now = millis(); //refresh timer keeping record of days, the bin not been cleaned.
          bin["alert"] = 0; // Needn't throw alert        
  }
  else{
    // bin filled less than 70% and collection of garbage not due by 2 or more days.
    //Bin is not in need to be cleaned at present.
    bin["alert"] = 0; //Alert off
    Serial.println("Adequate capacity available\n");
  }
}


//To display bin parameters.
void showData()
{
 Serial.print("Height of bin:");
 Serial.print((double)bin["height"]);
 Serial.println("cm"); 
 Serial.print("Current reading/height:");
 Serial.print(cm);
 Serial.println("cm"); 
 Serial.print("Difference:");
 Serial.println((double)diff);
 Serial.print("Percentage filled:");
 Serial.print(perc);
 Serial.println("%");
}


void setup() {

  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //Set parameters of bin to default values  
  bin["height"] = 0.0;
  bin["perc"] = 0.0;
  bin["alert"] = 0;
  bin["cleaned"] = 1;
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  //sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep
  //in seconds
  wifiManager.setTimeout(180);
  
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if(!wifiManager.autoConnect("X")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }   
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); //Connect to firebase
}


void loop() {
  if (Firebase.failed()) // Check for errors 
  {
    Serial.print("Firebase connection failed\n Error:");    
    Serial.println(Firebase.error());    
    return;  
  }
  Firebase.set("/bin1/", bin);
  Serial.print("Firebase: Pushed data\n ");
  cm = getUsReading();
  
  //readHeight = Firebase.getFloat("/bin/height");
  // Set height for the first time. height = initial reading of Us sensor for the given empty bin.
  if(bin["height"]== 0.0)
  {
    bin["height"]= cm;  //Height of the bin
  }
  
  else if(cm > (double)bin["height"] && cm < (double)bin["height"]*1.1)
  {
    bin["height"]= cm; 
  }
  
  else
  { 
    perc = percentageFilled((double)bin["height"],cm);
    bin["perc"]= perc; 
  }  
  throwAlert(); //To produce alert messages as required
  showData(); // To print data of the bin.
  delay(1000);
}
