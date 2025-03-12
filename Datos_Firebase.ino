
// **CONFIGURACIÓN WIFI Y FIREBASE**
#include <WiFi.h>
#include <FirebaseESP32.h>

// **CONFIGURACIÓN WIFI**
#define WIFI_SSID "red"
#define WIFI_PASSWORD "contraseña"

// **CONFIGURACIÓN FIREBASE**
#define FIREBASE_HOST "url"  // URL de Firebase
#define FIREBASE_AUTH "secreto" // Se obtiene en Firebase

FirebaseData firebaseData;  // Objeto para manejar la comunicación con Firebase
FirebaseConfig config;      // Configuración de Firebase
FirebaseAuth auth;          // Autenticación de Firebase

// **CONFIGURACIÓN DEL SENSOR ULTRASÓNICO**
const int trigPin = 4;
const int echoPin = 16;

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

unsigned long startTime;
const unsigned long runTime = 10000;  // Tiempo en milisegundos (60 segundos)

void setup() {
    Serial.begin(115200);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

    // **Conectar a WiFi**
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Conectando a WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado a WiFi!");

    // **Configurar Firebase**
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;

    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    startTime = millis();  // Guarda el tiempo de inicio
}

void loop() {

    if (millis() - startTime < runTime) {
         // Medir la distancia con el sensor ultrasónico
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        
        duration = pulseIn(echoPin, HIGH);
        distanceCm = duration * SOUND_SPEED / 2;
        distanceInch = distanceCm * CM_TO_INCH;

        Serial.print("Distance (cm): ");
        Serial.println(distanceCm);

        // **Generar un timestamp en milisegundos**
        String timestamp = String(millis());

        // **Guardar en Firebase con una clave única**
        String path = "/pf_aire_4/" + timestamp; // Crea una ruta única para cada medición

        if (Firebase.setFloat(firebaseData, path, distanceCm)) {
            Serial.println("Datos enviados a Firebase correctamente.");
        } else {
            Serial.print("Error al enviar datos: ");
            Serial.println(firebaseData.errorReason());
        }

        delay(33);  // Esperar 2 segundos antes de la siguiente medición
    } else {
        Serial.println("Tiempo límite alcanzado, deteniendo medición.");
        while (true);  // Detiene el loop
    }
   
}
