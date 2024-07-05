/* ================================================================ 
 *  14/07/2023 - TG 3 - Ivan Correia Lima Coqueiro RA: 11026416   
 *  Profs:     Dr. Olavo Luppi/ Dr. Fabrizio Leonardi       
 *  Programa - BLE Server (ESP32 DOIT V1 - Teste com potenciômetro)
 *  Este programa cria um servidor que notifica o cliente sobre o valor 
 *  de um float
 *  baseado no código de chegewara e de h2zero
 *  Revisão: 01/04/2024
 *  =============================================================== 
 */

// biblioteca nimBLE, BLE revisado pela APACHE para ser mais eficiente energeticamente
#include <NimBLEDevice.h>

    /** NOTA: Um 0x2902 descriptor não deve ser criado dado que a NimBLE irá automaticamente criá-la
     * se notify ou indicate estiver adotado em uma característica. Fonte: NimBLE (h2zero)
     */

//UUID  da caracteristica e do serviço (v4 UUID - generico da https://www.uuidgenerator.net/version4)
#define SERVICE_UUID  "4db917f0-6c43-470f-863b-247066515421"
#define CHARACTERISTIC_UUID  "8dbafdaa-eca2-45ef-9626-26944caa18b2"


//define um ponteiro para o servidor e cria um float
static NimBLEServer* pServer;
double potValue = 0.0;



//=================================== funções do programa(LEGACY CODE)============================================================

/*

float floatMap(float x, float entrada_min, float entrada_max, float saida_min, float saida_max) {
  //cria um mapa de resolução com valores quebrados ( valor atual - 0 )*[(3.3)/(4095)]+ saida_min. 
  //isso garante que o sinal fica entre 0 e 3.3V
  return (x - entrada_min) * (saida_max - saida_min) / (entrada_max - entrada_min) + saida_min;
  
}

float ReadPot(){
  // função para ler os valores contidos no potenciometro. GPIO 34 da Esp32:
  float analogValue = analogRead(34);
  float voltage = floatMap(analogValue, 0, 4095, 0, 3.3);
  return voltage;
  
}

*/
//===================================================================================================================================

void setup() {
  // inicia a leitura serial: 
  Serial.begin(922190);

  //nome do dispositivo
  NimBLEDevice::init("ESP32 Server");

  //define 9dB para a ESP32 em vez do padrão de -+3dB
  #ifdef ESP_PLATFORM
      NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
  #else
      NimBLEDevice::setPower(9); /** +9db */
  #endif

  //cria um servidor BLE - NimBLE
  pServer = NimBLEDevice::createServer();

  //cria um serviço BLE - NimBLE
  NimBLEService* pPotService = pServer->createService(SERVICE_UUID);

  //cria uma característica BLE - NimBLE
  NimBLECharacteristic* pForceCharacteristic = pPotService->createCharacteristic(
                                              CHARACTERISTIC_UUID,
                                              NIMBLE_PROPERTY::NOTIFY
                                              );

  pPotService->start();
  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    
    /** serviços que terão dados transmitidos **/
  pAdvertising->addServiceUUID(pPotService->getUUID());
  pAdvertising->setScanResponse(true);
  pAdvertising->start();
  Serial.print("Começou a Transmissão (advertising)");
  Serial.println("Tempo (millis): ");
  Serial.print(millis());
}

void loop() {
  // rotina geral:
  

//LEGACY CODE =========================================================================================
  //le os valores do potenciometro
 // float potValue = ReadPot();
/*
  Serial.print("Valor lido (ADC): ");
  Serial.println(potValue);
  Serial.print("Tempo (millis):");
  Serial.print(millis());
  Serial.println();
*/
//======================================================================================================


    /** rotina de enviar as notificações aos clientes conectados */
    if(pServer->getConnectedCount()) {
        //obtém o serviço pelo UUID
        NimBLEService* pSvc = pServer->getServiceByUUID(SERVICE_UUID);
        if(pSvc) {
            //obtem a caracaterística do serviço pelo UUID da characteristic
            NimBLECharacteristic* pChr = pSvc->getCharacteristic(CHARACTERISTIC_UUID);
            if(pChr) {
                //notifica outro dispositivo bluetooth com uma string sobre o valor em volts do pot
                //dtostrf(potValue, 7, 2, potValueVolts);
                //pChr->setValue(potValueVolts);

                pChr->setValue((uint8_t*)&potValue, sizeof(potValue));
                pChr->notify(true);
                Serial.print("Valor Pot: ");
                //Serial.println(potValueVolts);
                Serial.println(potValue);
                Serial.print("Tempo (millis):");
                Serial.print(millis());
                Serial.println();
                potValue = potValue + 0.10;
            }
        }
    }
     
}
