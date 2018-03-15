#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char *wifi_ssid PROGMEM = "ssid";
const char *wifi_password PROGMEM = "password";

const char *cloud_host PROGMEM = "eu.coresuite.com";
const char *cloud_account PROGMEM = "account";
const char *cloud_company PROGMEM = "company";
const char *cloud_user PROGMEM = "user";
const char *cloud_password PROGMEM = "password";

WiFiClientSecure client;

void setup(void) {

    Serial.begin(115200);
    Serial.setTimeout(2000);

    // Wait for serial to initialize.
    while (!Serial) { }

    Serial.print("Hello!");

    WiFi.begin(wifi_ssid, wifi_password);

    unsigned long wifiConnectStart = millis();

    while (WiFi.status() != WL_CONNECTED) {
        // Check to see if
        if (WiFi.status() == WL_CONNECT_FAILED) {
            Serial.println("Failed to connect to WiFi. Please verify credentials: ");
            delay(10000);
        }

        delay(500);
        Serial.println("...");
        // Only try for 5 seconds.
        if (millis() - wifiConnectStart > 15000) {
            Serial.println("Failed to connect to WiFi");
            return;
        }
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

String createServiceCall() {
    if (client.connect(cloud_host, 443)) {
        String data = "{"
            "\"subject\": \"I'm so thirsty\", "
            "\"priority\": \"MEDIUM\", "
            "\"statusName\": \"Ready to plan\", "
            "\"typeName\": \"Maintenance\", "
            "\"statusCode\": \"-2\", "
            "\"typeCode\": \"-4\", "
            "\"businessPartner\": \"0113FEDD7C9D4C4085567BDBBF547EF1\","
			"\"responsibles\": [\"60A745386C71444B96B29FBCCFC46C39\" ],"
            "\"originName\": \"Water Sensor\" }";
        client.println("POST /api/data/v4/ServiceCall?"
            "clientIdentifier=COR_SERVICE_CLOUD&"
            "accountName=" + (String)cloud_account + "&"
            "userAccountName=" + (String)cloud_user + "&"
            "userAccountPassword=" + (String)cloud_password + "&"
            "companyName=" + (String)cloud_company + "&"
            "dtos=ServiceCall.21 HTTP/1.1");
        client.println("Host: " + (String)cloud_host);
        client.println("User-Agent: ESP8266/1.0");
        client.println("Connection: close");
        client.println("Content-Type: application/json;");
        client.print("Content-Length: ");
        client.println(data.length());
        client.println();
        client.println(data);
        delay(10);
        String response = client.readString();
        int bodypos =  response.indexOf("\r\n\r\n") + 4;
        return response.substring(bodypos);
    } else {
        return "ERROR";
    }
}

void loop(void) {

    int sensorValue = analogRead(A0);
    Serial.println(sensorValue);
    if (sensorValue < 100) {
        Serial.println(createServiceCall());
        ESP.deepSleep(24 * 3600 * 1000000);
    }
    ESP.deepSleep(3600 * 1000000);
}

