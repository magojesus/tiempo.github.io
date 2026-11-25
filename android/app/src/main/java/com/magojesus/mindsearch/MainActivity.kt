package com.magojesus.mindsearch

import android.Manifest
import android.annotation.SuppressLint
import android.app.Activity
import android.app.AlertDialog
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothGatt
import android.bluetooth.BluetoothGattCallback
import android.bluetooth.BluetoothGattCharacteristic
import android.bluetooth.BluetoothGattDescriptor
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothProfile
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanResult
import android.bluetooth.le.ScanSettings
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Color
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Base64
import android.view.View
import android.webkit.JavascriptInterface
import android.webkit.WebChromeClient
import android.webkit.WebResourceRequest
import android.webkit.WebResourceResponse
import android.webkit.WebView
import android.webkit.WebViewClient
import android.widget.ArrayAdapter
import androidx.webkit.WebViewAssetLoader
import org.json.JSONArray
import org.json.JSONObject
import java.util.UUID

/**
 * Mind Search: la app de "Notas" trucha. La interfaz es la misma página web
 * (assets/index.html). Como el WebView de Android no trae Bluetooth, esta
 * actividad le da a la página un "navigator.bluetooth" propio (assets/bridge.js)
 * que usa el Bluetooth del celular por debajo.
 */
@SuppressLint("MissingPermission")
class MainActivity : Activity() {

    private lateinit var web: WebView
    private val main = Handler(Looper.getMainLooper())
    private val btAdapter: BluetoothAdapter? by lazy {
        (getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager).adapter
    }
    private val prefs by lazy { getSharedPreferences("ble", MODE_PRIVATE) }

    // ---------- conexiones abiertas ----------
    private class Conn(val addr: String) {
        var gatt: BluetoothGatt? = null
        var connectCb: String? = null
        val queue = ArrayDeque<() -> Unit>()
        var busy = false
        var pendingCb: String? = null       // callback de la operación en curso
    }
    private val conns = HashMap<String, Conn>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        window.statusBarColor = Color.WHITE
        window.decorView.systemUiVisibility = View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR

