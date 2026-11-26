/*
  ============================================================
   MIND SEARCH (antes Foto Mágica)  —  Mago Jesús
  ============================================================
   En TU celular (emisor) tenés una app de "Notas" que parece
   real. Escribís una palabra (Francia, perro, rojo, Ferrari...)
   y apretás ENTER: en el celular del ESPECTADOR se abre, según el
   modo elegido en los ajustes secretos:
     - Google:    escribe solo la palabra y la busca (en el buscador queda la palabra sola)
     - Google:    la búsqueda normal de Google
     - Wikipedia: directo el artículo de Wikipedia
     - Solo texto: escribe la palabra donde esté el cursor
   Cuando la foto ya debería estar
   cargada, tu nota muestra "Guardado ✓" (y vibra en Android).

   EMISOR (tu celu):
     - WiFi "TP-Link_6758" (clave 12345678). Si dice "imposible
       conectar": olvidá la red y probá de nuevo; mirá el Monitor
       Serie (115200) para ver si la placa se reinicia. Ya no abre la ventanita
       de "red cautiva": el celu cree que tiene internet y queda
       conectado. (En Android, si igual pregunta, elegí seguir
       conectado.)
     - Abrí Chrome/Safari en  192.168.4.1
     - Agregalo a la pantalla de inicio:
         Android: menú ⋮ > "Agregar a la pantalla principal"
         iPhone:  Compartir > "Agregar a inicio"
       Queda el ícono "Notas".
     - AJUSTES SECRETOS: tocá 3 veces seguidas la palabra "Notas"
       de arriba.

   MANDAR: escribís la palabra y apretás ENTER, o tocás "OK" arriba
   a la derecha (manda la línea donde está el cursor).

   LAPICERA (opcional): el espectador escribe con la lapicera NeoLAB
   (la de Vision). Cuando deja de escribir, la ESP32 reconoce la palabra
   con Google y la manda sola: vos no tocás ningún celular.
   Necesita internet: en los ajustes secretos poné el WiFi del lugar o
   el hotspot de tu celu (que queda en el bolsillo).

   BOTÓN: desde la app "cargás" un efecto (una palabra, subir volumen,
   sacar foto...) y lo disparás cuando quieras apretando el botón BOOT
   de la placa o un pulsador entre GPIO 4 y GND.

   EFECTOS (se eligen en la app):
     Google, Fotos, Cumpleaños famoso, YouTube, Mapa, Mensaje,
     Calculadora, Solo texto, Telequinesis (volumen / play) y Foto fantasma.

   ESPECTADOR (receptor):
     - Ajustes > Bluetooth > "Mind Search" > Vincular (sin código).
     - Chrome abierto (Android) o Safari (iPhone).
     - Al terminar: Ajustes secretos > "Nuevo espectador".

   ARCHIVOS: foto_magica.ino + icono.h + lapicera_ms.h + pagina.h (en la misma carpeta)
   OJO: usa la librería HijelHID_BLEKeyboard con un cambio [Mind Search]
   (excludeHandle) para que el celu de la app no reciba las teclas.
   LIBRERÍAS: HijelHID_BLEKeyboard 0.6.0, NimBLE-Arduino 2.3.8+
   PLACA: arduino-esp32 3.3.7 o más.
          ESP32-C3: "ESP32C3 Dev Module", USB CDC On Boot: Enabled.
          ESP32 común: "ESP32 Dev Module".
          En las dos: Partition Scheme "Huge APP (3MB No OTA)".
  ============================================================
*/

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <HijelHID_BLEKeyboard.h>
#include "icono.h"
#include "pagina.h"

Preferences prefs;
#include "lapicera_ms.h"

// ---------------- AJUSTES ----------------
const char *WIFI_NOMBRE = "TP-Link_6758";
const char *WIFI_CLAVE  = "12345678";        // mínimo 8 caracteres
const char *NOMBRE_BT   = "Mind Search";     // cómo aparece en el Bluetooth del espectador

// Direcciones que escribe en el celu del espectador (sin "https://", Chrome lo completa)
// Google: escribe SOLO la palabra en la barra y Enter. El navegador la busca en Google
// y en el buscador queda únicamente la palabra (como si la hubiera buscado el espectador).

