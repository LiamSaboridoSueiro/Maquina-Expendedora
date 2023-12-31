# Maquina Expendedora

Este repositorio presenta un proyecto de Controlador de Máquina Expendedora basado en Arduino UNO. El controlador incorpora diversas funcionalidades, desde el arranque del sistema hasta la administración de la máquina, utilizando componentes del kit Arduino.

## Bibliotecas utilizadas

● LiquidCrystal: https://www.arduino.cc/en/Reference/LiquidCrystal

● Watch Dog: https://create.arduino.cc/projecthub/rafitc/what-is-watchdog-timer-fffe20

● DHT-sensor-library: https://github.com/adafruit/DHT-sensor-library

● ezButton: https://www.arduino.cc/reference/en/libraries/ezbutton/

## Resumen de la práctica
Este proyecto implementa un controlador para una máquina expendedora utilizando Arduino UNO y componentes del kit Arduino. El controlador cuenta con las siguientes características:

    Arranque:
        LED1 parpadea 3 veces con el mensaje "CARGANDO..." en el LCD al inicio.
        Después, el sistema entra en la funcionalidad "Servicio".

    Servicio:
        Muestra información si el usuario está cerca; de lo contrario, indica "ESPERANDO CLIENTE".
        Presenta temperatura y humedad por 5 segundos, luego muestra la lista de productos con precios.
        Navegación y selección de productos mediante joystick y switch.
        "Preparando Café..." con duración aleatoria y progreso en LED2.
        Indicación de "RETIRE BEBIDA" al finalizar.
        Reinicio posible presionando el botón durante 2-3 segundos.

    Admin:
        Accesible presionando el botón durante más 5 segundos.
        Vista de administración con ambos LEDs encendidos.
        Menú con opciones para ver temperatura, distancia, contador y modificar precios.
        Modificación de precios con joystick. Confirmación y cancelación con el switch y movimiento "izquierda".
        Salida de la vista admin presionando el botón por 5 segundos.

Para más detalles sobre la implementación y configuración, consulte la documentación detallada en el código fuente.

## Circuito en Fritzing

Diagrama del circuito en Fritzing para una representación visual detallada del montaje y conexión de componentes.

![Circuito en Fritzing](https://github.com/LiamSaboridoSueiro/Maquina-Expendedora/blob/main/Fritzing/circuito.png)

## Video del circuito

Ver el video del circuito en acción para obtener una visión práctica y detallada de cómo opera el controlador de la máquina expendedora.

[![Alt text](https://img.youtube.com/vi/FhtrVg4GZSw/0.jpg)](https://www.youtube.com/watch?v=FhtrVg4GZSw)

## Descripción

He utilizado una máquina de estados para organizar y controlar el flujo de ejecución de la máquina expendedora de manera modular. Esto facilita la gestión de diferentes estados de operación (Arranque, Servicio, Administración) de forma estructurada, simplificando el mantenimiento y la comprensión del código.

Funciones principales:

● setup(): Configuración inicial de pines, sensores, LCD y la interrupción del botón.

● loop(): Bucle principal que gestiona los diferentes estados de la máquina (Arranque, Servicio, Administración).

● start_up(): Estado de arranque que parpadea los LEDs y transita al estado de Servicio.

● service(): Estado de servicio que muestra información en la pantalla LCD y realiza acciones según la interacción del usuario.

● check_distance(): Función para medir la distancia utilizando el sensor de ultrasonido.

● display_products(): Muestra los productos en la pantalla y permite al usuario seleccionar uno.

● read_joystick(): Lee la entrada del joystick y realiza acciones según la dirección o el botón presionado.

● admin(): Estado de administración que permite al usuario realizar diversas acciones administrativas.

● admin_temperature(), admin_distance(), admin_counter(), admin_modifyPrices(): Funciones para las acciones administrativas específicas.

● buttonISR(): Manejador de la interrupción del botón que registra la duración del botón presionado.

El código implementa funciones para la interacción con el usuario, la administración de productos y la visualización de información en la pantalla LCD. Además, utiliza un watchdog timer para evitar bloqueos y garantizar la operación segura del sistema.

## WatchDog

El código utiliza el Watchdog Timer (Temporizador de Vigilancia) de AVR para evitar bloqueos o fallas en el sistema. El Watchdog se configura para un tiempo de espera de 8 segundos, y se reinicia periódicamente durante el arranque y otras operaciones críticas para prevenir reinicios no deseados. Ayuda a mejorar la estabilidad y la resistencia del sistema.

``` c
#include <avr/wdt.h>

void setup() {
  Serial.begin(9600); // Iniciar la comunicación serial
  // Watchdog
  wdt_disable(); // Desactivar el Watchdog durante la configuración

  // Configurar el Watchdog para un tiempo de espera de 8 segundos
  wdt_enable(WDTO_8S);
}

void start_up() {

  wdt_reset(); // Reiniciar el contador del Watchdog
  lcd.clear();
  lcd.print("CARGANDO ...");

  unsigned long start_time = millis();
  int blinkCount = 0;
  int led_state = HIGH;
  int interval = 1000;  // Intervalo de parpadeo en milisegundos

  // ... Resto del código ...

  wdt_reset(); // Reiniciar el contador del Watchdog para evitar reinicios no deseados
  lcd.clear();
  state = 1;
}
```

## Interrupciones

Las interrupciones se utilizan para gestionar eventos externos de forma asíncrona, permitiendo que ciertos fragmentos de código (ISR, Rutinas de Servicio de Interrupción) se ejecuten en respuesta a cambios específicos. En este caso, la interrupción se usa para detectar el estado del botón y medir la duración de su presión.

``` c
// Variables para interrupción
volatile bool button_pressed = false;  // Variable que indica si el botón está presionado
volatile unsigned long button_press_start_time = 0;  // Tiempo de inicio de la presión del botón
volatile unsigned long button_press_end_time = 0;  // Tiempo de finalización de la presión del botón
volatile unsigned long duration_button_pressed = 0;  // Duración total del botón presionado

// Configurar interrupción
attachInterrupt(digitalPinToInterrupt(button_pin), buttonISR, CHANGE);
```

La función buttonISR se ejecuta cuando hay un cambio en el estado del botón, registrando el tiempo de inicio cuando se presiona y calculando la duración cuando se libera.

``` c
// Función de la interrupción del botón
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
```

En la función admin(), la duración del botón presionado se utiliza para reiniciar el estado y volver al servicio si el botón se mantiene presionado durante más de 5 segundos. La interrupción del botón proporciona una forma eficiente de gestionar eventos de usuario en tiempo real.

``` c
// Uso de la interrupción en la función de administración
void admin() {
  // ... Código previo ...

  while (true) {
    // ... Código existente ...

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
```

## Documentación

Consulte la [documentación](https://www.arduino.cc/reference/en/) oficial de Arduino para obtener información detallada sobre las funciones y la programación en Arduino.
