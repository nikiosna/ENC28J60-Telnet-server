//ENC28J60 Telnet-server (for powercontrol of a pc)
//By Nikiosna
//Based upon
//https://github.com/ntruchsess/arduino_uip
//https://www.arduino.cc/en/Tutorial/ChatServer
//https://gist.github.com/atomsfat/1813823
//https://github.com/bibi21000/arduino-fullip/

#include <UIPEthernet.h>

#define CMDLENGTH 12

static byte mac[6] =    {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
static byte ip[]  =     {192, 168, 1, 90 };
static byte mydns[] =   {192, 168, 1, 1 };
static byte gateway[] = {192, 168, 1, 1 };
static byte subnet[]  = {255, 255, 255, 0 };

EthernetServer server(23);
EthernetClient client;
boolean clientConnected = false;
//unsigned long timeOfConnection = 0;
//unsigned int timeout = 10000;

int pwrButton = 2;
int pwrSupply = 3;
int ledVoltage = A0;

float voltage = 0.0;
char command[CMDLENGTH] = "";

void setup()
{
  Serial.begin(9600);
  
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
  Ethernet.maintain();
  client = server.available();
  if (client) {
    if (!clientConnected) {
      client.flush();
      clientConnected = true;
      //timeOfConnection = millis();
      
      help();
      server.print(">");
    }
    /*else if(millis() > (timeOfConnection + timeout)) {
      server.println("You timed out!");
      disconnect();
      clientConnected = false;
      Serial.println("Client dropped!");
    }*/
    int i = 0;
    while (client.available() > 0 && i < CMDLENGTH) {
      command[i] = client.read();
      i++;
    }
    Serial.print(command);
    
    if(containsCR(command)) {
      //Add commands here
      
      //pwrbtn "number between 1 and 9" (0 in ASCII is 48 ... and 9 is 57)
      if (compare(command, "pwrbtn ", 7) && command[7]>48 && command[7]<58 ) {
        pwrbtn((command[7]-48)*1000);
      }
      
      else if (compare(command, "stat", 4)) {
        led_voltage();
      }
      
      else if (compare(command, "quit", 4) || compare(command, "exit", 4)) {
        client.stop();
        clientConnected = false;
      }
      
      else help();
      server.print(">");
    }
    memset(command, 0, CMDLENGTH); //delete the command
  }
}

//compares if the begin of two char arrays are equal
boolean compare(char a[], char b[], int length_of_b) {
  for(int i = 0; i < length_of_b; i++) {
    if(a[i] != b[i]) return false;
  }
  return true;
}

//tests if the chararray a contains a carriage return
boolean containsCR(char a[]) {
  for(int i = 0; i < CMDLENGTH; i++) {
    if(a[i] == (char)13) return true;
  }
  return false;
}

void pwrbtn(int x) {
   digitalWrite(pwrButton, LOW);
   delay(x);
   digitalWrite(pwrButton, HIGH);
   server.print("pwrbtn ");
   server.println(x);
}

void led_voltage() {
   voltage = (float)analogRead(ledVoltage);
   voltage = (voltage/1024)*5.0;
   server.print("LED-voltage is: ");
   server.print(voltage);
   server.println("V");
}

void help() {
  server.println("> Avaible commands:");
  server.println("pwrbtn [s]  | press the powerbutton [x] s");
  server.println("stat        | reads the voltage of the powerLED");
  server.println("exit / quit | close the telnet-session");
  delay(500);
}