HijelHID_BLEKeyboard teclado(NOMBRE_BT, "Generic", 100);
WebServer servidor(80);
DNSServer dns;

// ---------------- ESTADO ----------------
String   pendienteTexto = "";
String   pendienteModo  = "web";   // web | img | cum (navegador)  ent (escribe + Enter)  txt  calc  key
bool     pendienteIphone = false;
bool     pendienteAbrirNav = true;
uint8_t  pendienteBarra = 1;       // Android: 1 Ctrl+L, 2 Alt+D, 3 tecla Buscar
uint32_t pendienteCargaMs = 2500;
bool     hayPendiente = false;
bool     pedirNuevo = false;

uint32_t envioId = 0;         // número del último envío
uint8_t  fase = 0;            // 0 nada, 1 escribiendo, 2 cargando, 3 listo
uint32_t listoEn = 0;         // millis() en que se considera cargada la foto

// Ajustes guardados en la ESP32 (los usa la lapicera, que no pasa por la app)
String   gModo = "web";
bool     gIphone = false, gAbrirNav = true;
uint8_t  gBarra = 1;
uint32_t gCargaMs = 2500;
String   wifiSsid = "", wifiClave = "";   // WiFi con internet (para la lapicera)
uint32_t staBuscarHasta = 0;              // mientras busca el WiFi con internet (después deja de buscar)
bool     staBuscando = false;

// Declaraciones (el IDE no las genera solo en este sketch)
void guardarAjustes(const String &m, bool ip, bool nav, uint8_t bar, uint32_t carga);
String soloAscii(const String &s);
String jsonTexto(const String &s);
void empezarInternet(uint32_t ms);
void irABarra(bool iphone, uint8_t metodo);
void escribirEnReceptor(const String &texto, const String &modo, bool iphone, bool abrirNav, uint8_t metodoBarra);
bool modoValido(const String &m);
bool modoNavegador(const String &m);
String soloCalc(const String &s);
void hacerTeclasEspeciales(const String &orden);

// ---------------- PÁGINA "NOTAS" ----------------




// ---------------- RESPUESTAS WEB ----------------
String soloAscii(const String &s) {
  String r;
  for (size_t i = 0; i < s.length() && r.length() < 60; i++) {
    char c = s[i];
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') ||
        c == ' ' || c == '.' || c == ',' || c == '-' || c == '\'') r += c;
  }
  r.trim();
  return r;
}

// Calculadora: solo números y + - * / =
String soloCalc(const String &s) {
  String r;
  for (size_t i = 0; i < s.length() && r.length() < 40; i++) {
    char c = s[i];
    if (c == 'x' || c == 'X') c = '*';
    if ((c >= '0' && c <= '9') || c == '+' || c == '-' || c == '*' || c == '/' || c == '=') r += c;
  }
  return r;
}
bool modoValido(const String &m) {
  return m == "web" || m == "img" || m == "cum" || m == "ent" || m == "txt" || m == "calc";
}
bool modoNavegador(const String &m) { return m == "web" || m == "img" || m == "cum"; }

void paginaInicio()  { servidor.send_P(200, "text/html", PAGINA); }
void paginaManifest(){ servidor.send_P(200, "application/manifest+json", MANIFEST); }
void paginaIcono() {
  servidor.sendHeader("Cache-Control", "no-cache", false);
  servidor.send_P(200, "image/png", (const char *)ICONO_PNG, ICONO_LEN);
}

