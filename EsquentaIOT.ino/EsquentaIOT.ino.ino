#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

int luz = 0;
int temp;
int umid;
char msg[50];

const char* ssid ="CrazyTechLabs";
const char* senha = "#iotweekend";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient; //Objeto pra manipulação de WiFi
PubSubClient mqtt(espClient); //Objeto para manipulação de mqtt com WiFi

DHT dht(D4,DHT11);

void setup() {
  pinMode(D0, OUTPUT); //Seta led como saida
  pinMode(D1, OUTPUT);
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
  Serial.println(WiFi.localIP()); //Imprime o endereço IP do módulo

  mqtt.setServer(mqtt_server, 1883); //Defs. do servidor - end e porta
  mqtt.setCallback(callBackMqtt);
}


void callBackMqtt(char* topic, byte* payload, unsigned int size){
  Serial.print("Mensagem Sub: [");
  Serial.print(topic);
  Serial.println("]");
  for(int i = 0; i < size; i++){
    Serial.print( (char) payload[i]);
  } 
  Serial.println();

  char comando = payload[0];

  if (comando == '1'){
    digitalWrite(D1, HIGH);
  } else if (comando == '0'){
    digitalWrite(D1,LOW);
  }
}

void loop() {
  //1o - Aquisicao dos sinais de sensores
  luz = analogRead(A0);
  temp = (int) dht.readTemperature();
  umid = (int) dht.readHumidity();
  
  //2o - Formatacao desses dados
  sprintf(msg, "Luz %d - Temp: %d - Umid: %d", luz, temp, umid);
  
  //3o - Exibicao via Serial
  Serial.println(msg);

  sprintf(msg, "%d", temp);
  
  //4o - Evento ou Acao de Resposta
  if(luz < 500){
    Serial.println("Luz acesa");
    digitalWrite(D0, HIGH); //se luminosidade menor que 500 - acende led
  } else {
    //Caso contrario, luminosidade maior que 500, apaga led
    Serial.println("Luz apagada");
    digitalWrite(D0, LOW);
  }

  //5o - Comunicacao via MQTT
  if(mqtt.connected()){
    //Se temos uma conexao com o Broker MQTT - Vamos enviar os dados!
    mqtt.loop(); //eh um "ping" de conexao e atualiza dados com broker
    Serial.println("Enviando Publish de Mensagem MQTT...");
    Serial.println(msg);
    mqtt.publish("andreCurvello", msg); //1o argumento é o nome do canal
  } else {
    Serial.println("Conectando no Broker MQTT...");
    if (mqtt.connect("espCurvello")){ 
      //Conecta com ID espCurvello...
      mqtt.loop(); //ping com atualizacao de dados
      mqtt.subscribe("andreCurvelloLeds"); //canal de subscribe
    } else {
      Serial.println("Nao foi possivel conectar!");
    }
  }
  delay(5000); //atraso de 5s para interface com sensor de temp/umid
}
