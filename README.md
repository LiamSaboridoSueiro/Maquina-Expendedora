# Maquina-Expendedora

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

