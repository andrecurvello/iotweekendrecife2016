#include <DHT.h>
#include <ESP8266WiFi.h>

int luz = 0;
int temp;
int umid;
char msg[50];

const char* ssid ="CrazyTechLabs";
const char* senha = "#iotweekend";
const char* mqtt_server = "iot.eclipse.org";

WiFiClient espClient;

DHT dht(D4,DHT11);

void setup() {
  pinMode(D0, OUTPUT); //Seta led como saida
  Serial.begin(115200); //Define comunicação com 115200bps
  dht.begin(); //Inicializa o sensor DHT11

  //Setup da conexao WiFi do ESP8266
  Serial.print("Conectando na rede WiFi...");
  WiFi.begin(ssid,senha); //Inicia o processo de conexao via WiFi

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(); //Dá um "enter" - quebra de linha no serial
  Serial.println("ESP8266 Conectado no CrazyTechLabs!");
  Serial.println(WiFi.localIP()); //Imprime o endereço IP do módulo!
  
  
  
}

void loop() {
  luz = analogRead(A0);
  temp = (int) dht.readTemperature();
  umid = (int) dht.readHumidity();

  sprintf(msg, "Luz %d - Temp: %d - Umid: %d", luz, temp, umid);
  
  Serial.println(msg);
  
  if(luz < 500){
    Serial.println("Luz acesa");
    digitalWrite(D0, HIGH); //se luminosidade menor que 500 - acende led
  } else {
    //Caso contrario, luminosidade maior que 500, apaga led
    Serial.println("Luz apagada");
    digitalWrite(D0, LOW);
  }
  delay(2000); //atraso de 2s para interface com sensor de temp/umid
}