void paginaEnviar() {
  String t = soloAscii(servidor.arg("t"));
  if (t.length() == 0) { servidor.send(200, "application/json", "{\"ok\":0,\"r\":\"vacio\"}"); return; }
  if (!teclado.isPaired()) { servidor.send(200, "application/json", "{\"ok\":0,\"r\":\"bt\"}"); return; }
  if (hayPendiente) { servidor.send(200, "application/json", "{\"ok\":0,\"r\":\"ocupado\"}"); return; }
  pendienteTexto  = t;
  pendienteModo   = modoValido(servidor.arg("m")) ? servidor.arg("m") : "web";
  pendienteIphone = servidor.arg("i") == "1";
  pendienteAbrirNav = servidor.arg("n") != "0";
  long b = servidor.arg("b").toInt();
  pendienteBarra = (b >= 1 && b <= 3) ? (uint8_t)b : 1;
  long w = servidor.arg("w").toInt();
  pendienteCargaMs = (w >= 500 && w <= 10000) ? (uint32_t)w : 2500;
  guardarAjustes(pendienteModo, pendienteIphone, pendienteAbrirNav, pendienteBarra, pendienteCargaMs);
  envioId++;
  fase = 1;
  hayPendiente = true;
  servidor.send(200, "application/json", "{\"ok\":1,\"id\":" + String(envioId) + "}");
}

void guardarAjustes(const String &m, bool ip, bool nav, uint8_t bar, uint32_t carga) {
  gModo = modoValido(m) ? m : "web";
  gIphone = ip; gAbrirNav = nav; gBarra = bar; gCargaMs = carga;
  prefs.putString("m", gModo); prefs.putBool("i", gIphone); prefs.putBool("n", gAbrirNav);
  prefs.putUChar("b", gBarra); prefs.putUInt("w", gCargaMs);
}

// /ajustes?m=&i=&n=&b=&w=  (la app avisa cada cambio, así la lapicera usa lo mismo)
void paginaAjustes() {
  long b = servidor.arg("b").toInt(), w = servidor.arg("w").toInt();
  guardarAjustes(servidor.arg("m"), servidor.arg("i") == "1", servidor.arg("n") != "0",
                 (b >= 1 && b <= 3) ? (uint8_t)b : 1, (w >= 500 && w <= 10000) ? (uint32_t)w : 2500);
  servidor.send(200, "text/plain", "ok");
}

// /lapicera?on=1|0  &pau=ms  &pw=clave  &olvidar=1
void paginaLapicera() {
  if (servidor.hasArg("on"))  {
    penActivar(servidor.arg("on") == "1");
    if (penEnabled) empezarInternet(20000);
    else { staBuscando = false; WiFi.disconnect(false); }
  }
  if (servidor.hasArg("pau")) { long p = servidor.arg("pau").toInt(); if (p >= 800 && p <= 10000) { penPausaMs = p; prefs.putUInt("pau", p); } }
  if (servidor.hasArg("pw"))  { penPw = servidor.arg("pw"); prefs.putString("ppw", penPw); penKick = true; }
  if (servidor.hasArg("olvidar")) penOlvidar();
  if (servidor.hasArg("mandar")) { penUltima = soloAscii(servidor.arg("mandar")); mandarPalabraLapicera(penUltima); }
  servidor.send(200, "text/plain", "ok");
}

// Busca el WiFi con internet un rato y después deja de buscar,
// así la red propia (la de la app Notas) no se corta mientras tanto.
void empezarInternet(uint32_t ms) {
  if (!wifiSsid.length() || !penEnabled) return;
  if (WiFi.status() == WL_CONNECTED) return;
  WiFi.begin(wifiSsid.c_str(), wifiClave.c_str());
  staBuscando = true;
  staBuscarHasta = millis() + ms;
}
void revisarInternet() {
  if (staBuscando && WiFi.status() == WL_CONNECTED) staBuscando = false;
  if (staBuscando && (int32_t)(millis() - staBuscarHasta) >= 0) {
    staBuscando = false;
    WiFi.disconnect(false);                  // deja de buscar: la red propia queda quieta
  }
}
// La lapicera lo llama justo antes de reconocer
bool asegurarInternet(uint32_t esperaMs) {
  if (WiFi.status() == WL_CONNECTED) return true;
  if (!wifiSsid.length()) return false;
  empezarInternet(esperaMs);
  uint32_t t = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t < esperaMs) {
    dns.processNextRequest(); servidor.handleClient(); delay(20);
  }
  return WiFi.status() == WL_CONNECTED;
}

// /wifi?s=nombre&p=clave  (WiFi con internet para reconocer la letra)
void paginaWifi() {
  wifiSsid = servidor.arg("s"); wifiClave = servidor.arg("p");
  prefs.putString("ss", wifiSsid); prefs.putString("sp", wifiClave);
  servidor.send(200, "text/plain", "ok");
  delay(100);
  WiFi.disconnect(false);
  staBuscando = false;
  empezarInternet(20000);
}

