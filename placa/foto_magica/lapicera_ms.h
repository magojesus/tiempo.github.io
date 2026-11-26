// =====================================================================
//  LAPICERA para Mind Search / Foto Mágica
//  El espectador escribe con la lapicera (NeoLAB, protocolo 2, la misma
//  de Vision). Cuando deja de escribir unos segundos, la ESP32 manda los
//  trazos al reconocedor de escritura de Google, recibe la palabra y la
//  manda al celular del espectador (foto / Google / Wikipedia / texto).
//
//  Necesita INTERNET en la ESP32: se conecta como cliente a un WiFi
//  (el hotspot de tu celu en el bolsillo o el WiFi del lugar) sin dejar
//  de dar su propia red para la app "Notas".
//
//  OJO: el reconocedor (inputtools.google.com) no es una API oficial de
//  Google; funciona hace años pero podría cambiar.
// =====================================================================
#pragma once
#include <NimBLEDevice.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>

extern Preferences prefs;
void mandarPalabraLapicera(const String &palabra);
bool asegurarInternet(uint32_t esperaMs);           // está en foto_magica.ino   // está en foto_magica.ino

// ---------------- ajustes ----------------
#define LAP_MAXPTS 1500       // puntos guardados por palabra (alcanza de sobra)
#define LAP_MAXSTK   60         // trazos por palabra
#define LAP_MINDIST  8          // distancia mínima entre puntos guardados (centésimas Ncode ~ 0,2 mm)

static const char* PEN_SVC128 = "4f99f138-9d53-5bfa-9e50-b147491afe68";
static const char* PEN_NOT128 = "64cd86b1-2256-5aeb-9f04-2caf6c60ae57";
static const char* PEN_WR128  = "8bc8cc7d-88ca-56b0-af9a-9bf514d0d61a";

// estado: 0 apagada, 1 buscando, 2 conectando, 3 preparando, 4 lista, 5 pide clave
volatile uint8_t penState = 0;
volatile int     penBat = -1;
bool     penEnabled = false;
String   penMac = "", penPw = "";
uint32_t penPausaMs = 2500;               // cuánto espera sin escribir para dar la palabra por terminada
volatile bool penKick = false, penMacNew = false;
char     penMacBuf[20] = "";
String   penUltima = "";                  // última palabra reconocida
String   penOpciones = "";                // otras opciones que dio Google (separadas por |)
String   penErr = "";                     // último problema (para los ajustes)

// ---------------- trazos de la palabra actual ----------------
static uint16_t lpX[LAP_MAXPTS], lpY[LAP_MAXPTS], lpT[LAP_MAXPTS];   // T en centésimas de segundo
static uint16_t lpStk[LAP_MAXSTK];        // dónde empieza cada trazo
static int      lpN = 0, lpNS = 0;
static uint32_t lpT0 = 0, lpUlt = 0;      // primer punto y último movimiento
static bool     lpAbajo = false;

struct PenEv { uint8_t t; uint16_t x, y; };   // 1 baja, 2 punto, 3 sube
static QueueHandle_t penQ = nullptr;
static inline void penPush(uint8_t t, uint16_t x = 0, uint16_t y = 0) {
  if (!penQ) return;
  PenEv e = { t, x, y };
  xQueueSend(penQ, &e, 0);
}

// ---------------- conexión con la lapicera ----------------
static NimBLEClient* penCli = nullptr;
static NimBLERemoteCharacteristic* penWr = nullptr;
static bool penWrResp = true;
static NimBLEAddress penAddr;
static volatile bool penDoConnect = false;
static volatile uint8_t penReq = 0;
#define PREQ_STATUS 1
#define PREQ_PW     2
#define PREQ_ONLINE 4
static bool penAuthed = false, penHover = false, penIsDown = false;

