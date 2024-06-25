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
byte authorizedIDs[] = {1, 26, 27, 28,}; // Adicione quantos IDs desejar

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
