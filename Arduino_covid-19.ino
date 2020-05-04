
/*Version 1.0*/

#include <ArduinoJson.h>
#include <pt.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LED_1_PIN 9
#define LED_2_PIN 6
#define LED_3_PIN 8
#define LED_4_PIN 5
#define LED_5_PIN 4
#define LED_6_PIN 2
#define LED_7_PIN 3
#define LED_8_PIN 7

LiquidCrystal_I2C lcd(0x3F, 16, 2);
const size_t CAPACITY = JSON_ARRAY_SIZE(5) + 5 * JSON_OBJECT_SIZE(12) + 706;
String response;
char risposta[1500];
String title;
String res;
struct nation {
  String  stato;
  unsigned long positivi;
};

struct nation nat[5];
int N = 5;
bool flag = false;

static struct pt pt1, pt2;


static int first(struct pt *pt)
{


  static unsigned long lastTimeBlink = 0;
  static unsigned long dela = 225;
  PT_BEGIN(pt);
  while (dela > 0) {

    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > dela);
    digitalWrite(LED_1_PIN, LOW);
    digitalWrite(LED_3_PIN, LOW);
    digitalWrite(LED_5_PIN, LOW);
    digitalWrite(LED_7_PIN, LOW);
    digitalWrite(LED_2_PIN, HIGH);
    digitalWrite(LED_4_PIN, HIGH);
    digitalWrite(LED_6_PIN, HIGH);
    digitalWrite(LED_8_PIN, HIGH);

    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > dela);
    digitalWrite(LED_1_PIN, HIGH);
    digitalWrite(LED_3_PIN, HIGH);
    digitalWrite(LED_5_PIN, HIGH);
    digitalWrite(LED_7_PIN, HIGH);
    digitalWrite(LED_2_PIN, LOW);
    digitalWrite(LED_4_PIN, LOW);
    digitalWrite(LED_6_PIN, LOW);
    digitalWrite(LED_8_PIN, LOW);
    dela = dela - 1;
  }
  dela = 225;
  flag= false;
    Serial.println("FINE PROTOTHREAD 1");
  PT_END(pt);

}

static int second(struct pt *pt)
{
  static unsigned long lastTimeBlink = 0;
  static unsigned int i = 0;
  PT_BEGIN(pt);

  lastTimeBlink = millis();
  PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 2000);
  lcd.clear();
  lcd.print("    COVID-19    ");
  lcd.setCursor(0, 1);
  lcd.print("Confirmed  cases");

  lastTimeBlink = millis();
  PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 7000);

  for (; i < N; i++) {
    lcd.clear();
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 2000);

    lcd.setCursor(0, 0);
    title = String(N - i) + ". ";
    lcd.print(title);
    //lcd.print(" ");
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 2000);
    lcd.setCursor(3, 0);
    lcd.print(nat[i].stato);
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 2000);
    lcd.setCursor(0, 1);
    res = String(nat[i].positivi) + " positive ";
    lcd.print(res);
    lcd.print(nat[i].positivi);
    lastTimeBlink = millis();
    PT_WAIT_UNTIL(pt, millis() - lastTimeBlink > 2000);
    lcd.clear();
  }

  //printLCD();
  lcd.clear();
  flag = false;
  i=0;
  Serial.println("FINE PROTOTHREAD 2");
  
  PT_END(pt);

}

void setup()
{
  Serial.begin(9600);           // start serial for output
  lcd.begin(44, 43);
  lcd.init();
  lcd.backlight();
  Wire.setClock(10000);
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  pinMode(LED_4_PIN, OUTPUT);
  pinMode(LED_5_PIN, OUTPUT);
  pinMode(LED_6_PIN, OUTPUT);
  pinMode(LED_7_PIN, OUTPUT);
  pinMode(LED_8_PIN, OUTPUT);
  response.reserve(1200);
  //risposta.reserve(1200);


  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  PT_INIT(&pt1);
  PT_INIT(&pt2);
}

void loop()
{

  if (flag) {
    first(&pt1);
    second(&pt2);
  }
  //delay(1000);

}




void buildJson()
{

  response.toCharArray(risposta, response.length() + 1);
  Serial.println();
  Serial.println(risposta);
  Serial.flush();
  StaticJsonDocument<CAPACITY>doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, risposta);

  // Test if parsing succeeds.
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Fetch values.
  //

  JsonArray jsonarr = doc.as<JsonArray>();
  for (int i = 0; i < N; i++)
  {
    JsonVariant v = jsonarr[i]["country"];
    //Serial.println(v.as<String>());
    nat[i].stato = v.as<String>();
    JsonVariant w = jsonarr[i]["cases"];
    //Serial.println(w.as<String>());
    nat[i].positivi = w.as<unsigned long>();
  }

  int imin, k, p;
  struct nation temp;


  for (k = 0; k < N - 1; k++)
  {
    imin = k;
    for (p = k + 1; p < N; p++)
    {
      if (nat[p].positivi < nat[imin].positivi) {
        imin = p;
      }

    }
    temp = nat[k];
    nat[k] = nat[imin];
    nat[imin] = temp;
  }

  for (int i = 0; i < N; i++)
  {
    Serial.println(nat[i].stato);
    Serial.println(String(nat[i].positivi));
    Serial.println();
  }

  //lcd.clear();
  //printLCD();
  response = "";
  flag = true;
}

void check()
{


  int endIndex = response.indexOf('$');

  if (endIndex != -1) {


    Serial.println();
    Serial.println(endIndex);
    Serial.flush();
    //delay(2000);
    //response = response.substring(5, endIndex);
    //risposta += response;

    response.remove(endIndex);   // response.remove(0);
    response.remove(0, 5);
    //delay(5000);
    response = '[' + response;
    //risposta.concat(response);
    Serial.flush();
    //delay(5000);
    //risposta.trim();
    Serial.println();
    Serial.println(response);
    Serial.flush();
    buildJson();

  }
}



// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while (Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    // print the character

    Serial.print(c);
    response += c;


    Serial.flush();
  }
  //Serial.println("----FINE----");
  check();
}
