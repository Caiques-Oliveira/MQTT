#include <SPI.h>
#include <UIPEthernet.h>
#include <utility/logging.h>
#include <PubSubClient.h>

byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED }; // mac addres é padrão do dispositivo. A placa ethernet do Arduino vem sem mac address registrado. O valor que colocamos nessa linha será o mac addres da placa ethernet do Arduino

void callback(char* topic, byte* payload, unsigned int length) { // callback é a função que aguarda retorno do mqtt, topic é a fila conectada e payload é a mensagem que recebemos da fila. Ex: if topic == Lampada e payload == 1, acende a lampada
  Serial.println(topic);
  // 
}

EthernetClient ethClient;
PubSubClient client("m10.cloudmqtt.com", 11356, callback, ethClient);
// se conectando no servidor da fila

long lastReconnectAttempt = 0; // está guardando quando foi a ultima vez que tentou se conectar (time out = tempo máximo de tentativa de conexão)

boolean reconnect() { // função de tentativa de reconexão (boolean é uma função que espera retorno True ou False - a resposta está na ultima linha da função)
  Serial.println("reconectando...");
  if (client.connect("arduinoClient", "charles", "charles")) {
    Serial.println("conectado");
    // client.publish("vaga/1","hello world");
    client.subscribe("vaga/#"); // # é o coringa, ou seja, nessa linha ele vai pegar tudo o que vier depois de "vaga/"
  }
  return client.connected(); // retorno True ou False
}

void setup()
{
  Serial.begin(9600);
  Serial.println("iniciando...");
  Ethernet.begin(mac);
  delay(1500);
  
  lastReconnectAttempt = 0; // desnecessário, pois já foi igualado a 0 lá em cima
}


void loop()
{
  if (!client.connected()) {
    long now = millis(); // millis guarda o tempo atual (em que essa função iniciou) em milisegundos
    if (now - lastReconnectAttempt > 5000) { // se o tempo atual - a ultima vez que houve conexão for maior que 5 segundos, ele vai tentar reconectar. É meio que um delay, mas sem travar o código.
      Serial.println("reconectando...");
      lastReconnectAttempt = now;
      
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    client.loop();
  }

}
