# Código dosificador de pastillas

Esta versión del dosificador incluye el modo configuración a traves de WiFi y el modo de bajo consumo de 50 mA.

## Guía de uso

En caso de que sea el primer arranque del micro o este detecte que todavía no hay ninguna configuración, entraremos en el modo configuración.

El modo configuración también será accesible manteniendo el botón pulsado durante 3 segundos.
Una secuencia de colores nos indicará que hemos entrado en este modo.
Una vez activado deberíamos buscar entre las redes WiFi la de nuestro dosificador, preferiblemente han de ser llamadas DosificadorPastillasX, sustituyendo X por el indicador que se considere necesario. 

Al conectarnos a esa WiFi desde nuestro dispositivo, nos saldrá una imagen como esta:

<div align="center">
  <img src="https://github.com/user-attachments/assets/5653b087-0cc3-454f-aab4-22147bc487ac" alt="modoConfig" width="500">
</div>


Y después de seleccionar las horas, un mensaje de confirmación:

<div align="center">
  <img src="https://github.com/user-attachments/assets/61aceff4-9fdc-468e-9346-d426f29599e5" alt="confirmacionConfig" width="500">
</div>


Al finalizar podremos cerrar la página y el WiFi se apagará automáticamente. El micro se dormirá y despertará en el momento que deba sonar la alarma.

## Modificaciones de código

<div align="center">
  <img src="https://github.com/user-attachments/assets/740b35e8-f64d-4015-9c06-9d9fe85d653f" alt="credenciales" width="500">
</div>

Para modificar el código hay que ir a la pestaña configuration.h y ahí cambiar estas dos variables con el nombre y contraseña deseadas.

## Otras características
El sistema está pensado de forma que si se pulsa el botón sin querer el micro se vuelva a dormir en un minuto, y si se entra en modo configuración, el micro vuelva a dormirse después de 10 minutos.
