// tomn CLS (Cubicle Lighting System)

#include <avr/wdt.h>

// Stuff for the motors
#include <AFMotor.h>
#include <Servo.h> 

// Able to control 4 lights!
// AF_DCMotor motor1(1);      Cannot use motor 1 as Digital Pin 11 is used with SPI and Ethernet
AF_DCMotor motor2(2);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);

// There are 4 motors.  0 = Descired, 1 = Current, 2 = Reported
int motorSpeeds[4][3];

// Stuff for the MQTT client
#include <SPI.h>
#include <Wire.h>
#include <Ethernet.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xE4, 0xBA, 0xF0, 0xFE, 0xED };
byte server[] = { 206, 246, 158, 179 };

// Only 128 character messages are supported.
// Longer messages are UNDEFINED!  (ie.. You will crash the Arduino.)
char topic_buff[25];
char message_buff[128];




void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.println(topic);
  
  // create character buffer with ending null terminator (string)
  int i = 0;
  for(i=0; i<128; i++) {
   message_buff[i] = '\0'; 
  }

  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  // Chop the string off at 22 - that's the widt of the display
  message_buff[length] = '\0';
  
  String msgString = String(message_buff);
  Serial.println("Payload: " + msgString);

  if ( strncmp( topic, "/arduino/lights/status", 22 ) == 0 ) {
    return;
  }

  // If we get a message to just lights, set them all to this value
  if ( strncmp( topic, "/arduino/lights/all", 19) == 0 )
  {
    
    Serial.print("All Lights - ");
    Serial.println( topic );

    for ( int c = 0; c < 4; c++ ) {
      motorSpeeds[c][0] = msgString.toInt();
    }
    return;
  }

  // If we get a "directory" then set the individual light number to this value
  if ( strncmp( topic, "/arduino/lights/", 16) == 0 )
  {
    Serial.println(topic);

    int light = topic[16]-48;
    motorSpeeds[light-1][0] = msgString.toInt();
  }


}

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);


void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("tomn CLS!");

  // Turn off the lights
  // motor1.setSpeed(0);
  // motor1.run(FORWARD);
  motor2.setSpeed(0);
  motor2.run(FORWARD);
  motor3.setSpeed(0);
  motor3.run(FORWARD);
  motor4.setSpeed(0);
  motor4.run(FORWARD);

  // We're alive, enable the watch dog
  wdt_enable(WDTO_8S);

  Serial.println("Starting Network"); 

  Ethernet.begin(mac);
  
  Serial.print("IP: ");
  Serial.println(Ethernet.localIP());

  // Tell the watchdog we're still alive
  wdt_reset();
  
  if (client.connect("arduinoClient", "arduino", "moo837388d")) {
    client.publish("hello","hello world from tomn CLS");
    client.subscribe("/arduino/lights/#");
  }

  // Tell the watchdog we're still alive
  wdt_reset();

}

void loop() {

  // Tell the watchdog we're still alive
  wdt_reset();
  
  client.loop();
  
  setMotors();
  
  delay(7);

  if ( !client.connected() ) {
    // Sshhhhhhh.
    Serial.println("Going quietly into the night.");
    while(1){}    
  }
}


void setMotors() {

  for ( int c = 0; c < 4; c++ ) {
    if ( motorSpeeds[c][0] > 255 ) { motorSpeeds[c][0] = 255; }
    if ( motorSpeeds[c][0] <   0 ) { motorSpeeds[c][0] = 0; }
    // Check to see if we desire a speed change
    if ( motorSpeeds[c][0] > motorSpeeds[c][1] ) {
      motorSpeeds[c][1]++;
      motorSpeeds[c][2] = 1;
    }
    if ( motorSpeeds[c][0] < motorSpeeds[c][1] ) {
      motorSpeeds[c][1]--;
      motorSpeeds[c][2] = 1;
    }
    // If we've reached equalibrimu then tell the world.. ONCE.
    if ( motorSpeeds[c][2] && motorSpeeds[c][0] == motorSpeeds[c][1] ) {
      motorSpeeds[c][2] = 0;
      
      String topicString = String("/arduino/lights/status/")+(c+1);
      topicString.toCharArray(topic_buff, 25);
      String msgString = String("")+(motorSpeeds[c][1]);
      msgString.toCharArray(message_buff, 128);

      client.publish(topic_buff,message_buff);

    }
  }
  
  // motor1.setSpeed(motorSpeeds[0][1]);
  //if ( motorSpeeds[0][1] == 0 ) {
  //  motor1.run(RELEASE);
  //} else {
  //  motor1.run(FORWARD);
  //}
  // motor1.run(FORWARD);
  motor2.setSpeed(motorSpeeds[1][1]);
  if ( motorSpeeds[1][1] == 0 ) {
    motor2.run(RELEASE);
  } else {
    motor2.run(FORWARD);
  }
  motor3.setSpeed(motorSpeeds[2][1]);
  if ( motorSpeeds[2][1] == 0 ) {
    motor3.run(RELEASE);
  } else {
    motor3.run(FORWARD);
  }
  motor4.setSpeed(motorSpeeds[3][1]);
  if ( motorSpeeds[3][1] == 0 ) {
    motor4.run(RELEASE);
  } else {
    motor4.run(FORWARD);
  }

  
}