// La lapicera reconoció una palabra: se manda igual que desde la app
void mandarPalabraLapicera(const String &palabra) {
  String t = soloAscii(palabra);
  if (!t.length() || !teclado.isPaired() || hayPendiente) return;
  pendienteTexto = t; pendienteModo = gModo; pendienteIphone = gIphone;
  pendienteAbrirNav = gAbrirNav; pendienteBarra = gBarra; pendienteCargaMs = gCargaMs;
  envioId++;
  fase = 1;
  hayPendiente = true;
}

String jsonTexto(const String &s) {
  String r = "\"";
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '"' || c == '\\') { r += '\\'; r += c; }
    else if ((uint8_t)c >= 32) r += c;
  }
  return r + "\"";
}

void paginaEstado() {
  if (fase == 2 && (int32_t)(millis() - listoEn) >= 0) fase = 3;
  String j = "{\"c\":" + String(teclado.isPaired() ? 1 : 0) +
             ",\"f\":" + String(fase) +
             ",\"id\":" + String(envioId) +
             ",\"pe\":" + String(penEnabled ? 1 : 0) +
             ",\"p\":" + String(penState) +
             ",\"pb\":" + String(penBat) +
             ",\"pau\":" + String(penPausaMs) +
             ",\"in\":" + String(WiFi.status() == WL_CONNECTED ? 1 : 0) +
             ",\"sb\":" + String(staBuscando ? 1 : 0) +
             ",\"ss\":" + jsonTexto(wifiSsid) +
             ",\"lw\":" + jsonTexto(penUltima) +
             ",\"lo\":" + jsonTexto(penOpciones) +
             ",\"er\":" + jsonTexto(penErr) + "}";
  servidor.send(200, "application/json", j);
}

void paginaNuevo() {
  pedirNuevo = true;
  servidor.send(200, "text/plain", "ok");
}

// ---- Que los celulares crean que HAY internet (sin ventanita de red cautiva) ----
void respApple()   { servidor.send(200, "text/html", "<HTML><HEAD><TITLE>Success</TITLE></HEAD><BODY>Success</BODY></HTML>"); }
void resp204()     { servidor.send(204, "text/plain", ""); }
void respWindows() { servidor.send(200, "text/plain", "Microsoft Connect Test"); }
void respNcsi()    { servidor.send(200, "text/plain", "Microsoft NCSI"); }
void respFirefox() { servidor.send(200, "text/plain", "success\n"); }

// Cualquier otra dirección: respuesta vacía (no abre nada raro)
void paginaOtra() {
  String host = servidor.hostHeader();
  if (host.indexOf("apple") >= 0 || host.indexOf("ibook") >= 0 || host.indexOf("itools") >= 0) { respApple(); return; }
  resp204();
}

// ---------------- ESCRIBIR EN EL ESPECTADOR ----------------
#define TECLA_NAVEGADOR ((uint16_t)0x0196)   // tecla "Navegador" (AL Internet Browser)

#define TECLA_BUSCAR    ((uint16_t)0x0221)   // tecla "Buscar" (AC Search)

void irABarra(bool iphone, uint8_t metodo) {
  if (iphone)            teclado.tap(KEY_L, KEY_MOD_LGUI);                 // Cmd+L
  else if (metodo == 2)  teclado.tap(teclado.keycodeFor('d'), KEY_MOD_LALT); // Alt+D
  else if (metodo == 3)  teclado.tap(TECLA_BUSCAR);                        // tecla Buscar
  else                   teclado.tap(KEY_L, KEY_MOD_LCTRL);                // Ctrl+L
}

// Teclas que no dependen del idioma del celular (números de arriba y el bloque numérico)
#define KP_DIV   0x54
#define KP_MUL   0x55
#define KP_MENOS 0x56
#define KP_MAS   0x57
#define KP_ENTER 0x58

