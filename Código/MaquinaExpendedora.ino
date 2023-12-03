/*
  Máquina expendedora
  Resolución práctica 3
  Sitemas Empotrados y de Tiempo Real
  Liam Saborido Sueiro
 */


#include <LiquidCrystal.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Thread.h>
#include <StaticThreadController.h>
#include <ThreadController.h>
#include <TimerOne.h>
#include <ezButton.h>
#include <avr/wdt.h>

const int led_pin_1 = A1;
const int led_pin_2 = 6;
const int button_pin = 3;

// Variables para interrupcion
volatile bool button_pressed = false;
volatile unsigned long button_press_start_time = 0;
volatile unsigned long button_press_end_time = 0;
volatile unsigned long duration_button_pressed = 0;

const int trig_pin = 7; // Pin de salida del pulso de ultrasonidos (Trigger)
const int echo_pin = 8; // Pin de entrada del eco de ultrasonidos


#define VRX_PIN  A4 // Arduino pin connected to VRX pin
#define VRY_PIN  A5 // Arduino pin connected to VRY pin
const int sw_pin = 13;
ezButton button(sw_pin);


LiquidCrystal lcd(12, 11, 5, 4, 10, 2); // Definir LCD con librería LiquidCrystal



#define DHTPIN 9
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

int state = 0; // 0: Arranque, 1: Servicio, 2: Admin
int selected_product = 0;
int temperature, humidity;
int distance;
int joystick_pressed = 10;
int left_joystick = 11;
int right_joystick = 12;

float prices[] = {1.00, 1.10, 1.25, 1.50, 2.00}; // Precios iniciales
String products[] = {"Cafe Solo", "CafeCortado", "Cafe Doble", "CafePremium", "Chocolate"}; //Productos


void setup() {
  // Configurar el pin del LED como salida
  pinMode(led_pin_1, OUTPUT);
  pinMode(led_pin_2, OUTPUT);

  // Configurar el pin del botón como entrada
  pinMode(button_pin, INPUT_PULLUP);
  Serial.begin(9600); // Iniciar la comunicación serial
  
  // Watchdog
  wdt_disable();

  // Configurar el ultrasonido
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  // Configurar el DHT
  dht.begin(); // Comenzar el sensor DHT
  lcd.begin(16, 2); // Configurar el LCD

  //Configurar interrupcion
  attachInterrupt(digitalPinToInterrupt(button_pin), buttonISR, CHANGE);

  // Watchdog de 8 segundos
  wdt_enable(WDTO_8S);
}

void loop() {

  switch (state) {
    case 0:
      start_up();
      break;
    case 1:
      service();
      break;
    case 2:
      admin();
      break;
  }

  wdt_reset();
}

void start_up() {

  wdt_reset();
  lcd.clear();
  lcd.print("CARGANDO ...");

  unsigned long start_time = millis();
  int blinkCount = 0;
  int led_state = HIGH;
  int interval = 1000;  // Intervalo de parpadeo en milisegundos

  while (blinkCount < 6) {  // Parpadear 3 veces (6 eventos alternantes de encendido/apagado)
    if (millis() - start_time >= interval) {
      digitalWrite(led_pin_1, led_state);
      led_state = (led_state == HIGH) ? LOW : HIGH;  // Alternar el estado del LED
      start_time = millis();
      blinkCount++;
    }

    // Verificar si el botón fue presionado durante un rango de tiempo (5 segundos)
    if ((duration_button_pressed >= 5000)) {
      state = 2;  // Cambiar al estado de Admin
      button_press_start_time = 0;
      duration_button_pressed = 0;
      button_pressed = false;
      break;  // Salir de la función y pasar al estado de Admin
    }
  }

  wdt_reset();
  lcd.clear();
  state = 1;
}

