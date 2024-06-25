# Projeto de Tranca Eletrônica com Reconhecimento Biométrico


![Projeto de Tranca Eletrônica](https://user-images.githubusercontent.com/your-image.png)

## Índice
1. [Introdução](#introdução)
2. [Apresentação do Projeto](#apresentação-do-projeto)
3. [Objetivos](#objetivos)
4. [Componentes Necessários](#componentes-necessários)
5. [Diagrama de Circuito](#diagrama-de-circuito)
6. [Montagem do Hardware](#montagem-do-hardware)
7. [Desenvolvimento do Software](#desenvolvimento-do-software)
8. [Código-Fonte](#código-fonte)
9. [Testes e Validação](#testes-e-validação)
10. [Fotos do Projeto Apresentado](#fotos-do-projeto-apresentado)
11. [Conclusão](#conclusão)
12. [Referências](#referências)
13. [Projeto Desenvolvido Por](#projeto-desenvolvido-por)

## Introdução
Este projeto tem como objetivo desenvolver um protótipo de tranca eletrônica com reconhecimento biométrico utilizando a plataforma Arduino. A ideia é criar um sistema de segurança que permita o acesso somente a indivíduos autorizados através da leitura de suas impressões digitais.

## Apresentação do Projeto
Este projeto foi apresentado para a empresa interessada na tranca eletrônica (Pague Bem Construções) e na Universidade Estácio de Sá, campus West Shopping.

## Objetivos
- Desenvolver um sistema de tranca eletrônica seguro e confiável.
- Utilizar um sensor biométrico para reconhecimento de impressões digitais.
- Integrar o sensor biométrico com o Arduino para controle da tranca.
- Fornecer uma solução acessível e de baixo custo.

## Componentes Necessários
- 1 x Arduino Uno
- 1 x Sensor biométrico
- 1 x Módulo Relé
- 1 x Tranca solenoide
- 3 x Jumpers macho-fêmea
- 2 x Jumpers macho-macho
- 1 x Bateria de 12V e 5 Ampères-hora

## Diagrama de Circuito
![Diagrama de Circuito](https://user-images.githubusercontent.com/your-diagram.png)

## Montagem do Hardware
1. **Conexão do Sensor Biométrico ao Arduino**:
   - VCC do sensor ao 5V do Arduino.
   - GND do sensor ao GND do Arduino.
   - TX do sensor ao pino digital 2 do Arduino.
   - RX do sensor ao pino digital 3 do Arduino.
   
2. **Conexão do Módulo Relé ao Arduino**:
   - VCC do módulo relé ao 5V do Arduino.
   - GND do módulo relé ao GND do Arduino.
   - IN do módulo relé ao pino digital 4 do Arduino.
   
3. **Conexão da Tranca Elétrica ao Módulo Relé**:
   - Conecte a tranca elétrica ao relé conforme especificado no diagrama do módulo.

## Desenvolvimento do Software
O software será desenvolvido em C++ utilizando a IDE do Arduino. Será necessário utilizar a biblioteca `Adafruit_Fingerprint` para integrar o sensor biométrico com o Arduino.

## Instalação da Biblioteca
1. Abra a IDE do Arduino.
2. Vá em **Sketch** > **Include Library** > **Manage Libraries**.
3. Procure por "Adafruit Fingerprint Sensor Library" e clique em instalar.

## Código-Fonte
### Funcionamento
```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

#define BUZZER 4    // Define o pino do buzzer
#define LED 6       // Pino do LED
#define SOLENOID 5  // Define o pino para a solenoide

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
LiquidCrystal_I2C dis(0x27, 16, 2);

// Lista de IDs de impressão digital autorizados
byte authorizedIDs[] = {1, 26, 27, 28}; // Adicione quantos IDs desejar

void setup() {
  Serial.begin(9600);
  dis.init();
  dis.backlight();
  dis.setCursor(0, 0);
  dis.print("Aproxime o Dedo");
  pinMode(BUZZER, OUTPUT);    // Configura o pino do buzzer como saída
  pinMode(LED, OUTPUT);       // Configura o pino do LED como saída
  pinMode(SOLENOID, OUTPUT);  // Configura o pino da solenoide como saída
  digitalWrite(SOLENOID, HIGH); // Garante que a solenoide esteja desligada inicialmente
  while (!Serial);  // Para Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // Inicializa o sensor de impressão digital
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  finger.getParameters();
  finger.getTemplateCount();
  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  } else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
}

void loop() {
  int value = getFingerprintIDez();

  // Verifica se o ID da impressão digital está na lista de IDs autorizados
  bool accessGranted = false;
  for (int i = 0; i < sizeof(authorizedIDs); i++) {
    if (value == authorizedIDs[i]) {
      accessGranted = true;
      break;
    }
  }

  if (accessGranted) {
    dis.setCursor(0, 0);
    dis.print("                "); // Limpa a linha superior
    dis.setCursor(0, 1);
    dis.print("ACESSO PERMITIDO");

    digitalWrite(LED, HIGH);    // Liga o LED
    digitalWrite(SOLENOID, LOW); // Ativa a solenoide para destrancar a porta
    digitalWrite(BUZZER, HIGH); // Ativa o buzzer

    delay(2000); // Mantém a solenoide e o buzzer ativados por 2 segundos (ajustado conforme necessário)

    digitalWrite(SOLENOID, HIGH); // Desativa a solenoide
    digitalWrite(BUZZER, LOW);    // Desativa o buzzer
    digitalWrite(LED, LOW);       // Desliga o LED

    delay(2000);

    dis.setCursor(0, 1);
    dis.print("                "); // Limpa a linha inferior
    dis.setCursor(0, 0);
    dis.print("APROXIME O DEDO");

  } else {
    dis.setCursor(0, 1);
    dis.print("                "); // Limpa a linha inferior
  }
  delay(50);
}

// Função para obter o ID da impressão digital
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p == FINGERPRINT_NOFINGER) {
    Serial.println("No finger detected");
    return -1;
  } else if (p != FINGERPRINT_OK) {
    Serial.println("Error getting image");
    return -1;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Error converting image");
    return -1;
  }

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) {
    Serial.println("No match found");
    return -1;
  }

  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence "); Serial.println(finger.confidence);
  return finger.fingerID;
}
```
### Enrolamento Digital
```cpp
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

LiquidCrystal_I2C dis(0x27, 16, 2);
SoftwareSerial mySerial(2, 3); // TX/RX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

void setup() {
  Serial.begin(9600);
  dis.init();
  dis.backlight();
  dis.setCursor(0, 0);
  dis.print("Place finger");
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }

  finger.getParameters();
}

uint8_t readnumber

(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop() {
  Serial.println("Ready to enroll a fingerprint! Please type in the ID # you want to save this finger as...");
  id = readnumber();
  Serial.print("Enrolling ID #");
  Serial.println(id);

  while (!  getFingerprintEnroll());
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  // OK success!
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}
```
## Testes e Validação
O sistema foi testado exaustivamente para garantir seu funcionamento adequado. Foram realizados testes com múltiplas impressões digitais para verificar a precisão e confiabilidade do reconhecimento biométrico.

## Fotos do Projeto Apresentado
![Foto do Projeto](https://user-images.githubusercontent.com/your-photo.png)

## Conclusão
Este projeto demonstrou com sucesso a viabilidade de um sistema de tranca eletrônica utilizando reconhecimento biométrico. O protótipo criado é seguro, eficiente e de baixo custo, apresentando um potencial considerável para aplicações residenciais e comerciais.

## Referências
- [Documentação da Biblioteca Adafruit Fingerprint Sensor](https://learn.adafruit.com/adafruit-optical-fingerprint-sensor)
- [Arduino Uno Documentation](https://www.arduino.cc/en/Main/ArduinoBoardUno)
- [Como Usar o Sensor de Impressão Digital com Arduino](https://www.instructables.com/id/How-to-Use-Fingerprint-Sensor-With-Arduino/)

## Projeto Desenvolvido Por
Gabriel Alves Campos  
Guilherme de Araujo Nunes  
Larissa Gabriel dos Santos  
Leonardo Correia de Lima  
Ryan Costa Ferreira  


2024.1
Rio de Janeiro, RJ
Universidade Estácio de Sá