void escribirCalculadora(const String &t) {
  for (size_t i = 0; i < t.length(); i++) {
    char c = t[i];
    if (c >= '0' && c <= '9') teclado.print(String(c));
    else if (c == '+') teclado.tap((uint8_t)KP_MAS);
    else if (c == '-') teclado.tap((uint8_t)KP_MENOS);
    else if (c == '*') teclado.tap((uint8_t)KP_MUL);
    else if (c == '/') teclado.tap((uint8_t)KP_DIV);
    else if (c == '=') teclado.tap((uint8_t)KP_ENTER);
    delay(60);
  }
}

void escribirEnReceptor(const String &texto, const String &modo, bool iphone, bool abrirNav, uint8_t metodoBarra) {
  uint8_t mod = iphone ? KEY_MOD_LGUI : KEY_MOD_LCTRL;   // Cmd en iPhone, Ctrl en Android
  bool nav = modoNavegador(modo);

  teclado.tap(KEY_LSHIFT);           // despierta la pantalla si estaba apagada
  delay(300);

  if (modo == "calc") { escribirCalculadora(texto); return; }

  // Abrir el navegador solo (Android): tecla "Navegador" + Win+B
  if (nav && abrirNav && !iphone) {
    teclado.tap(TECLA_NAVEGADOR);
    delay(400);
    teclado.tap(teclado.keycodeFor('b'), KEY_MOD_LGUI);
    delay(1500);                     // tiempo para que aparezca el navegador
  }

  if (nav) {
    irABarra(iphone, metodoBarra);   // ir a la barra de direcciones
    delay(700);
    teclado.tap(teclado.keycodeFor('a'), mod);  // seleccionar lo que había
    delay(400);
    teclado.print(" ");              // tecla de relleno: si se pierde, no importa
    delay(150);                      // (Chrome ignora el espacio del principio)
  }

  if (modo == "img") teclado.print("imagenes de ");          // Fotos: solo se ve en el espectador
  if (modo == "cum") teclado.print("famosos nacidos el ");   // Cumpleaños famoso
  teclado.print(texto);

  if (nav || modo == "ent") {        // ent: YouTube, Mapa, Mensaje (busca / envía)
    delay(200);
    teclado.tap(KEY_RETURN);
  }
}

// Volumen, play, silencio y foto:  orden = "accion|veces|pausaMs"
#define MEDIA_SUBIR   ((uint16_t)0x00E9)
#define MEDIA_BAJAR   ((uint16_t)0x00EA)
#define MEDIA_MUTE    ((uint16_t)0x00E2)
#define MEDIA_PLAY    ((uint16_t)0x00CD)
#define MEDIA_SIG     ((uint16_t)0x00B5)
#define MEDIA_ANT     ((uint16_t)0x00B6)
void hacerTeclasEspeciales(const String &orden) {
  int a = orden.indexOf('|'), b = orden.indexOf('|', a + 1);
  String acc = a < 0 ? orden : orden.substring(0, a);
  long veces = a < 0 ? 1 : orden.substring(a + 1, b < 0 ? orden.length() : b).toInt();
  long pausa = b < 0 ? 250 : orden.substring(b + 1).toInt();
  if (veces < 1) veces = 1;
  if (veces > 30) veces = 30;
  if (pausa < 60) pausa = 60;
  if (pausa > 3000) pausa = 3000;
  uint16_t u = 0;
  if (acc == "VU" || acc == "FO") u = MEDIA_SUBIR;   // FO: en la cámara, subir volumen saca la foto
  else if (acc == "VD") u = MEDIA_BAJAR;
  else if (acc == "MU") u = MEDIA_MUTE;
  else if (acc == "PP") u = MEDIA_PLAY;
  else if (acc == "NX") u = MEDIA_SIG;
  else if (acc == "PV") u = MEDIA_ANT;
  if (!u) return;
  for (long i = 0; i < veces; i++) {
    teclado.tap(u);
    if (i + 1 < veces) delay(pausa);
  }
}


