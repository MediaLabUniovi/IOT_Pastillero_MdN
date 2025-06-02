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
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f0f4f8;
      margin: 0;
      padding: 0;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      min-height: 100vh;
    }

    .bloque {
      background-color: #fff;
      padding: 20px 30px;
      border-radius: 12px;
      box-shadow: 0 4px 12px rgba(0, 0, 0, 0.1);
      width: 90%;
      max-width: 400px;
      box-sizing: border-box;
    }

    h2 {
      color: #333;
      margin-bottom: 20px;
      text-align: center;
    }

    label {
      display: block;
      margin-top: 15px;
      font-weight: bold;
    }

    .fila {
      display: flex;
      gap: 10px;
      margin-bottom: 10px;
    }

    .fila select {
      flex: 1;
      padding: 6px;
      font-size: 1em;
      border-radius: 6px;
      border: 1px solid #ccc;
    }

    button {
      margin-top: 25px;
      width: 100%;
      padding: 12px;
      font-size: 1.1em;
      background-color: #007bff;
      color: white;
      border: none;
      border-radius: 8px;
      cursor: pointer;
      transition: background-color 0.3s ease;
    }

    button:hover {
      background-color: #0056b3;
    }

    #respuesta {
      margin-top: 15px;
      color: red;
      text-align: center;
    }

    /* Responsivo */
    @media (max-width: 480px) {
      .fila {
        flex-direction: column;
      }

      .fila select {
        width: 100%;
      }
    }
  </style>
</head>
<body>
  <div class="bloque">
    <h2>Configura las horas</h2>

    <label>Mañana:</label>
    <div class="fila">
      <select id="hora_m"></select>
      <select id="min_m"></select>
    </div>

    <label>Tarde:</label>
    <div class="fila">
      <select id="hora_t"></select>
      <select id="min_t"></select>
    </div>

    <label>Noche:</label>
    <div class="fila">
      <select id="hora_n"></select>
      <select id="min_n"></select>
    </div>

    <button onclick="enviarTodo()">Enviar configuración</button>

    <p id="respuesta"></p>
  </div>

  <script>
    function cargarOpciones(id, desde, hasta, paso) {
      const sel = document.getElementById(id);
      const opcionVacia = document.createElement("option");
      opcionVacia.value = "";
      opcionVacia.text = "--";
      sel.appendChild(opcionVacia);

      for (let i = desde; i <= hasta; i += paso) {
        const opt = document.createElement("option");
        opt.value = i;
        opt.text = i.toString().padStart(2, '0');
        sel.appendChild(opt);
      }
    }

    function obtenerValor(id) {
      const val = document.getElementById(id).value;
      return val === "" ? -1 : parseInt(val);
    }

    function enviarTodo() {
      const h_m = obtenerValor("hora_m");
      const m_m = obtenerValor("min_m");
      const h_t = obtenerValor("hora_t");
      const m_t = obtenerValor("min_t");
      const h_n = obtenerValor("hora_n");
      const m_n = obtenerValor("min_n");

      const ts = Math.floor(new Date().getTime() / 1000);
      const url = `/setConfig?hm=${h_m}&mm=${m_m}&ht=${h_t}&mt=${m_t}&hn=${h_n}&mn=${m_n}&ts=${ts}`;

      fetch(url)
        .then(res => res.text())
        .then(data => {
          window.location.href = "/confirmacion";
        })
        .catch(err => {
          document.getElementById("respuesta").innerText = "Error: " + err;
        });
    }

    const tramos = ["m", "t", "n"];
    tramos.forEach(sufijo => {
      cargarOpciones(`hora_${sufijo}`, 0, 23, 1);
      cargarOpciones(`min_${sufijo}`, 0, 55, 5);
    });
  </script>
</body>
</html>
)rawliteral";