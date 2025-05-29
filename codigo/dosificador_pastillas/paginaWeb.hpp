//-------------------- Página Web ----------------------
#ifdef ARDUINO
#include <pgmspace.h>
#endif

const char html_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Configurar Horarios</title>
</head>
<body>
  <h2>Configura las horas</h2>

  <label>Mañana:</label><br>
  Hora: <select id="hora_m"></select>
  Min: <select id="min_m"></select><br><br>

  <label>Tarde:</label><br>
  Hora: <select id="hora_t"></select>
  Min: <select id="min_t"></select><br><br>

  <label>Noche:</label><br>
  Hora: <select id="hora_n"></select>
  Min: <select id="min_n"></select><br><br>

  <button onclick="enviarTodo()">Enviar configuración</button>

  <p id="respuesta"></p>

  <script>
    function cargarOpciones(id, desde, hasta, paso) {
      const sel = document.getElementById(id);
      for (let i = desde; i <= hasta; i += paso) {
        const opt = document.createElement("option");
        opt.value = i;
        opt.text = i.toString().padStart(2, '0');
        sel.appendChild(opt);
      }
    }

    function enviarTodo() {
      const h_m = document.getElementById("hora_m").value;
      const m_m = document.getElementById("min_m").value;
      const h_t = document.getElementById("hora_t").value;
      const m_t = document.getElementById("min_t").value;
      const h_n = document.getElementById("hora_n").value;
      const m_n = document.getElementById("min_n").value;

      const ts = Math.floor(new Date().getTime() / 1000);
      const url = `/setConfig?hm=${h_m}&mm=${m_m}&ht=${h_t}&mt=${m_t}&hn=${h_n}&mn=${m_n}&ts=${ts}`;

      fetch(url)
        .then(res => res.text())
        .then(data => {
          // Redirigir a la página de confirmación
          window.location.href = "/confirmacion";
        })
        .catch(err => {
          document.getElementById("respuesta").innerText = "Error: " + err;
        });
    }

    // Cargar listas al iniciar
    const tramos = ["m", "t", "n"];
    tramos.forEach(sufijo => {
      cargarOpciones(`hora_${sufijo}`, 0, 23, 1);
      cargarOpciones(`min_${sufijo}`, 0, 55, 5);
    });
  </script>
</body>
</html>
)rawliteral";

const char html_confirm[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>Confirmación</title>
</head>
<body>
  <h2>✅ Configuración enviada correctamente</h2>
  <p>Puedes cerrar esta ventana o desconectarte del dispositivo.</p>
</body>
</html>
)rawliteral";