// ---------------- APP MIND SEARCH POR BLUETOOTH ----------------
// La app (Android / iPhone) se conecta a la placa por Bluetooth, al mismo
// tiempo que el celu del espectador. Así tu celu no usa el WiFi de la placa.
//   Comando (texto):  E|modo|iphone|abrirNav|barra|cargaMs|palabra   -> escribir
//                     K|accion|veces|pausaMs                          -> volumen / play / foto
//                     N                                               -> nuevo espectador
//   Estado (notifica): {"c":espectador,"f":fase,"id":envio,"n":comandos,"e":"error"}
#define APP_SVC "6d696e64-7365-6172-6368-000000000001"
#define APP_CMD "6d696e64-7365-6172-6368-000000000002"
#define APP_EST "6d696e64-7365-6172-6368-000000000003"
NimBLECharacteristic *appEst = nullptr;
String   appCmd = "";
volatile bool appHay = false;
String   appErr = "", appUltEst = "";
uint32_t appN = 0;                // comandos recibidos (para que la app sepa que llegó)
uint32_t appTEst = 0, appTAdv = 0;

// El celu que usa la app nunca debe recibir las teclas (eso es para el espectador)
class AppCmdCB : public NimBLECharacteristicCallbacks {
  void onWrite(NimBLECharacteristic *c, NimBLEConnInfo &ci) override {
    teclado.excludeHandle(ci.getConnHandle());
    if (!appHay) { appCmd = String(c->getValue().c_str()); appHay = true; }
  }
  void onSubscribe(NimBLECharacteristic *c, NimBLEConnInfo &ci, uint16_t v) override {
    teclado.excludeHandle(ci.getConnHandle());
  }
  void onRead(NimBLECharacteristic *c, NimBLEConnInfo &ci) override {
    teclado.excludeHandle(ci.getConnHandle());
  }
} appCmdCB;

void appSetup() {
  NimBLEServer *srv = NimBLEDevice::getServer();
  if (!srv) return;
  if (srv->getServiceByUUID(APP_SVC)) return;
  // Para sumar el servicio no puede haber nadie conectado: si el celu del
  // espectador ya se reconectó solo, se lo corta un instante (vuelve solo).
  NimBLEDevice::stopAdvertising();
  for (int k = 0; k < 30 && srv->getConnectedCount() > 0; k++) {
    for (uint16_t h : srv->getPeerDevices()) srv->disconnect(h);
    delay(100);
    NimBLEDevice::stopAdvertising();
  }
  NimBLEDevice::stopAdvertising();
  NimBLEService *sv = srv->createService(APP_SVC);
  NimBLECharacteristic *cmd = sv->createCharacteristic(APP_CMD, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR);
  cmd->setCallbacks(&appCmdCB);
  appEst = sv->createCharacteristic(APP_EST, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY);
  appEst->setValue("{}");
  appEst->setCallbacks(&appCmdCB);
  bool ok = srv->start();
  NimBLEDevice::startAdvertising();
  Serial.printf("App Mind Search: servicio Bluetooth %s\n", ok && sv->isStarted() ? "listo" : "NO quedo registrado");
}

// Procesa lo que mandó la app
// ---- BOTÓN DEL DISPOSITIVO: dispara lo que dejaste "cargado" desde la app ----
// Sirve el botón BOOT de la placa (GPIO 9) o un pulsador externo entre GPIO 4 y GND.
#define BOTON_BOOT 9
#define BOTON_EXT  4
String   armado = "";              // comando guardado (E|... o K|...)
bool     botonAntes = false;
uint32_t botonT = 0;
void procesarComando(const String &c);

void botonSetup() {
  pinMode(BOTON_BOOT, INPUT_PULLUP);
  pinMode(BOTON_EXT, INPUT_PULLUP);
  armado = prefs.getString("arm", "");
}
void botonLoop() {
  bool ahora = digitalRead(BOTON_BOOT) == LOW || digitalRead(BOTON_EXT) == LOW;
  if (ahora != botonAntes && millis() - botonT > 40) {
    botonT = millis();
    botonAntes = ahora;
    if (ahora && armado.length()) {
      Serial.printf("Boton: %s\n", armado.c_str());
      procesarComando(armado);
    }
  }
}

void appProcesar() {
  if (!appHay) return;
  String c = appCmd; appHay = false;
  appN++;
  procesarComando(c);
}

