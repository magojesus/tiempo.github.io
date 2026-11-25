// navigator.bluetooth para la app Android (usa el Bluetooth del celu vía AndroidBLE)
(function(){
 if(!window.AndroidBLE)return;
 const A=window.AndroidBLE;
 let n=0;const cbs={},devs={},chars={};
 window.__bleCb=(id,ok,p)=>{const c=cbs[id];if(!c)return;delete cbs[id];
  if(ok)c.res(p);else{const e=new Error(p&&p.message||'error');e.name=p&&p.name||'NetworkError';c.rej(e)}};
 window.__bleDisc=(addr)=>{const d=devs[addr];if(d){d.gatt.connected=false;d._emit('gattserverdisconnected')}};
 window.__bleNotify=(addr,chr,b64)=>{const c=chars[addr+'|'+chr.toLowerCase()];if(!c)return;c.value=dv(b64);c._emit('characteristicvaluechanged')};
 function call(fn,...args){return new Promise((res,rej)=>{const id='c'+(++n);cbs[id]={res,rej};A[fn](id,...args)})}
 function canon(u){if(typeof u==='number')return('0000'+u.toString(16)).slice(-4).padStart(8,'0')+'-0000-1000-8000-00805f9b34fb';return String(u).toLowerCase()}
 function dv(b64){const s=atob(b64||'');const u=new Uint8Array(s.length);for(let i=0;i<s.length;i++)u[i]=s.charCodeAt(i);return new DataView(u.buffer)}
 function b64(buf){const u=buf instanceof ArrayBuffer?new Uint8Array(buf):new Uint8Array(buf.buffer,buf.byteOffset,buf.byteLength);let s='';for(let i=0;i<u.length;i++)s+=String.fromCharCode(u[i]);return btoa(s)}
 class Ev{constructor(){this._l={}}addEventListener(t,f){(this._l[t]=this._l[t]||[]).push(f)}removeEventListener(t,f){this._l[t]=(this._l[t]||[]).filter(x=>x!==f)}_emit(t){(this._l[t]||[]).forEach(f=>{try{f({type:t,target:this})}catch(e){}})}}
 class Chr extends Ev{constructor(d,s,u){super();this.service={uuid:s};this.uuid=u;this._d=d;this._s=s;this.value=new DataView(new ArrayBuffer(0))}
  writeValueWithResponse(b){return call('write',this._d.id,this._s,this.uuid,b64(b),true)}
  writeValueWithoutResponse(b){return call('write',this._d.id,this._s,this.uuid,b64(b),false)}
  writeValue(b){return this.writeValueWithResponse(b)}
  async readValue(){this.value=dv(await call('read',this._d.id,this._s,this.uuid));return this.value}
  async startNotifications(){chars[this._d.id+'|'+this.uuid]=this;await call('notify',this._d.id,this._s,this.uuid);return this}}
 class Svc{constructor(d,u){this.device=d;this.uuid=u}
  async getCharacteristic(u){u=canon(u);if(!A.hasChar(this.device.id,this.uuid,u)){const e=new Error('No está la característica');e.name='NotFoundError';throw e}return new Chr(this.device,this.uuid,u)}}
 class Gatt{constructor(d){this.device=d;this.connected=false}
  async connect(){await call('connect',this.device.id);this.connected=true;return this}
  disconnect(){A.disconnect(this.device.id)}
  async getPrimaryService(u){u=canon(u);if(!this.connected||!A.hasService(this.device.id,u)){const e=new Error(this.connected?'No está el servicio':'GATT Server is disconnected');e.name=this.connected?'NotFoundError':'NetworkError';throw e}return new Svc(this.device,u)}}
 class Dev extends Ev{constructor(id,name){super();this.id=id;this.name=name;this.gatt=new Gatt(this);devs[id]=this}}
 function filtros(o){return JSON.stringify((o.filters||[]).map(f=>({name:f.name||'',namePrefix:f.namePrefix||'',services:(f.services||[]).map(canon)})))}
 const bt={
  async requestDevice(o){const r=await call('requestDevice',filtros(o||{}));return devs[r.id]||new Dev(r.id,r.name)},
  async getDevices(){return JSON.parse(A.getDevices()).map(r=>devs[r.id]||new Dev(r.id,r.name))},
  async getAvailability(){return true}
 };
 try{Object.defineProperty(navigator,'bluetooth',{value:bt,configurable:true})}catch(e){navigator.bluetooth=bt}
})();
