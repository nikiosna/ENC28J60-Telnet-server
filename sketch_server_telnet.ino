//ENC28J60 Telnet-server (for powercontroll of a pc)
//By Nikiosna
//Based upon
//https://github.com/ntruchsess/arduino_uip
//https://www.arduino.cc/en/Tutorial/ChatServer
//https://gist.github.com/atomsfat/1813823

#include <UIPEthernet.h>

#define cmdlength 12

static byte mac[] =     { 0xED, 0xDE, 0x12, 0x31, 0x41, 0x41 };
static byte ip[]  =     {192, 168, 1, 90 };
static byte mydns[] =   {192, 168, 1, 1 };
static byte gateway[] = {192, 168, 1, 1 };
static byte subnet[]  = {255, 255, 255, 0 };

EthernetServer server(23);;
EthernetClient client;
boolean clientConnected = false;
//unsigned long timeOfConnection = 0;
//unsigned int timeout = 10000;

//Relais sind invertiert: HIGH = OFF, LOW = ON
int pwrButton = 2;
int pwrSupply = 3;
int ledVoltage = A0;

float voltage = 0.0;
char command[cmdlength] = "";

void setup()
{
  Serial.begin(9600);
  //while(!Serial) {;}
  
  pinMode(pwrButton, OUTPUT);
  pinMode(pwrSupply, OUTPUT);
  digitalWrite(pwrButton, HIGH); 
  digitalWrite(pwrSupply, HIGH); 
  
  Ethernet.begin(mac, ip, mydns, gateway, subnet);
  server.begin();
  
  Serial.print("Server address: ");
  Serial.println(Ethernet.localIP());
}

void loop()
{
  client = server.available();
  if (client) {
    if (!clientConnected) {
      client.flush(); // clear out the input buffer:
      Serial.println("We have a new client");
      client.println("Hello, client!");
      clientConnected = true;
//    timeOfConnection = millis();
    }
    /*else if(millis() > (timeOfConnection + timeout)) {
      server.println("You timed out!");
      client.stop();
      clientConnected = false;
      Serial.println("Client dropped!");
    }*/
    int i = 0;
    while (client.available() > 0 && i < cmdlength) {
      command[i] = client.read();
      i++;
    }
    Serial.print(command);
    if(command[0]=='p' && command[1]=='w' && command[2]=='r' && command[3]=='b' && command[4]=='t' && command[5]=='n' && command[6]==' '
        && command[7]>48 && command[7]<58 ) {
      pwrbtn((command[7]-48)*1000);
    }
    else if (command[0]=='s' && command[1]=='t' && command[2]=='a' && command[3]=='t') {
      led_voltage();
    } else unknown();
    server.print(">");
  }
}

void pwrbtn(int x) {
   digitalWrite(pwrButton, LOW);
   delay(x);
   digitalWrite(pwrButton, HIGH);
   server.print("pwrbtn ");
   server.println(x);
}

void led_voltage() {
   voltage = (double)analogRead(ledVoltage);
   server.print("LED-voltage is: ");
   server.print(voltage);
   server.println("V");
}

void unknown() {
  server.println("> Avaible commands:");
  server.println("pwrbtn [s]  | press the powerbutton [x] s");
  server.println("stat        | reads the voltage of the powerLED");
}