static void penSend(uint8_t cmd, const uint8_t* d, uint16_t n) {
  if (!penWr || !penCli || !penCli->isConnected()) return;
  uint8_t out[128]; int k = 0;
  out[k++] = 0xC0;
  auto put = [&](uint8_t b) {
    if (k > 124) return;
    if (b == 0xC0 || b == 0xC1 || b == 0x7D) { out[k++] = 0x7D; out[k++] = b ^ 0x20; } else out[k++] = b;
  };
  put(cmd); put(n & 0xFF); put(n >> 8);
  for (int i = 0; i < n; i++) put(d[i]);
  out[k++] = 0xC1;
  penWr->writeValue(out, k, penWrResp);
}
static void penReqVersion() {
  uint8_t d[42]; memset(d, 0, sizeof(d));
  d[16] = 0xF0; d[17] = 0x01;
  memcpy(d + 18, "0.0.0.0", 7);
  memcpy(d + 34, "2.18", 4);
  penSend(0x01, d, 42);
}
static void penReqStatus() { penSend(0x04, nullptr, 0); }
static void penReqPw() {
  uint8_t d[16]; memset(d, 0, 16);
  memcpy(d, penPw.c_str(), min((int)penPw.length(), 16));
  penSend(0x02, d, 16);
}
static void penReqOnline() { uint8_t d[2] = { 0xFF, 0xFF }; penSend(0x11, d, 2); }

static void penPacket(const uint8_t* p, int n) {
  if (n < 3) return;
  uint8_t cmd = p[0];
  int o = 1, res = 0;
  bool hasRes = (cmd >> 4) != 6 && cmd != 0x73 && cmd != 0x24 && cmd != 0x32;
  if (hasRes) res = p[o++];
  if (o + 2 > n) return;
  o += 2;
  const uint8_t* d = p + o;
  int dl = n - o;
  if (hasRes && res != 0) return;
  switch (cmd) {
    case 0x81: penReq |= PREQ_STATUS; break;
    case 0x84:
      if (dl >= 21) { penBat = d[20]; penHover = d[19] == 1; }
      if (!penAuthed) {
        if (dl >= 1 && d[0] == 1) penReq |= PREQ_PW;
        else { penAuthed = true; penReq |= PREQ_ONLINE; }
      }
      break;
    case 0x82:
      if (dl >= 1 && d[0] == 1) { penAuthed = true; penReq |= PREQ_ONLINE; } else penState = 5;
      break;
    case 0x91: penState = 4; break;
    case 0x69: penIsDown = true;  penPush(1); break;
    case 0x6A: penIsDown = false; penPush(3); break;
    case 0x63: if (dl >= 1) { penIsDown = d[0] == 0; penPush(penIsDown ? 1 : 3); } break;
    case 0x65: case 0x6C: {
      int k = cmd == 0x6C ? 1 : 0;
      if (dl < k + 9) break;
      int x = d[k + 3] | (d[k + 4] << 8), y = d[k + 5] | (d[k + 6] << 8);
      int fx = d[k + 7], fy = d[k + 8];
      if (!penIsDown) {
        if (penHover) break;
        penIsDown = true; penPush(1);
      }
      uint32_t X = (uint32_t)x * 100 + fx, Y = (uint32_t)y * 100 + fy;   // resolución completa
      if (X > 65000) X = 65000;
      if (Y > 65000) Y = 65000;
      penPush(2, X, Y);
      break;
    }
  }
}
static uint8_t penBuf[700];
static int     penBn = -1;
static bool    penEsc = false;
static void penNotify(NimBLERemoteCharacteristic*, uint8_t* data, size_t len, bool) {
  for (size_t i = 0; i < len; i++) {
    uint8_t b = data[i];
    if (b == 0xC0) { penBn = 0; penEsc = false; continue; }
    if (penBn < 0) continue;
    if (b == 0xC1) { penPacket(penBuf, penBn); penBn = -1; continue; }
    if (b == 0x7D && !penEsc) { penEsc = true; continue; }
    if (penEsc) { b ^= 0x20; penEsc = false; }
    if (penBn < (int)sizeof(penBuf)) penBuf[penBn++] = b;
  }
}

