# smartbin


Problem Statement: 
  Unduly collection of garbage inside the bin or overflowing bins produces foul smell thereby casuing inconvenience to the surrounding. The current status of the bin that is the time period for how long the garbage remains unattended since previous cleanup and the capacity of the bin filled must be brought to the notice. Alert can be produced accordingly so as to ensure duly cleanup of the bins to keep surrounding clean and healthy.
  
Solution ? Uhn... Wish we could have a smart bin that can sort out the issue on its own! Yes why not? Lets build one ;)

Features
  - Foremost the solution is applicable to varieties of the bin
  - Measure height of the bin
  - Calculate capacity of the bin filled ie amount of trash dumped
  - Calculate time since last cleaned
  - Throw alert if capacity reaches the threshold lets say 70% or exceeds given period of time say 2 days
  - Send alert to user using mobile application representing bins in red color which need to be cleaned immediately and in blue color the ones which has capacity left yet uncleaned
  - Authorized users alone can access the data
  - Statistics available through web service
  
 Requirements
  - NodeMCU
  - Jumper Wires
  - UltraSonic Sensor
  - Bin
  - Firebase account
  
 Steps
  1. Create a firebase project @ https://console.firebase.google.com
  2. Go to Database -> Realtime Database -> Rules tab
      -Set read and write as true or anything else as per the requirements.
      -Click Publish.
  3. In Arduino IDE, 
     - Import FirebaseArduino and ArduinoJson 5.x library files. Update the fingerprint in 
     /Arduino/libraries/firebase-arduino-master/src/FirebaseHttpClient.h with latest one 
     - Import WifiManager library files
  4. Hardware connections
     - Connect Vcc and GND of Ultrasonic sensor to any 3v3 & GND pin of nodemcu repectively.
     - Connect Trig and Echo of Ultrasonic sensor to D1 and D2 of Nodemcu respectively.
  5. Substitute your project url in FIREBASE_HOST and database secret key in FIREBASE_AUTH in the code.
  6. Upload the code to NodeMCU.
  7. Connect to WiFi namely "X" using mobile or pc and upload WiFi credentials of the WiFi the NodeMCU is supposed to use.
  8. Put the setup in an Empty bin as it has to determine height. Reset NodeMCU.
  9. Observe the result in Serial monitor
  
