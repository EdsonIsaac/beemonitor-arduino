#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <HX711.h>

#define DHTPIN 2
#define DHTTYPE DHT11

#define pinDT  4
#define pinSCK  3

DHT dht(DHTPIN, DHT11);
HX711 scale;
HTTPClient http;
WiFiClient client;

const char* ssid = "yourNetworkName";
const char* password = "yourNetworkPassword";

const char codigo[] = "0001";

long tempoDelay = 60000 * 1;
unsigned long ultimaLeitura = 0;
unsigned long tempoDecorrido = 0;

float temperatura, umidade, peso;
String url;

void setup() {
  
  Serial.begin(115200);
  setupWifi();
  setupSensors();
  
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

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    
    http.begin(client, "https://beemonitor-backend.herokuapp.com/colmeias/update?codigo=" + String(codigo) + "&temperatura=" + String(temperatura) + "&umidade=" + String(umidade) + "&peso=" + String(peso));
    int httpCode = http.GET(); //Send http request and return request status code

    if (httpCode > 0) Serial.println(http.getString()); //Prints the request response
    
    http.end(); //
  } else {
      Serial.println("Erro! Conexão com a rede Wi-Fi não estabelecida!");
      setupWifi();
      sendData();
  }
}

void setupSensors() {
  dht.begin();
  scale.begin(pinDT, pinSCK);
  scale.set_scale(-422.50);
  scale.tare();
}

void setupWifi () {
  WiFi.begin(ssid, password);
  
  Serial.println("Conectando a rede Wi-Fi: " + (String)ssid);
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Conexão realizada com sucesso! Endereço IP: ");
  Serial.println(WiFi.localIP());
}