void service() {

  wdt_reset();
  lcd.clear();

  unsigned long start_time = millis();
  bool service_completed = false;
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  if (check_distance() < 100) {
    while (!service_completed) {
      unsigned long currentTime = millis();

      if (currentTime - start_time >= 500 && currentTime - start_time < 3000) {
        lcd.clear();
        lcd.print("Temp:        ");
        lcd.print(temperature);
        lcd.print("C ");
      } else if (currentTime - start_time >= 3000 && currentTime - start_time < 5500) {
        lcd.clear();
        lcd.print("Hum:         ");
        lcd.print(humidity);
        lcd.print("% ");
      } else if (currentTime - start_time >= 5500) {
        lcd.clear();
        display_products();
        lcd.clear();
        service_completed = true;  // Marcar el servicio como completado
      }
    }
    wdt_reset();
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   ESPERANDO");

    lcd.setCursor(0, 1);
    lcd.print("    CLIENTE");
    delay(100);

    if (millis() - start_time >= 5000) {
      // Marcar el servicio como completado después de 5000 milisegundos
      service_completed = true;
    }
    wdt_reset();
  }
  wdt_reset();
}

float check_distance() {
  // Inicializar el sensor de ultrasonido
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);

  // Leer el pulso de eco
  unsigned long duration = pulseIn(echo_pin, HIGH);

  // Calcular la distancia en centímetros
  float distance = duration * 0.034 / 2; // El factor 0.034 es para convertir el tiempo a centímetros

  return distance;
  delay(500);
}

void display_products() {

  wdt_reset();
  lcd.clear();

  int joystick_action;
  String product;

  unsigned long previous_millis = 0;
  const long interval = 100;

  while (true) {
    // Verificar si el botón fue presionado durante un rango de tiempo (2-3 segundos)
    if ((duration_button_pressed >= 2000) && (duration_button_pressed <= 3000)){
      // Reiniciar el estado (volver al start_up)
      state = 0;  // Cambiar al estado de start_up
      button_press_start_time = 0;
      button_pressed = false;
      duration_button_pressed = 0;
      break;
    }else if ((duration_button_pressed >= 5000)) {
      state = 2;  // Cambiar al estado de Admin
      button_press_start_time = 0;
      duration_button_pressed = 0;
      button_pressed = false;
      break;  // Salir de la función y pasar al estado de Admin
    }

    joystick_action = read_joystick();

    if (joystick_action == 10) { // Si el botón del joystick está presionado
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("  PREPARANDO");
      lcd.setCursor(0, 1);
      lcd.print(product);

      int intense = 0;
      int preparation_time = random(4, 9);
      int step_time = 255 / preparation_time;
      unsigned long prep_start_time = millis();

      while (millis() - prep_start_time < preparation_time * 1000) {
        analogWrite(led_pin_2, intense);
        intense += step_time;

        // Verificar si el botón fue presionado durante un rango de tiempo (2-3 segundos)
        if ((duration_button_pressed >= 2000) && (duration_button_pressed <= 3000)){
          // Reiniciar el estado (volver al start_up)
          state = 0;  // Cambiar al estado de start_up
          button_press_start_time = 0;
          button_pressed = false;
          duration_button_pressed = 0;
          break;
        }else if ((duration_button_pressed >= 5000)) {
          state = 2;  // Cambiar al estado de Admin
          button_press_start_time = 0;
          duration_button_pressed = 0;
          button_pressed = false;
          break;  // Salir de la función y pasar al estado de Admin
        }
        wdt_reset();
      }
      wdt_reset();

      analogWrite(led_pin_2, 0);
      lcd.clear();
      lcd.print("RETIRE BEBIDA");

      // Usar millis() para esperar sin bloquear
      unsigned long displaystart_time = millis();
      while (millis() - displaystart_time < 3000) {
        // Verificar si se debe reiniciar al estado 0 durante la espera
        // Verificar si el botón fue presionado durante un rango de tiempo (2-3 segundos)
        if ((duration_button_pressed >= 2000) && (duration_button_pressed <= 3000)){
          // Reiniciar el estado (volver al start_up)
          state = 0;  // Cambiar al estado de start_up
          button_press_start_time = 0;
          button_pressed = false;
          duration_button_pressed = 0;
          break;
        }else if ((duration_button_pressed >= 5000)) {
          state = 2;  // Cambiar al estado de Admin
          button_press_start_time = 0;
          duration_button_pressed = 0;
          button_pressed = false;
          break;  // Salir de la función y pasar al estado de Admin
        }
        wdt_reset();
      }
      wdt_reset();

      break; // Salir del bucle

    } else if (joystick_action >= 0 && joystick_action <= 4) {
      lcd.clear();
      switch (joystick_action) {
        case 0:
          lcd.print(products[0]);
          lcd.print("   ");
          lcd.print(prices[0]);
          product = "Cafe Solo";
          break;
        case 1:
          lcd.print(products[1]);
          lcd.print(" ");
          lcd.print(prices[1]);
          product = "Cafe Cortado";
          break;
        case 2:
          lcd.print(products[2]);
          lcd.print("  ");
          lcd.print(prices[2]);
          product = "Cafe Doble";
          break;
        case 3:
          lcd.print(products[3]);
          lcd.print(" ");
          lcd.print(prices[3]);
          product = "Cafe Premium";
          break;
        case 4:
          lcd.print(products[4]);
          lcd.print("   ");
          lcd.print(prices[4]);
          product = "Chocolate";
          break;
      }
    }
    wdt_reset();
  }
  wdt_reset();
}


