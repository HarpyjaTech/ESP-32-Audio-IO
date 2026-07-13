#ifndef HTML_PAGES_H
#define HTML_PAGES_H

#define VERSAO_LIB 1.0

// ============================================================
// DECLARAÇÃO DE VARIÁVEIS EXTERNAS (Vêm do ficheiro principal)
// ============================================================
extern bool isSetupMode;
extern int currentMode;
extern int connected_apps;
extern int vu_in;
extern int vu_out;
extern WebServer webServer;
extern Preferences preferences;

// ============================================================
// PÁGINAS WEB (HTML)
// ============================================================
const char* html_setup = R"rawliteral(
<!DOCTYPE html><html><head><meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Configuração de Áudio</title>
<style>
  body{font-family:Arial;padding:20px;background:#222;color:#fff;}
  select,input{width:100%;padding:10px;margin:5px 0 15px;border-radius:5px;border:none;}
  button{background:#007BFF;color:#fff;padding:15px;border:none;width:100%;
         font-size:16px;border-radius:5px;cursor:pointer;}
</style></head>
<body><h2>Configurar Servidor de Áudio</h2>
<form action="/save" method="POST">
  <label>Selecione o Wi-Fi Local:</label>
  <select name="ssid" required>
    <option value="">Escaneando redes...</option>
    %OPTIONS%
  </select>
  <label>Senha do Wi-Fi:</label>
  <input type="password" name="pass" placeholder="Deixe em branco se for rede aberta">
  <button type="submit">Salvar e Iniciar Servidor</button>
</form></body></html>
)rawliteral";

const char* html_dashboard = R"rawliteral(
<!DOCTYPE html><html><head><meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Dashboard</title><meta http-equiv="refresh" content="3">
<style>
  body{font-family:Arial;text-align:center;background:#111;color:#0f0;}
  .box{border:1px solid #0f0;padding:20px;margin:10px;border-radius:10px;}
</style></head>
<body><h2>Status - MODO %MODO%</h2>
<div class="box"><h3>IP Servidor: %IP% (Porta 7000)</h3></div>
<div class="box"><h3>Apps Conectados: %CLIENTS% / 4</h3></div>
<div class="box"><h3>Data/Hora: %TIME%</h3></div>
<div class="box"><h3>Line-In (Mesa): %VU_IN% %</h3></div>
<div class="box"><h3>Line-Out (Saida): %VU_OUT% %</h3></div>
<a href="/reset" style="color:red;">Resetar Wi-Fi</a>
</body></html>
)rawliteral";

// ============================================================
// FUNÇÕES DO SERVIDOR WEB
// ============================================================
void handleRoot() {
  if (isSetupMode) {
    String html = html_setup;
    String options = "";
    int n = WiFi.scanNetworks();
    if (n == 0) {
      options = "<option value=''>Nenhuma rede encontrada</option>";
    } else {
      for (int i = 0; i < n; i++) {
        options += "<option value='" + WiFi.SSID(i) + "'>"
                +  WiFi.SSID(i) + " (" + String(WiFi.RSSI(i)) + " dBm)</option>";
      }
    }
    html.replace("%OPTIONS%", options);
    webServer.send(200, "text/html", html);
  } else {
    String html = html_dashboard;

    String modoStr;
    if (currentMode == 0) modoStr = "SERVIDOR";
    else if (currentMode == 1) modoStr = "BLUETOOTH";
    else modoStr = "AUXILIAR";
    html.replace("%MODO%", modoStr);

    html.replace("%IP%",   WiFi.localIP().toString());
    html.replace("%CLIENTS%", String(connected_apps));

    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      char timeStr[32];
      sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      html.replace("%TIME%", String(timeStr));
    } else {
      html.replace("%TIME%", "Sincronizando...");
    }

    html.replace("%VU_IN%",  String(vu_in));
    html.replace("%VU_OUT%", String(vu_out));
    webServer.send(200, "text/html", html);
  }
}

void handleSave() {
  preferences.begin("audio_cfg", false);
  preferences.putString("ssid", webServer.arg("ssid"));
  preferences.putString("pass", webServer.arg("pass"));
  preferences.end();
  webServer.send(200, "text/html", "<h2>Salvo! Reiniciando Servidor...</h2>");
  delay(2000);
  ESP.restart();
}

void handleReset() {
  preferences.begin("audio_cfg", false);
  preferences.clear();
  preferences.end();
  webServer.send(200, "text/html", "<h2>Resetado! Reiniciando...</h2>");
  delay(2000);
  ESP.restart();
}

#endif // HTML_PAGES_H