        val loader = WebViewAssetLoader.Builder()
            .addPathHandler("/assets/", WebViewAssetLoader.AssetsPathHandler(this))
            .build()
        web = WebView(this)
        web.settings.javaScriptEnabled = true
        web.settings.domStorageEnabled = true
        web.settings.mediaPlaybackRequiresUserGesture = false
        web.webChromeClient = WebChromeClient()
        web.webViewClient = object : WebViewClient() {
            override fun shouldInterceptRequest(v: WebView, r: WebResourceRequest): WebResourceResponse? =
                loader.shouldInterceptRequest(r.url)
        }
        web.addJavascriptInterface(Bridge(), "AndroidBLE")
        setContentView(web)
        web.loadUrl("https://appassets.androidplatform.net/assets/index.html")
        pedirPermisos()
    }

    override fun onDestroy() {
        conns.values.forEach { it.gatt?.close() }
        super.onDestroy()
    }

    // ---------- permisos ----------
    private fun permisosNecesarios(): Array<String> =
        if (Build.VERSION.SDK_INT >= 31)
            arrayOf(Manifest.permission.BLUETOOTH_SCAN, Manifest.permission.BLUETOOTH_CONNECT)
        else arrayOf(Manifest.permission.ACCESS_FINE_LOCATION)

    private fun tienePermisos() = permisosNecesarios().all {
        checkSelfPermission(it) == PackageManager.PERMISSION_GRANTED
    }

    private fun pedirPermisos() {
        if (!tienePermisos()) requestPermissions(permisosNecesarios(), 1)
    }

    // ---------- respuestas a la página ----------
    private fun js(code: String) = main.post { web.evaluateJavascript(code, null) }
    private fun ok(cb: String?, payload: Any? = null) {
        if (cb == null) return
        val p = when (payload) { null -> "null"; is String -> JSONObject.quote(payload); else -> payload.toString() }
        js("window.__bleCb(${JSONObject.quote(cb)},true,$p)")
    }
    private fun err(cb: String?, name: String, msg: String) {
        if (cb == null) return
        js("window.__bleCb(${JSONObject.quote(cb)},false,${JSONObject().put("name", name).put("message", msg)})")
    }

    // ---------- elegir dispositivo ----------
    private data class Filtro(val name: String?, val prefix: String?, val services: List<UUID>)

    private fun leerFiltros(json: String): List<Filtro> {
        val out = ArrayList<Filtro>()
        val arr = JSONArray(json)
        for (i in 0 until arr.length()) {
            val o = arr.getJSONObject(i)
            val sv = ArrayList<UUID>()
            o.optJSONArray("services")?.let { a -> for (k in 0 until a.length()) sv.add(UUID.fromString(a.getString(k))) }
            out.add(Filtro(o.optString("name").ifEmpty { null }, o.optString("namePrefix").ifEmpty { null }, sv))
        }
        return out
    }

    private fun coincide(f: List<Filtro>, r: ScanResult): Boolean {
        val nombre = r.scanRecord?.deviceName ?: r.device.name ?: ""
        val uuids = r.scanRecord?.serviceUuids?.map { it.uuid } ?: emptyList()
        return f.any { x ->
            (x.name != null && nombre == x.name) ||
            (x.prefix != null && nombre.startsWith(x.prefix)) ||
            (x.services.isNotEmpty() && x.services.any { it in uuids })
        }
    }

    private fun elegir(cb: String, filtrosJson: String) {
        val ad = btAdapter
        if (ad == null || !ad.isEnabled) {
            startActivity(Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE))
            err(cb, "NetworkError", "Prendé el Bluetooth del celular y probá de nuevo")
            return
        }
        if (!tienePermisos()) { pedirPermisos(); err(cb, "SecurityError", "Falta el permiso de Bluetooth"); return }
        val filtros = leerFiltros(filtrosJson)
        val encontrados = LinkedHashMap<String, String>()   // dirección -> nombre
        val lista = ArrayList<String>()
        val adapter = ArrayAdapter(this, android.R.layout.simple_list_item_1, lista)
        val scanner = ad.bluetoothLeScanner
        var terminado = false
        val scb = object : ScanCallback() {
            override fun onScanResult(type: Int, r: ScanResult) {
                if (!coincide(filtros, r)) return
                val a = r.device.address
                if (encontrados.containsKey(a)) return
                val n = r.scanRecord?.deviceName ?: r.device.name ?: a
                encontrados[a] = n
                main.post { lista.add(n); adapter.notifyDataSetChanged() }
            }
        }
        val dlg = AlertDialog.Builder(this)
            .setTitle("Buscando...")
            .setAdapter(adapter) { _, which ->
                if (terminado) return@setAdapter
                terminado = true
                scanner?.stopScan(scb)
                val addr = encontrados.keys.elementAt(which)
                val name = encontrados[addr] ?: ""
                prefs.edit().putString("n_$addr", name).apply()
                ok(cb, JSONObject().put("id", addr).put("name", name))
            }
            .setNegativeButton("Cancelar") { _, _ -> }
            .setOnDismissListener {
                if (!terminado) { terminado = true; scanner?.stopScan(scb); err(cb, "NotFoundError", "Cancelado") }
            }
            .create()
        dlg.show()
        scanner?.startScan(null, ScanSettings.Builder().setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY).build(), scb)
        main.postDelayed({ if (!terminado) { scanner?.stopScan(scb); dlg.setTitle(if (lista.isEmpty()) "No se encontró nada (¿está prendido?)" else "Elegí uno") } }, 12000)
    }

    // ---------- GATT ----------
    private fun conn(addr: String) = conns.getOrPut(addr) { Conn(addr) }

    private fun encolar(c: Conn, op: () -> Unit) {
        main.post { c.queue.addLast(op); siguiente(c) }
    }
    private fun siguiente(c: Conn) {
        if (c.busy) return
        val op = c.queue.removeFirstOrNull() ?: return
        c.busy = true
        op()
    }
    private fun terminar(c: Conn) {
        main.post { c.busy = false; c.pendingCb = null; siguiente(c) }
    }

    private fun conectar(cb: String, addr: String) {
        val ad = btAdapter ?: return err(cb, "NetworkError", "Sin Bluetooth")
        val c = conn(addr)
        c.gatt?.let { g ->
            // reconexión limpia
            g.close(); c.gatt = null
        }
        c.queue.clear(); c.busy = false
        c.connectCb = cb
        val dev: BluetoothDevice = ad.getRemoteDevice(addr)
        c.gatt = dev.connectGatt(this, false, callback(c), BluetoothDevice.TRANSPORT_LE)
        main.postDelayed({
            if (c.connectCb == cb) { c.connectCb = null; c.gatt?.close(); c.gatt = null; err(cb, "NetworkError", "No respondió (¿está prendido y cerca?)") }
        }, 15000)
    }

    private fun callback(c: Conn) = object : BluetoothGattCallback() {
        override fun onConnectionStateChange(g: BluetoothGatt, status: Int, newState: Int) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                main.post { g.discoverServices() }
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                val cb = c.connectCb
                c.connectCb = null
                main.post {
                    g.close()
                    if (c.gatt === g) c.gatt = null
                    c.queue.clear(); c.busy = false
                    c.pendingCb?.let { err(it, "NetworkError", "Se desconectó") }
                    c.pendingCb = null
                }
                if (cb != null) err(cb, "NetworkError", "No se pudo conectar ($status)")
                else js("window.__bleDisc(${JSONObject.quote(c.addr)})")
            }
        }
        override fun onServicesDiscovered(g: BluetoothGatt, status: Int) {
            main.post { g.requestMtu(185) }
            main.postDelayed({
                val cb = c.connectCb ?: return@postDelayed
                c.connectCb = null
                if (status == BluetoothGatt.GATT_SUCCESS) ok(cb) else err(cb, "NetworkError", "Error al leer servicios ($status)")
            }, 300)
        }
        override fun onCharacteristicRead(g: BluetoothGatt, ch: BluetoothGattCharacteristic, value: ByteArray, status: Int) {
            val cb = c.pendingCb
            if (status == BluetoothGatt.GATT_SUCCESS) ok(cb, Base64.encodeToString(value, Base64.NO_WRAP))
            else err(cb, "NetworkError", "Error al leer ($status)")
            terminar(c)
        }
        @Deprecated("Android < 13")
        override fun onCharacteristicRead(g: BluetoothGatt, ch: BluetoothGattCharacteristic, status: Int) {
            @Suppress("DEPRECATION")
            onCharacteristicRead(g, ch, ch.value ?: ByteArray(0), status)
        }
        override fun onCharacteristicWrite(g: BluetoothGatt, ch: BluetoothGattCharacteristic, status: Int) {
            val cb = c.pendingCb
            if (status == BluetoothGatt.GATT_SUCCESS) ok(cb) else err(cb, "NetworkError", "Error al escribir ($status)")
            terminar(c)
        }
        override fun onDescriptorWrite(g: BluetoothGatt, d: BluetoothGattDescriptor, status: Int) {
            val cb = c.pendingCb
            if (status == BluetoothGatt.GATT_SUCCESS) ok(cb) else err(cb, "NetworkError", "Error al activar avisos ($status)")
            terminar(c)
        }
        override fun onCharacteristicChanged(g: BluetoothGatt, ch: BluetoothGattCharacteristic, value: ByteArray) {
            val b = Base64.encodeToString(value, Base64.NO_WRAP)
            js("window.__bleNotify(${JSONObject.quote(c.addr)},${JSONObject.quote(ch.uuid.toString())},${JSONObject.quote(b)})")
        }
        @Deprecated("Android < 13")
        override fun onCharacteristicChanged(g: BluetoothGatt, ch: BluetoothGattCharacteristic) {
            @Suppress("DEPRECATION")
            onCharacteristicChanged(g, ch, ch.value ?: ByteArray(0))
        }
    }

    private fun caracteristica(c: Conn, svc: String, chr: String): BluetoothGattCharacteristic? =
        c.gatt?.getService(UUID.fromString(svc))?.getCharacteristic(UUID.fromString(chr))

    // ---------- lo que llama la página ----------
    inner class Bridge {
        @JavascriptInterface fun requestDevice(cb: String, filtros: String) { main.post { elegir(cb, filtros) } }

        @JavascriptInterface fun getDevices(): String {
            val arr = JSONArray()
            prefs.all.keys.filter { it.startsWith("n_") }.forEach {
                arr.put(JSONObject().put("id", it.substring(2)).put("name", prefs.getString(it, "")))
            }
            return arr.toString()
        }

        @JavascriptInterface fun connect(cb: String, addr: String) { main.post { conectar(cb, addr) } }

        @JavascriptInterface fun disconnect(addr: String) {
            main.post { conns[addr]?.let { it.gatt?.disconnect() } }
        }

        @JavascriptInterface fun hasService(addr: String, svc: String): Boolean =
            conns[addr]?.gatt?.getService(UUID.fromString(svc)) != null

        @JavascriptInterface fun hasChar(addr: String, svc: String, chr: String): Boolean =
            conns[addr]?.let { caracteristica(it, svc, chr) } != null

        @JavascriptInterface fun write(cb: String, addr: String, svc: String, chr: String, b64: String, conRespuesta: Boolean) {
            val c = conns[addr] ?: return err(cb, "NetworkError", "No conectado")
            val data = Base64.decode(b64, Base64.NO_WRAP)
            encolar(c) {
                val g = c.gatt
                val ch = caracteristica(c, svc, chr)
                if (g == null || ch == null) { err(cb, "NetworkError", "No conectado"); terminar(c); return@encolar }
                val tipo = if (conRespuesta) BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT else BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE
                c.pendingCb = cb
                val okEnvio = if (Build.VERSION.SDK_INT >= 33) {
                    g.writeCharacteristic(ch, data, tipo) == BluetoothGatt.GATT_SUCCESS
                } else {
                    @Suppress("DEPRECATION")
                    run { ch.writeType = tipo; ch.value = data; g.writeCharacteristic(ch) }
                }
                if (!okEnvio) { c.pendingCb = null; err(cb, "NetworkError", "No se pudo escribir"); terminar(c) }
            }
        }

        @JavascriptInterface fun read(cb: String, addr: String, svc: String, chr: String) {
            val c = conns[addr] ?: return err(cb, "NetworkError", "No conectado")
            encolar(c) {
                val g = c.gatt
                val ch = caracteristica(c, svc, chr)
                if (g == null || ch == null) { err(cb, "NetworkError", "No conectado"); terminar(c); return@encolar }
                c.pendingCb = cb
                if (!g.readCharacteristic(ch)) { c.pendingCb = null; err(cb, "NetworkError", "No se pudo leer"); terminar(c) }
            }
        }

        @JavascriptInterface fun notify(cb: String, addr: String, svc: String, chr: String) {
            val c = conns[addr] ?: return err(cb, "NetworkError", "No conectado")
            encolar(c) {
                val g = c.gatt
                val ch = caracteristica(c, svc, chr)
                if (g == null || ch == null) { err(cb, "NetworkError", "No conectado"); terminar(c); return@encolar }
                g.setCharacteristicNotification(ch, true)
                val d = ch.getDescriptor(UUID.fromString("00002902-0000-1000-8000-00805f9b34fb"))
                if (d == null) { ok(cb); terminar(c); return@encolar }
                val valor = if (ch.properties and BluetoothGattCharacteristic.PROPERTY_NOTIFY != 0)
                    BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE else BluetoothGattDescriptor.ENABLE_INDICATION_VALUE
                c.pendingCb = cb
                val okEnvio = if (Build.VERSION.SDK_INT >= 33) {
                    g.writeDescriptor(d, valor) == BluetoothGatt.GATT_SUCCESS
                } else {
                    @Suppress("DEPRECATION")
                    run { d.value = valor; g.writeDescriptor(d) }
                }
                if (!okEnvio) { c.pendingCb = null; err(cb, "NetworkError", "No se pudieron activar los avisos"); terminar(c) }
            }
        }
    }
}