int read_joystick() {
  int x_value = analogRead(VRX_PIN); // Valor del eje X
  int y_value = analogRead(VRY_PIN); // Valor del eje Y

  button.loop();
  int b_value = button.getState();

  // Incrementar o decrementar la opción seleccionada según la posición del joystick
  if (y_value < 100 && selected_product > 0) {
    selected_product--; // Mover hacia arriba
  } else if (y_value > 900 && selected_product < 4) {
    selected_product++; // Mover hacia abajo
  }else if (b_value == 0) {
    return joystick_pressed; // Botón pulsado
  } else if (x_value < 100){
    return left_joystick; // Joystick a la izquierda
  } else if (x_value > 900){
    return right_joystick; // Joystick a la derecha
  }

  // Asegurarse de que la opción no exceda ciertos límites (ajustar según tus necesidades)
  selected_product = constrain(selected_product, 0, 4);

  // Si no se ha realizado ninguna acción del joystick, devolver -1
  delay(500);
  return selected_product;
}

void admin() {

  wdt_reset();
  lcd.clear();
  digitalWrite(led_pin_1, HIGH);
  digitalWrite(led_pin_2, HIGH);

  int admin_state = 0;

  unsigned long previous_millis = 0;
  const long interval = 100;

  while (true) {

    wdt_reset();
    int joystick_action = read_joystick();

    if (joystick_action == 10) { // Si el botón del joystick está presionado
      switch (admin_state) {
        case 0:
          admin_temperature();
          break;
        case 1:
          admin_distance();
          break;
        case 2:
          admin_counter();
          break;
        case 3:
          admin_modifyPrices();
          break;
      }
    } else if (joystick_action >= 0 && joystick_action <= 4) {
      wdt_reset();
      lcd.clear();
      switch (joystick_action) {
        case 0:
          lcd.print("Ver temperatura");
          admin_state = 0;
          break;
        case 1:
          lcd.print("Ver distancia");
          admin_state = 1;
          break;
        case 2:
          lcd.print("Ver contador");
          admin_state = 2;
          break;
        case 3:
          lcd.print("Modificar precios");
          admin_state = 3;
          break;
      }
    }

    if ((duration_button_pressed >= 5000)){
      // Reiniciar el estado (volver al Servicio)
      state = 1;  // Cambiar al estado de Servicio
      button_press_start_time = 0;
      button_pressed = false;
      duration_button_pressed = 0;
      digitalWrite(led_pin_1, LOW);
      digitalWrite(led_pin_2, LOW);
      break;
    }
  }
  wdt_reset();
}



