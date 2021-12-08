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

long tempoDelay = 60000 * 15;
unsigned long ultimaLeitura = 0;
unsigned long tempoDecorrido = 0;

float temperatura, umidade, peso;
String url;

void setup() {
  Serial.begin(9600);
  
  gprsSerial.listen();
  gprsSerial.begin(9600);  
  
  dht.begin();
  scale.begin(pinDT, pinSCK);
  scale.set_scale(-422.50);
  scale.tare();
 
  delay(10000);
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

String getSerialData() {
  return gprsSerial.available() != 0 ? gprsSerial.readString() : "";
}
/*
void resendData() {
  Serial.println("Erro! Falha ao enviar os dados! Será feita uma nova tentativa em alguns instantes!");

  gprsSerial.println("AT+HTTPTERM");
  delay(3000);
  getSerialData();
  
  gprsSerial.println("AT+SAPBR=0,1");
  delay(3000);
  getSerialData();
    
  delay(10000);
  sendData();
}
*/
void sendData() {
  
  temperatura = dht.readTemperature(); 
  umidade = dht.readHumidity();
  peso = scale.get_units(5);
  
  delay(1000);

  gprsSerial.println("AT");
  delay(3000);

  //if (getSerialData().indexOf("OK") == -1) resendData();

  gprsSerial.println("AT+SAPBR=0,1");
  delay(3000);

  //if (getSerialData().indexOf("OK") == -1) resendData();
  
  gprsSerial.println("AT+SAPBR=3,1,\"APN\", \"gprs.oi.com.br\"");
  delay(3000);

  //if (getSerialData().indexOf("OK") == -1) resendData();
  
  gprsSerial.println("AT+SAPBR=1,1");
  delay(3000);
  
  //if (getSerialData().indexOf("OK") == -1) resendData();

  gprsSerial.println("AT+HTTPINIT");
  delay(3000);

  //if (getSerialData().indexOf("OK") == -1) resendData();

  gprsSerial.println("AT+HTTPPARA=\"CID\",1");
  delay(3000);

  //if (getSerialData().indexOf("OK") == -1) resendData();

  url="185.28.23.22:8081/colmeias/update?codigo=" + String(codigo) + "&temperatura=" + String(temperatura) + "&umidade=" + String(umidade) + "&peso=" + String(peso) + "&telefone=" + String(telefone);
  Serial.println("Enviando dados para o servidor. URL: " + url);
  gprsSerial.println("AT+HTTPPARA=\"URL\",\"" + url + "\"");
  delay(3000);
  
  //if (getSerialData().indexOf("OK") == -1) resendData();

  gprsSerial.println("AT+HTTPACTION=0");
  delay(10000);
  //getSerialData();
  
  gprsSerial.println("AT+HTTPTERM");
  delay(3000);
  //getSerialData();
  
  gprsSerial.println("AT+SAPBR=0,1");
  delay(3000);
  //getSerialData();
}
