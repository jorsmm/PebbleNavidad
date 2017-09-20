var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);
var messageKeys = require('message_keys');

// settings
var urlconsulta="http://192.168.0.2:18080/domo/iPHC-status.php?q=a3&b=true";
var urlnavon="http://192.168.0.2:18080/domo/d.php?q=ssm+navON";
var urlnavoff="http://192.168.0.2:18080/domo/d.php?q=ssm+navOFF";

var configurandoDesdeElMovil=false;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Called when JS is ready
Pebble.addEventListener("ready", function(e) {
  console.log("JS is ready!");
  if (configurandoDesdeElMovil) {
    console.log("####Configurando desde el móvil esta app");
    sendMyMessage("settings=true");
    return;
  }
  sendMyMessage("init=true");
});

// Called when incoming message from the Pebble is received
// We are currently only checking the "message" appKey defined in appinfo.json/Settings
Pebble.addEventListener("appmessage", function(dict) {
  console.log("##Received from Watch: " + JSON.stringify(dict.payload));
  if(dict.payload.configurlconsulta) {
    urlconsulta=dict.payload.configurlconsulta;
  }
  if(dict.payload.configurlnavoff) {
    urlnavoff=dict.payload.configurlnavoff;
  }
  if(dict.payload.configurlnavon) {
    urlnavon=dict.payload.configurlnavon;
  }
  if(dict.payload.configure) {
    console.log("####Received configure message from Watch: " + dict.payload.configure);
    sendMyMessage("configured=true");
    if (configurandoDesdeElMovil) {
      console.log("####No envío aún toggleLuces porque esta configurando settings desde el movil");
    }
    else {
      toggleNavidad();
    }
  }
});

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function toggleNavidad() {
  console.log("toggleNavidad");
  var json = null;

  xhrWrapper(urlconsulta, 'get', json, function(req) {
    if(req.status == 200) {
      var statusOrig=req.response.trim();
      console.log("respuestaCONSULTA="+statusOrig);
      var urlaccion=statusOrig==='on'?urlnavoff:urlnavon;
      xhrWrapper(urlaccion, 'get', json, function(req) {
        if(req.status == 200) {
          console.log("respuestaEJEC="+req.response);
          sendResultToPebble(statusOrig);
        }
      });
    }
  });
}

function xhrWrapper(url, type, data, callback) {
  console.log("xhrWrapper("+url+","+type+","+data+",");
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(xhr);
  };
  xhr.timeout = 3000;
  xhr.ontimeout = function () {
    console.log("Error de TIMEOUT ["+url+"]");
    sendResultToPebble(false,true);
  };
  xhr.onerror = function () {
    console.log("Error en URL ["+url+"]");
    sendResultToPebble(false,true);
  };
  xhr.open(type, url);
  if(data) {
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send(JSON.stringify(data));
  } else {
    xhr.send();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Called when the message send attempt succeeds
function messageSuccessHandler() {
//  console.log("Message send succeeded.");
}

// Called when the message send attempt fails
function messageFailureHandler() {
  console.log("##### ERROR ##### 'Message send' failed.");
}

function sendMyMessage(mymessage) {
  console.log("Sending message to Watch: "+mymessage);
	Pebble.sendAppMessage({
    "message": mymessage
  }, messageSuccessHandler, messageFailureHandler);
}
function sendResultToPebble(result,huboerror) {
  var iresult = result ? 1 : 0;
  if (huboerror) {
    iresult=2;
  }
  Pebble.sendAppMessage({
    'status': iresult
  }, messageSuccessHandler, messageFailureHandler);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// called when config page is closed
Pebble.addEventListener('webviewclosed', function(e) {
  configurandoDesdeElMovil=false;
  console.log('webviewclosed (settings saved):'+JSON.stringify(e));
  if (e.response) {
    var claySettings = clay.getSettings(e.response);
    urlconsulta=claySettings[messageKeys.urlconsulta];
    console.log('URL Consulta ' + urlconsulta);
    urlnavon=claySettings[messageKeys.urlnavon];
    console.log('URL Navidad ON ' + urlnavon);
    urlnavoff=claySettings[messageKeys.urlnavoff];
    console.log('URL Navidad OFF ' + urlnavoff);
  }
  sendMyMessage("finish=true");
});
Pebble.addEventListener('showConfiguration', function(e) {
  configurandoDesdeElMovil=true;
  console.log('showConfiguration');
  Pebble.openURL(clay.generateUrl());
  sendMyMessage("settings=true");
});