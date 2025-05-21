void WiFi_setup(){
  //WiFi.mode(WIFI_AP_STA);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_NAME);

  Serial.println("Iniciando AP...");
  Serial.println("IP: " + WiFi.softAPIP().toString());

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.setTTL(300);
  if (dnsServer.start(53, "*", WiFi.softAPIP())){
    Serial.println("Inciado servidor DNS en modo portal cautivo");
  } else {
    Serial.println("Error: No se pudo iniciar el servidor DNS");
  }
}

void obtener_claves(int count){
  while (addedSSID <= count){ //Lee tantas claves como valga counter SI ya registró una, no la vuelve a registrar
    String ssidKey = "ssid" + String(addedSSID);
    String passKey = "pass" + String(addedSSID);
    //Serial.println(ssidKey);
    //Serial.println(passKey);
    String ssid = pref.getString(ssidKey.c_str(), "");
    String password = pref.getString(passKey.c_str(), "");
    if (ssid.length() > 0) { // Evito añadir varias veces la misma clave
      wifimulti.addAP(ssid.c_str(), password.c_str());
      Serial.println("Añadida ssid: " + ssid);
      addedSSID++;
    }
    delay(50);
    }
}

String getAvailableNetworks(){
    String json = "[";
    Serial.println("** Scan Networks **");
    int numSSID = WiFi.scanNetworks();
    //Serial.print("Found ");
    //Serial.print(numSSID);
    //Serial.println(" Networks. ");
    if (numSSID == -1) {
        json += "\"Error scanning networks\"";
        Serial.println("Error scanning networks");
        return json + "]";
    }
    if (numSSID > MAX_NETWORKS) {

        Serial.print("Showing first ");
        Serial.print(MAX_NETWORKS);
        Serial.println(" networks found");
        numSSID = MAX_NETWORKS;
    }
    else {   // print the list of networks seen:
        Serial.print("Number of available networks: ");
        Serial.println(numSSID);
    }
    for (int i = 0; i < numSSID; i++) { 
        if (i > 0) json += ",";  
        //json += "\"" + WiFi.SSID(i) + "  (" + WiFi.RSSI(i) + " dBm)\"";
        json += "\"" + WiFi.SSID(i) + "\"";
        //Serial.print(i + 1);
        //Serial.print(") ");
        //Serial.print(WiFi.SSID(i));
        //Serial.print("\tSignal: ");
        //Serial.print(WiFi.RSSI(i));
        //Serial.print(" dBm\n");
    }
        //Serial.println("");
        //Serial.print("JSON Output: ");
        //Serial.println(json + "]");
    return json + "]";
}
// index page handler
void handleRoot() {
  server.send(200, "text/plain", "Hello from esp32!");
}

void handleNotFound() {
  Serial.print("Petición no encontrada: ");
  Serial.println(server.uri());  // Muestra qué ruta la causó

  server.sendHeader("Location", "/portal");
  server.send(302, "text/plain", "redirect to captive portal");
}

void server_config(){
  // Server 
  // serve a simple root page
  server.on("/", handleRoot);

  // serve portal page
  server.on("/portal", []() {
    server.send(200, "text/html", index_html);
  });

  // Maneja el formulario de las redes WiFi
  server.on("/networks", HTTP_GET, []() {
    String networksJson = getAvailableNetworks();
    server.send(200, "application/json", networksJson);
  });

  // get WiFI credentials
  server.on("/guardar", HTTP_POST, []() {
    if (server.hasArg("ssid") && server.hasArg("password")) {
      // Extraemos las claves del portal
      String ssid = server.arg("ssid");
      String password = server.arg("password");
      Serial.println("SSID: " + ssid);
      Serial.println("Password: " + password);
      // Cogemos el counter de cuantas redes WiFi tenemos ya almacenadas
      int counter = pref.getInt("counter", 0);
      counter++;
      Serial.print("Número de redes en memoria no volátil: " );
      Serial.println(counter);
      pref.putInt("counter", counter);
      // Creamos las nuevas keys con el valor de counter
      String ssidKey = "ssid" + String(counter);
      String passKey = "pass" + String(counter);
      //Serial.println(ssidKey);
      //Serial.println(passKey);
      // Guardar el ssid y el password utilizando las claves correspondientes
      Serial.println("Guardando claves en memoria");
      pref.putString(ssidKey.c_str(), ssid); // key, value
      pref.putString(passKey.c_str(), password);
      wifimulti.addAP(ssid.c_str(), password.c_str());
      Serial.println("Añadida WiFi");
      server.send(200, "text/plain", "Credenciales recibidas.");
    } else {
      server.send(400, "text/plain", "Faltan parámetros");
    }
  });

  server.on("/descargar", HTTP_GET, [](){
        
    File file = SD.open("/I4life/datos.json", FILE_READ);
    if (!file){
      server.send(500, "text/plain", "Error: No se pudo abrir el archivo");
      Serial.println("Error abriendo datos.json");
      return;
    }
    
    server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    // Cabecera para forzar la descarga
    server.sendHeader("Content-Type", "application/json");
    server.sendHeader("Content-Disposition", "attachment; filename=\"datos.json\"");
    server.send(200, "application/json", "");  // <-- lanza el chunked
    //server.streamFile(file, "application/json");

    uint8_t buf[128];
    while(file.available()){
      size_t n = file.read(buf, sizeof(buf));
      server.client().write(buf, n);
    }
    file.close();
    server.sendContent("");  // flush final
    Serial.println("Archivo enviado correctamente");
  });

  server.on("/favicon.ico", HTTP_GET, []() {
    server.send(204);  // No Content
  });

  server.on("/favicon.ico", HTTP_GET, []() {
    server.send(204);  // No Content
  });

  // all unknown pages are redirected to captive portal
  server.onNotFound(handleNotFound);
  server.begin();
}