class PenScanCB : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice* dev) override {
    if (penDoConnect) return;
    String nm = dev->getName().c_str(); nm.toLowerCase();
    bool isPen = dev->isAdvertisingService(NimBLEUUID((uint16_t)0x19F1)) ||
                 dev->isAdvertisingService(NimBLEUUID(PEN_SVC128)) ||
                 nm.indexOf("smartpen") >= 0 || nm.startsWith("neo") || nm.startsWith("nwp");
    if (!isPen) return;
    String mac = dev->getAddress().toString().c_str(); mac.toLowerCase();
    if (penMac.length() && mac != penMac) return;     // hay otra lapicera vinculada
    penAddr = dev->getAddress();
    penDoConnect = true;
    NimBLEDevice::getScan()->stop();
  }
} penScanCB;

class PenCliCB : public NimBLEClientCallbacks {
  void onDisconnect(NimBLEClient*, int) override {
    penWr = nullptr; penAuthed = false; penIsDown = false;
    if (penState) penState = 1;
    penPush(3);
  }
} penCliCB;

static bool penConnect() {
  if (!penCli) {
    penCli = NimBLEDevice::createClient();
    penCli->setClientCallbacks(&penCliCB, false);
    penCli->setConnectTimeout(6000);
  }
  if (!penCli->connect(penAddr)) return false;
  NimBLERemoteCharacteristic *nt = nullptr, *wr = nullptr;
  NimBLERemoteService* s = penCli->getService(NimBLEUUID(PEN_SVC128));
  if (s) { nt = s->getCharacteristic(NimBLEUUID(PEN_NOT128)); wr = s->getCharacteristic(NimBLEUUID(PEN_WR128)); }
  if (!nt || !wr) {
    s = penCli->getService(NimBLEUUID((uint16_t)0x19F1));
    if (s) { nt = s->getCharacteristic(NimBLEUUID((uint16_t)0x2BA1)); wr = s->getCharacteristic(NimBLEUUID((uint16_t)0x2BA0)); }
  }
  if (!nt || !wr) { penCli->disconnect(); return false; }
  penBn = -1; penAuthed = false; penReq = 0;
  if (!nt->subscribe(true, penNotify)) { penCli->disconnect(); return false; }
  penWr = wr;
  penWrResp = wr->canWrite();
  String mac = penCli->getPeerAddress().toString().c_str(); mac.toLowerCase();
  if (mac != penMac) { strncpy(penMacBuf, mac.c_str(), sizeof(penMacBuf) - 1); penMacNew = true; }
  penState = 3;
  vTaskDelay(pdMS_TO_TICKS(500));
  penReqVersion();
  return true;
}

