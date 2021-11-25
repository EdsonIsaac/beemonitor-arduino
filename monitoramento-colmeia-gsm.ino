#include <DHT.h>
#include <DHT_U.h>
#include <HX711.h>
#include <SoftwareSerial.h> 
#include <String.h>

#define DHTPIN 2
#define DHTTYPE DHT11

#define pinDT  4
#define pinSCK  3

DHT dht(DHTPIN, DHT11);
HX711 scale;
SoftwareSerial gprsSerial(6, 7);

const char codigo[] = "0001";
const char telefone[] = "74988287606";

long tempoDelay = 60000 * 1;
unsigned long ultimaLeitura = 0;
unsigned long tempoDecorrido = 0;

float temperatura, umidade, peso;
String url;

void setup() {
  gprsSerial.begin(9600);  
  Serial.begin(9600);
  
  dht.begin();
  scale.begin(pinDT, pinSCK);
  scale.set_scale(-422.50);
  scale.tare();
 
  delay(1000);
  sendData();
}
 
void loop() {
  
  tempoDecorrido = millis();

  if (tempoDecorrido < ultimaLeitura) {
    ultimaLeitura = 0;
  }

  if ((tempoDecorrido - ultimaLeitura) >= tempoDelay) {
    sendData();
    ultimaLeitura = tempoDecorrido;
  }
}

void sendData() {
  
  temperatura = dht.readTemperature(); 
  umidade = dht.readHumidity();
  peso = scale.get_units(5);
  
  delay(100);

  if (gprsSerial.available()) Serial.write(gprsSerial.read());
 
  gprsSerial.println("AT");
  delay(1000);
 
  gprsSerial.println("AT+CPIN?");
  delay(1000);
 
  gprsSerial.println("AT+CREG?");
  delay(1000);
 
  gprsSerial.println("AT+CGATT?");
  delay(1000);
 
  gprsSerial.println("AT+CIPSHUT");
  delay(1000);
 
  gprsSerial.println("AT+CIPSTATUS");
  delay(2000);
 
  gprsSerial.println("AT+CIPMUX=0");
  delay(2000);
 
  showSerialData();
 
  gprsSerial.println("AT+CSTT=\"gprs.oi.com.br\"");//start task and setting the APN,
  delay(1000);
 
  showSerialData();
 
  gprsSerial.println("AT+CIICR");//bring up wireless connection
  delay(3000);
 
  showSerialData();
 
  gprsSerial.println("AT+CIFSR");//get local IP adress
  delay(2000);
 
  showSerialData();
 
  gprsSerial.println("AT+CIPSPRT=0");
  delay(3000);
 
  showSerialData();
  
  gprsSerial.println("AT+CIPSTART=\"TCP\",\"185.28.23.22\",\"8081\"");//start up the connection
  delay(6000);
 
  showSerialData();
 
  gprsSerial.println("AT+CIPSEND");//begin send data to remote server
  delay(4000);
  showSerialData();
  
  url="GET 185.28.23.22:8081/colmeias/update?codigo=" + String(codigo) + "&temperatura=" + String(temperatura) + "&umidade=" + String(umidade) + "&peso=" + String(peso) + "&telefone=" + String(telefone) + " HTTP/1.1\r\n\r\n";
  Serial.println(url);
  gprsSerial.println(url);//begin send data to remote server
  
  delay(4000);
  showSerialData();
 
  gprsSerial.println((char)26);//sending
  delay(5000);//waitting for reply, important! the time is base on the condition of internet 
  gprsSerial.println();
  
  gprsSerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  showSerialData();
}

void showSerialData() {
  
  while(gprsSerial.available() != 0) {
    Serial.write(gprsSerial.read());
  }
  
  delay(5000);
}