void admin_temperature() {

  wdt_reset();
  lcd.clear();
  
  unsigned long previous_millis = 0;
  const long interval = 500;

  while (true) {
    int joystick_action = read_joystick();
    
    unsigned long currentMillis = millis();

    if (currentMillis - previous_millis >= interval) {
      // Actualizar cada 500 milisegundos (0.5 segundos)
      previous_millis = currentMillis;

      int temperature_admin = dht.readTemperature();
      int humidity_admin = dht.readHumidity();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(temperature_admin);
      lcd.print("C ");
      lcd.setCursor(0, 1);
      lcd.print("C Hum: ");
      lcd.print(humidity_admin);
      lcd.print("% ");
    }

    if (joystick_action == 11) {
      break;
    }
    wdt_reset();
  }
  wdt_reset();
}


void admin_distance() {

  wdt_reset();
  lcd.clear();
  
  unsigned long previous_millis = 0;
  const long interval = 500;

  while (true) {
    int joystick_action = read_joystick();
    
    unsigned long currentMillis = millis();

    if (currentMillis - previous_millis >= interval) {
      // Actualizar cada 500 milisegundos (0.5 segundos)
      previous_millis = currentMillis;

      float distance_admin = check_distance();

      lcd.clear();
      lcd.print("Distancia: ");
      lcd.print(distance_admin);
      lcd.print("cm ");
    }

    if (joystick_action == 11) {
      break;
    }
    wdt_reset();
  }
  wdt_reset();
}


void admin_counter() {

  wdt_reset();
  lcd.clear();
  

  while (true) {
    int joystick_action = read_joystick();

    unsigned long elapsed_time = millis();

    lcd.clear();
    lcd.print("Contador: ");
    lcd.print(elapsed_time / 1000); // Muestra el tiempo transcurrido en segundos
    lcd.print("s ");

    if (joystick_action == 11) {
      break;
    }
    wdt_reset();
  }
  wdt_reset();
}



void admin_modifyPrices() {

  delay(500);
  wdt_reset();
  lcd.clear();
  int product_index = 0;
  float temp_prices[5]; // Array auxiliar para modificar precios temporalmente

  // Copiar los precios actuales a la array auxiliar
  for (int i = 0; i < 5; i++) {
    temp_prices[i] = prices[i];
  }

  while (true) {
    lcd.clear();
    lcd.print("Modificar precios");
    lcd.setCursor(0, 1);
    lcd.print(products[product_index]);
    lcd.print(" ");
    lcd.print(temp_prices[product_index]);

    int x_value = analogRead(VRX_PIN); // Valor del eje X
    int y_value = analogRead(VRY_PIN); // Valor del eje Y

    button.loop();
    int b_value = button.getState();

    // Incrementar o decrementar la opción seleccionada según la posición del joystick
    if (y_value < 100) { // Arriba
      temp_prices[product_index] += 0.05; // Incrementar precio en 5 céntimos; // Mover hacia arriba
      delay(500);
    } else if (y_value > 900) { // Abajo
      temp_prices[product_index] -= 0.05; // Decrementar precio en 5 céntimos // Mover hacia abajo
      delay(500);
    } else if (b_value == 0) {
      // Copiar los precios modificados de la array auxiliar a la array original
      for (int i = 0; i < 5; i++) {
        prices[i] = temp_prices[i];
      }
      break; // Salir de la función
    } else if (x_value < 100) { // Izquierda
      return; // Salir de la función
    } else if (x_value > 900) { // Derecha
      product_index = (product_index + 1) % 5; // Avanzar al siguiente producto circularmente
    }
    
    wdt_reset();
  }
  wdt_reset();
}

void buttonISR() {
  if (digitalRead(button_pin) == LOW) {
    button_press_start_time = millis();
    button_pressed = true;
  } else if (button_pressed && digitalRead(button_pin) == HIGH) {
    // El botón acaba de ser liberado, y estaba previamente presionado
    button_press_end_time = millis();
    duration_button_pressed = button_press_end_time - button_press_start_time;
    button_pressed = false;

    // Puedes imprimir la duración si es necesario
    Serial.print("Duración del botón pulsado: ");
    Serial.print(duration_button_pressed);
    Serial.println(" ms");
  }
}