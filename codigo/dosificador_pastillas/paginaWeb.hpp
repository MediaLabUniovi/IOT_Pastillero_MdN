//-------------------- Página Web ----------------------
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Inicio de Sesión</title>
    <style>
        body {
            font-family: Arial Narrow, Arial, sans-serif;
            margin: 20px;
        }
        #container-lista {
            margin-bottom: 20px;
        }
        #lista-redes {
            list-style-type: "\1f6dc";
            padding: 0;
        }
        #lista-redes li {
            padding: 5px;
            border: 1px solid #ddd;
            margin-bottom: 5px;
            background-color: #f9f9f9;
        }
        input[type="text"], input[type="password"] {
            width: 200px;
            padding: 10px;
            margin-top: 10px;
            border: 1px solid #ccc;
        }
        #vertical-flex {
            display: flex;
            flex-direction: column;
            align-items: left;
            justify-content: center;
            width: 35%;
            padding-top: 1%;
            padding-bottom: 1%;
        }
        #horizontal-flex {
            display: flex;
            flex-direction: row;
            justify-content: left;
            align-items: center;
            width: 45%;
            padding-left: 2%;
            padding-top: 1%;
            padding-bottom: 1%;
        }
    </style>
</head>
<body>
    <h1>Inicio de Sesión</h1>
    <!-- Formulario para añadir elementos a la lista -->
    <div id="container-lista">
        <input type="text" id="nuevaRed" placeholder="Añadir red manualmente" />
        <button onclick="agregarElemento()">Añadir</button>
    </div>
    <h2>Redes Disponibles</h2>
    <div style="width: 95%">
        <ul id="lista-redes"></ul>
    </div>
    <!-- Campo de texto para ingresar la contraseña -->
    <div id="vertical-flex">
        <label for="elementosLista">Selecciona tu red:</label>
        <select id="elementosLista" style="width:52%">
            <option value="" selected disabled></option>
        </select>
        <div id="horizontal-flex">
            <label for="contraseña">Contraseña:</label>
            <div id="vertical-flex">
                <input type="password" id="contraseña" placeholder="Contraseña" autofocus style = "height:50%; min-width: 100px"/>
                <button type="submit" style="min-width: 100px"> Enviar </button>
            </div>
        </div>
    </div>

    <script>
        const CB_TIME_MS = 1000; 
        // Obtener datos del servidor
        async function getData() {
            try {
            // Asegúrate de que el navegador maneje automáticamente las cookies de sesión del Captive Portal
            const respuesta = await fetch('/networks', {
                method: 'GET', // Método GET para obtener los datos
                credentials: 'same-origin' // Asegura que las cookies de sesión se envíen automáticamente
            });
        
            if (!respuesta.ok) {
                throw new Error('Error al obtener los datos desde el servidor');
            }
        
            const datos = await respuesta.json();
        
            // Verificar que los datos son un array de cadenas
            if (Array.isArray(datos) && datos.every(item => typeof item === 'string')) {
                console.log('Datos recibidos:', datos);
                return datos;
            } else {
                throw new Error('Los datos recibidos no son un array de cadenas de texto');
            }
            } catch (error) {
                console.error('Error al obtener los datos:', error);
            }
        }
        // Función para agregar un elemento a la lista visual y al desplegable
        function agregarElemento() {
            const nuevaRed = document.getElementById('nuevaRed').value;
            if (nuevaRed) {
                // Crear el elemento de la lista (ul -> li)
                const ul = document.getElementById('lista-redes');
                const li = document.createElement('li');
                li.textContent = nuevaRed;
                ul.appendChild(li);
                // Crear la opción para el desplegable (select -> option)
                const select = document.getElementById('elementosLista');
                const option = document.createElement('option');
                option.value = nuevaRed;
                option.textContent = nuevaRed;
                select.appendChild(option);
                // Limpiar el campo de texto
                document.getElementById('nuevaRed').value = '';
            } else {
                alert('Por favor, ingresa un elemento.');
            }
        }
        // Función para cargar los datos desde el servidor y mostrar en la lista y el desplegable
        async function cargarDatos() {
            try {
                // Obtener los datos del servidor
                const datos = await getData();
                // Añadir los elementos a la lista y al desplegable
                const ul = document.getElementById('lista-redes');
                const select = document.getElementById('elementosLista');

                datos.forEach(item => {
                    // Añadir a la lista visual
                    const li = document.createElement('li');
                    li.textContent = item;
                    ul.appendChild(li);
                    // Añadir al desplegable
                    const option = document.createElement('option');
                    option.value = item;
                    option.textContent = item;
                    select.appendChild(option);
                });
            } catch (error) {
                console.error('Error al cargar los datos:', error);
            }
        }


        // Función para ejecutar en intervalos
        function ejecutarEnIntervalo() {
        setInterval(() => {
            getData().then(cadenas => {
            // Aquí puedes manejar los datos recibidos en cada intervalo
                console.log(cadenas);
            });
        }, CB_TIME_MS);
        }

        // Llamar a la función para ejecutar el intervalo
        window.onload = ejecutarEnIntervalo;
        window.onload = cargarDatos;
    </script>
</body>
</html>
)rawliteral";