void procesarComando(const String &c) {
  if (c == "N") { pedirNuevo = true; return; }
  if (c.startsWith("A|")) {                     // cargar el botón (A| vacío = descargar)
    armado = c.substring(2);
    prefs.putString("arm", armado);
    appErr = "";
    return;
  }
  if (c.startsWith("K|")) {
    if (!teclado.isPaired()) { appErr = "bt"; return; }
    if (hayPendiente) { appErr = "ocupado"; return; }
    appErr = "";
    pendienteTexto = c.substring(2);
    pendienteModo = "key";
    pendienteCargaMs = 0;
    envioId++; fase = 1; hayPendiente = true;
    return;
  }
  if (!c.startsWith("E|")) return;
  String p[7]; int k = 0, a = 2;
  while (k < 6) { int b = c.indexOf('|', a); if (b < 0) break; p[k++] = c.substring(a, b); a = b + 1; }
  if (k < 5) return;
  String t = (p[0] == "calc") ? soloCalc(c.substring(a)) : soloAscii(c.substring(a));
  if (!t.length()) { appErr = "vacio"; return; }
  if (!teclado.isPaired()) { appErr = "bt"; return; }
  if (hayPendiente) { appErr = "ocupado"; return; }
  appErr = "";
  pendienteTexto = t;
  pendienteModo = modoValido(p[0]) ? p[0] : "web";
  pendienteIphone = p[1] == "1";
  pendienteAbrirNav = p[2] != "0";
  long b = p[3].toInt(); pendienteBarra = (b >= 1 && b <= 3) ? (uint8_t)b : 1;
  long w = p[4].toInt(); pendienteCargaMs = (w >= 500 && w <= 10000) ? (uint32_t)w : 2500;
  if (p[0] != "calc") guardarAjustes(pendienteModo, pendienteIphone, pendienteAbrirNav, pendienteBarra, pendienteCargaMs);
  envioId++;
  fase = 1;
  hayPendiente = true;
}

// Avisa a la app cómo va todo, y deja la placa visible para que se conecten los dos celus
void appLoop() {
  appProcesar();
  NimBLEServer *srv = NimBLEDevice::getServer();
  if (srv && millis() - appTAdv > 1000) {
    appTAdv = millis();
    if (srv->getConnectedCount() < 3 && !NimBLEDevice::getAdvertising()->isAdvertising())
      NimBLEDevice::startAdvertising();
  }
  if (appEst && millis() - appTEst > 250) {
    appTEst = millis();
    if (fase == 2 && (int32_t)(millis() - listoEn) >= 0) fase = 3;
    String j = "{\"c\":" + String(teclado.isPaired() ? 1 : 0) + ",\"f\":" + String(fase) +
               ",\"id\":" + String(envioId) + ",\"n\":" + String(appN) + ",\"a\":" + String(armado.length() ? 1 : 0) + ",\"e\":\"" + appErr + "\"}";
    if (j != appUltEst) { appUltEst = j; appEst->setValue(j.c_str()); appEst->notify(); }
  }
}

