/**
*===========================================================================
 * CLIENTE BLE - NimBLE (TG 3)
 * Autor: Ivan Correia Lima Coqueiro
 * Orientadores: Prof. Dr. Olavo Luppi Silva / Prof. Dr. Fabrizio Leonardi
 * RA 11026416
 * Baseado no código de chegewara e H2zero
 * Revisão - 01/04/2024
 ===========================================================================
 **/

//caracteristica e serviço BLE inscrito
#include "NimBLEDevice.h"
#define SERVICE_UUID  "4db917f0-6c43-470f-863b-247066515421"
#define CHARACTERISTIC_UUID  "8dbafdaa-eca2-45ef-9626-26944caa18b2"


// Indica o serviço que queremos conectar
static BLEUUID serviceUUID(SERVICE_UUID);
// Indica a caracteristica do serviço que se deseja conectar
static BLEUUID    charUUID(CHARACTERISTIC_UUID);

// declara as variaveis booleanas para conectividade, e caracteristicas. Original de chegewara e H2Zero.
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* meuDispositivo;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    //float Value = *(float*)pData;
    Serial.print("Valor Pot: ");
    // operador deref. Deixa trabalhar com o valor exato em vez da sua pos na memória
    for(int i=0;i<length;i++){
    Serial.println(*(uint8_t*)pData[i]);
    //Serial.println((char*)pData);
    }
    Serial.print("Tempo (millis): ");
    Serial.println(millis());
}

/*******************************************************************/


bool conectaNoServidor() {
  
    // cria um cliente para conectar no servidor
    BLEClient*  pClient  = BLEDevice::createClient();
    //pClient->setClientCallbacks(new MyClientCallback());
    // Conecta ao servidor ESP32 BLE
    pClient->connect(meuDispositivo);
    Serial.println(" - Conectado ao servidor");

    // verifica o serviço que queremos parear
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);

    // verifica a caracteristica do serviço desejado
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);


    // le o valor da caracteristica
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("Valor da Caracterstica: ");
      Serial.println(value.c_str());
    }

    //verifica o serviço como notify
    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    connected = true;
    return true;
}

/**
 * Escaneia os serviços BLE e pareia com o desejado
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {

   
  void onResult(BLEAdvertisedDevice* advertisedDevice) {
    Serial.print("Dispositivo encontrado: ");
    Serial.println(advertisedDevice->toString().c_str());

    if (advertisedDevice->haveServiceUUID() && advertisedDevice->isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();

      meuDispositivo = advertisedDevice; 
      doConnect = true;
      doScan = true;

    } 
  } 
}; 


void setup() {
  Serial.begin(922190);
  pinMode(LED_BUILTIN, OUTPUT); //acende o led para determinar se a placa está conectada
  Serial.println("Iniciando cliente");
  BLEDevice::init("");

  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(45);
  pBLEScan->setWindow(15);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
  Serial.println("Setup BLE Client (millis):");
  Serial.println(millis());
} 


void loop() {
  // aqui configura a conexão para verdadeira. Usa as variaveis doConnect para verificar
  if (doConnect == true) {
    if (conectaNoServidor()) {
      
      Serial.println();
      Serial.print("Conectamos no servidor ESP32-Strain-Gauge.");
      Serial.print("Tempo (millis): ");
      Serial.println(millis());
      Serial.println();
    } 
    doConnect = false;
  }
  
// pareia os tempos da conexão e acende o  LED
  if (connected) {
     digitalWrite(LED_BUILTIN, HIGH);
     //delay(100); 
  }else if(doScan){
    digitalWrite(LED_BUILTIN, LOW); 
    //delay(100); 
    BLEDevice::getScan()->start(0); 

  }
  
} 