// Tarea aparte: busca, conecta y le contesta a la lapicera
static void penTask(void*) {
  uint32_t tAsk = 0, tScan = 0;
  bool scanListo = false;
  for (;;) {
    if (penKick) {
      penKick = false;
      if (penCli && penCli->isConnected()) penCli->disconnect();
      vTaskDelay(pdMS_TO_TICKS(300));
    }
    if (!penEnabled || !NimBLEDevice::isInitialized()) {
      if (penCli && penCli->isConnected()) penCli->disconnect();
      penState = 0; penDoConnect = false;
      vTaskDelay(pdMS_TO_TICKS(300));
      continue;
    }
    if (!scanListo) {                      // el Bluetooth ya lo prendió el teclado
      NimBLEScan* sc = NimBLEDevice::getScan();
      sc->setScanCallbacks(&penScanCB, false);
      sc->setActiveScan(true);
      sc->setInterval(100);
      sc->setWindow(30);                   // escucha poco: WiFi y teclado siguen rápidos
      scanListo = true;
    }
    if (penCli && penCli->isConnected() && penWr) {
      uint8_t r = penReq;
      if (r) {
        penReq = 0;
        if (r & PREQ_STATUS) penReqStatus();
        if (r & PREQ_PW) { if (penPw.length()) penReqPw(); else penState = 5; }
        if (r & PREQ_ONLINE) penReqOnline();
        tAsk = millis();
      }
      if (penState == 3 && millis() - tAsk > 4000) { tAsk = millis(); penReqVersion(); }
      if (penState == 4 && millis() - tAsk > 60000) { tAsk = millis(); penReqStatus(); }
      vTaskDelay(pdMS_TO_TICKS(20));
      continue;
    }
    if (penDoConnect) {
      penState = 2;
      bool ok = penConnect();
      penDoConnect = false;
      if (!ok) penState = 1;
      continue;
    }
    penState = 1;
    NimBLEScan* sc = NimBLEDevice::getScan();
    if (!sc->isScanning() && millis() - tScan > 1500) { sc->start(4000, false, true); tScan = millis(); }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// ---------------- pasar de UTF-8 / \uXXXX a letras sin acento ----------------
static char sinAcento(uint32_t cp) {
  if (cp < 128) return (char)cp;
  switch (cp) {
    case 0xE1: case 0xE0: case 0xE4: case 0xE2: case 0xE3: return 'a';
    case 0xC1: case 0xC0: case 0xC4: case 0xC2: case 0xC3: return 'A';
    case 0xE9: case 0xE8: case 0xEB: case 0xEA: return 'e';
    case 0xC9: case 0xC8: case 0xCB: case 0xCA: return 'E';
    case 0xED: case 0xEC: case 0xEF: case 0xEE: return 'i';
    case 0xCD: case 0xCC: case 0xCF: case 0xCE: return 'I';
    case 0xF3: case 0xF2: case 0xF6: case 0xF4: case 0xF5: return 'o';
    case 0xD3: case 0xD2: case 0xD6: case 0xD4: case 0xD5: return 'O';
    case 0xFA: case 0xF9: case 0xFC: case 0xFB: return 'u';
    case 0xDA: case 0xD9: case 0xDC: case 0xDB: return 'U';
    case 0xF1: return 'n';
    case 0xD1: return 'N';
    case 0xE7: return 'c';
    case 0xC7: return 'C';
  }
  return 0;
}

// Saca la primera palabra candidata de la respuesta de Google:
// ["SUCCESS",[["<id>",["francia","Francia",...],[],{...}]]]
static String unaCandidata(const String &r, int &i) {   // i: justo después de la comilla de apertura
  String out;
  while (i < (int)r.length() && out.length() < 60) {
    char c = r[i];
    if (c == '"') break;
    uint32_t cp = (uint8_t)c;
    if (c == '\\' && i + 1 < (int)r.length()) {
      char e = r[i + 1];
      if (e == 'u' && i + 5 < (int)r.length()) { cp = strtoul(r.substring(i + 2, i + 6).c_str(), nullptr, 16); i += 6; }
      else { cp = (uint8_t)e; i += 2; }
    } else if ((cp & 0xE0) == 0xC0 && i + 1 < (int)r.length()) {   // UTF-8 de 2 bytes
      cp = ((cp & 0x1F) << 6) | ((uint8_t)r[i + 1] & 0x3F); i += 2;
    } else if (cp >= 0x80) {                                       // otros: se saltean
      i++; while (i < (int)r.length() && ((uint8_t)r[i] & 0xC0) == 0x80) i++;
      continue;
    } else i++;
    char a = sinAcento(cp);
    if ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || (a >= '0' && a <= '9') || a == ' ' || a == '-' || a == '\'') out += a;
  }
  while (i < (int)r.length() && r[i] != '"') i++;
  i++;                                                  // pasa la comilla de cierre
  out.trim();
  return out;
}

// Devuelve la mejor palabra y deja las otras en penOpciones
static String primeraCandidata(const String &r) {
  penOpciones = "";
  if (!r.startsWith("[\"SUCCESS\"")) return "";
  int i = r.indexOf("\",[\"");
  if (i < 0) return "";
  i += 4;
  String primera = unaCandidata(r, i);
  for (int n = 0; n < 4 && i + 1 < (int)r.length() && r[i] == ',' && r[i + 1] == '"'; n++) {
    i += 2;
    String w = unaCandidata(r, i);
    if (w.length() && !w.equalsIgnoreCase(primera)) { if (penOpciones.length()) penOpciones += '|'; penOpciones += w; }
  }
  return primera;
}

// Manda los trazos a Google y devuelve la palabra ("" si no pudo)
static String reconocer() {
  if (!asegurarInternet(9000)) { penErr = "Sin internet: prendé el hotspot / revisá el WiFi en los ajustes"; return ""; }
  int x0 = 65535, y0 = 65535, x1 = 0, y1 = 0;
  for (int i = 0; i < lpN; i++) { x0 = min(x0, (int)lpX[i]); y0 = min(y0, (int)lpY[i]); x1 = max(x1, (int)lpX[i]); y1 = max(y1, (int)lpY[i]); }

  // ¿La hoja está al revés? Al escribir, la mano avanza hacia la derecha con el tiempo.
  // Si los puntos avanzan hacia la izquierda, la escritura llegó girada 180°: se endereza.
  {
    double mt = 0, mx = 0, cov = 0;
    for (int i = 0; i < lpN; i++) { mt += lpT[i]; mx += lpX[i]; }
    mt /= lpN; mx /= lpN;
    for (int i = 0; i < lpN; i++) cov += (lpT[i] - mt) * (lpX[i] - mx);
    if (cov < 0) {
      for (int i = 0; i < lpN; i++) { lpX[i] = x1 - lpX[i] + x0; lpY[i] = y1 - lpY[i] + y0; }
      Serial.println("Lapicera: la hoja estaba al revés, la enderezo");
    }
  }

  String b;
  b.reserve(lpN * 18 + 400);
  b = "{\"app_version\":0.4,\"api_level\":\"537.36\",\"device\":\"Mozilla/5.0\",\"input_type\":0,"
      "\"options\":\"enable_pre_space\",\"requests\":[{\"writing_guide\":{\"writing_area_width\":";
  b += String(max(1, x1 - x0 + 20)); b += ",\"writing_area_height\":"; b += String(max(1, y1 - y0 + 20));
  b += "},\"pre_context\":\"\",\"max_num_results\":6,\"max_completions\":0,\"language\":\"es\",\"ink\":[";
  for (int s = 0; s < lpNS; s++) {
    int a = lpStk[s], z = (s + 1 < lpNS) ? lpStk[s + 1] : lpN;
    if (z <= a) continue;
    if (s) b += ',';
    b += "[[";
    for (int i = a; i < z; i++) { if (i > a) b += ','; b += String(lpX[i] - x0 + 10); }
    b += "],[";
    for (int i = a; i < z; i++) { if (i > a) b += ','; b += String(lpY[i] - y0 + 10); }
    b += "],[";
    for (int i = a; i < z; i++) { if (i > a) b += ','; b += String((uint32_t)lpT[i] * 10); }
    b += "]]";
  }
  b += "]}]}";

  IPAddress gip;
  if (!WiFi.hostByName("inputtools.google.com", gip)) {
    penErr = "No encuentra a Google (¿el hotspot tiene datos?)";
    Serial.println("Lapicera: falló el DNS de inputtools.google.com");
    return "";
  }
  Serial.printf("Lapicera: Google=%s  memoria libre %u (bloque mayor %u)\n",
                gip.toString().c_str(), ESP.getFreeHeap(), ESP.getMaxAllocHeap());

  Serial.print("TINTA:"); Serial.println(b);          // para revisar el dibujo que se manda
  const char *RUTA = "inputtools.google.com/request?ime=handwriting&app=mobilesearch&cs=1&oe=UTF-8";
  int code = -1;
  String resp;
  {                                                    // 1) conexión común (rápida, poca memoria)
    WiFiClient cli;
    HTTPClient http;
    http.setTimeout(8000);
    if (http.begin(cli, String("http://") + RUTA)) {
      http.addHeader("Content-Type", "application/json");
      code = http.POST(b);
      if (code > 0) resp = http.getString();
      http.end();
    }
    if (code != 200) Serial.printf("Lapicera: http respondió %d -> pruebo https\n", code);
  }
  if (code != 200) {                                   // 2) conexión segura (https)
    code = -1; resp = "";
    WiFiClientSecure cli;
    cli.setInsecure();
    HTTPClient http;
    http.setTimeout(8000);
    if (http.begin(cli, String("https://") + RUTA)) {
      http.addHeader("Content-Type", "application/json");
      code = http.POST(b);
      if (code > 0) resp = http.getString();
      http.end();
    }
    if (code <= 0) { char e[90] = ""; cli.lastError(e, sizeof(e)); Serial.printf("Lapicera: https falló (%d) %s\n", code, e); }
  }
  if (code != 200) { penErr = "Google respondió " + String(code); return ""; }
  Serial.print("RESPUESTA:"); Serial.println(resp.substring(0, 300));
  String w = primeraCandidata(resp);
  if (!w.length()) penErr = "No reconoció la letra";
  else penErr = "";
  return w;
}

static void lpBorrar() { lpN = 0; lpNS = 0; lpAbajo = false; }

// ---------------- se llama en cada vuelta de loop() ----------------
void penLoop() {
  if (penMacNew) { penMacNew = false; penMac = penMacBuf; prefs.putString("pmac", penMac); }
  if (!penQ) return;
  PenEv e;
  int k = 0;
  while (k < 200 && xQueueReceive(penQ, &e, 0) == pdTRUE) {
    k++;
    lpUlt = millis();
    if (e.t == 1) {                                   // apoya: empieza un trazo
      if (lpN == 0) lpT0 = millis();
      if (lpNS < LAP_MAXSTK && lpN < LAP_MAXPTS) lpStk[lpNS++] = lpN;
      lpAbajo = true;
      continue;
    }
    if (e.t == 3) { lpAbajo = false; continue; }      // levanta
    if (!lpAbajo || lpNS == 0 || lpN >= LAP_MAXPTS) continue;
    if (lpN > lpStk[lpNS - 1]) {                       // descarta puntos casi iguales
      int dx = (int)e.x - lpX[lpN - 1], dy = (int)e.y - lpY[lpN - 1];
      if (dx * dx + dy * dy < LAP_MINDIST * LAP_MINDIST) continue;
    }
    lpX[lpN] = e.x; lpY[lpN] = e.y;
    lpT[lpN] = (uint16_t)min((uint32_t)65000, (millis() - lpT0) / 10);
    lpN++;
  }
  // terminó de escribir: pasó la pausa sin tocar el papel
  if (lpN > 3 && !lpAbajo && millis() - lpUlt > penPausaMs) {
    Serial.printf("Lapicera: reconociendo %d puntos en %d trazos...\n", lpN, lpNS);
    String w = reconocer();
    lpBorrar();
    if (w.length()) {
      penUltima = w;
      Serial.printf("Lapicera: escribió \"%s\"\n", w.c_str());
      mandarPalabraLapicera(w);
    } else {
      Serial.printf("Lapicera: %s\n", penErr.c_str());
    }
  } else if (lpN > 0 && lpN <= 3 && !lpAbajo && millis() - lpUlt > penPausaMs) {
    lpBorrar();                                       // un puntito suelto: se ignora
  }
}

static bool penTaskOn = false;
void penTaskStart() {
  if (!penTaskOn) { penTaskOn = true; xTaskCreatePinnedToCore(penTask, "pen", 6144, nullptr, 1, nullptr, 0); }
}
void penActivar(bool on) {
  penEnabled = on; prefs.putBool("pen", on);
  if (on) penTaskStart();
  if (!on) { penMac = ""; prefs.putString("pmac", ""); penKick = true; }
}
void penOlvidar() { penMac = ""; prefs.putString("pmac", ""); penKick = true; }
void penSetup() {
  penEnabled = prefs.getBool("pen", false);
  penMac = prefs.getString("pmac", "");
  penPw  = prefs.getString("ppw", "");
  penPausaMs = prefs.getUInt("pau", 2500);
  penQ = xQueueCreate(400, sizeof(PenEv));
  if (penEnabled) penTaskStart();
}