// ---------------- PROGRAMA ----------------
void setup() {
  Serial.begin(115200);
  delay(300);

  prefs.begin("fm", false);
  gModo = prefs.getString("m", "web"); if (!modoValido(gModo)) gModo = "web"; gIphone = prefs.getBool("i", false);
  gAbrirNav = prefs.getBool("n", true); gBarra = prefs.getUChar("b", 1); gCargaMs = prefs.getUInt("w", 2500);
  wifiSsid = prefs.getString("ss", ""); wifiClave = prefs.getString("sp", "");

  // ---- 1) WiFi primero (su propia red + cliente de un WiFi con internet) ----
  WiFi.mode(WIFI_AP_STA);
  delay(100);
  bool apOk = WiFi.softAP(WIFI_NOMBRE, WIFI_CLAVE, 6);   // canal 6
  delay(300);
  // Las ESP32-C3 SuperMini tienen la antena floja: con potencia alta
  // a veces el celu no se puede conectar. 8.5 dBm lo soluciona.
#if CONFIG_IDF_TARGET_ESP32C3
  WiFi.setTxPower(WIFI_POWER_8_5dBm);   // solo en la C3 (en la ESP32 común, potencia normal)
#endif
  Serial.printf("WiFi AP: %s  |  red: %s  |  IP: %s\n",
                apOk ? "OK" : "FALLO", WiFi.softAPSSID().c_str(), WiFi.softAPIP().toString().c_str());
  WiFi.setAutoReconnect(false);          // no buscar sin parar (cortaría la red propia)
  WiFi.onEvent([](WiFiEvent_t e, WiFiEventInfo_t) {
    if (e == ARDUINO_EVENT_WIFI_AP_STACONNECTED)    Serial.println("WiFi: se conectó un celular");
    if (e == ARDUINO_EVENT_WIFI_AP_STADISCONNECTED) Serial.println("WiFi: se desconectó un celular");
  });

  // ---- 2) Bluetooth después ----
  teclado.setLogLevel(HIDLogLevel::Normal);
  teclado.setSecurityMode(BLEKeyboardSecurity::JustWorks);  // sin código: se vincula con un toque
  teclado.setPairingMode(HIDPairingMode::Auto);              // cualquier celular puede conectarse
  teclado.setRandomAddress(false);  // dirección fija: el celu se reconecta solo al prenderla
  teclado.setTapDelay(15);
  teclado.setKeyGap(15);
  teclado.begin();
  appSetup();                       // app Mind Search por Bluetooth
  penSetup();                       // lapicera (si está activada en los ajustes)
  botonSetup();                     // botón para disparar lo cargado
  empezarInternet(20000);           // internet solo si la lapicera está activada

  dns.start(53, "*", WiFi.softAPIP());

  servidor.on("/", paginaInicio);
  servidor.on("/manifest.json", paginaManifest);
  servidor.on("/icono.png", paginaIcono);
  servidor.on("/apple-touch-icon.png", paginaIcono);              // iPhone lo busca acá
  servidor.on("/apple-touch-icon-precomposed.png", paginaIcono);
  servidor.on("/favicon.ico", paginaIcono);
  servidor.on("/enviar", paginaEnviar);
  servidor.on("/estado", paginaEstado);
  servidor.on("/nuevo", paginaNuevo);
  servidor.on("/ajustes", paginaAjustes);
  servidor.on("/lapicera", paginaLapicera);
  servidor.on("/wifi", paginaWifi);
  servidor.on("/hotspot-detect.html", respApple);         // iPhone / Mac
  servidor.on("/library/test/success.html", respApple);   // iPhone viejo
  servidor.on("/generate_204", resp204);                   // Android
  servidor.on("/gen_204", resp204);                        // Android
  servidor.on("/connecttest.txt", respWindows);            // Windows
  servidor.on("/ncsi.txt", respNcsi);                      // Windows viejo
  servidor.on("/success.txt", respFirefox);                // Firefox
  servidor.onNotFound(paginaOtra);
  servidor.begin();

  Serial.printf("Memoria libre: %u bytes\n", ESP.getFreeHeap());
  Serial.println("MIND SEARCH lista");
  Serial.printf("WiFi: %s  clave: %s  ->  http://192.168.4.1\n", WIFI_NOMBRE, WIFI_CLAVE);
  Serial.printf("Bluetooth del espectador: \"%s\" (sin codigo)\n", NOMBRE_BT);
}

void loop() {
  dns.processNextRequest();
  servidor.handleClient();
  penLoop();
  revisarInternet();
  appLoop();
  botonLoop();

  if (pedirNuevo) {
    pedirNuevo = false;
    Serial.println("Nuevo espectador: corto y borro emparejados");
    teclado.clearBonds();
    teclado.end();
    delay(500);
    teclado.begin();
    appSetup();
    fase = 0;
  }

  if (hayPendiente) {
    Serial.printf("Escribiendo \"%s\"...\n", pendienteTexto.c_str());
    if (pendienteModo == "key") hacerTeclasEspeciales(pendienteTexto);
    else escribirEnReceptor(pendienteTexto, pendienteModo, pendienteIphone, pendienteAbrirNav, pendienteBarra);
    listoEn = millis() + (modoNavegador(pendienteModo) ? pendienteCargaMs : 0);
    fase = 2;
    hayPendiente = false;
  }
}
