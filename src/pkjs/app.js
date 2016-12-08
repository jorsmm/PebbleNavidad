// Called when the message send attempt succeeds
function messageSuccessHandler() {
  console.log("Message send succeeded.");  
}

// Called when the message send attempt fails
function messageFailureHandler() {
  console.log("Message send failed.");
}

// Called when JS is ready
Pebble.addEventListener("ready", function(e) {
  console.log("JS is ready!");
  toggleNavidad();
});
												
// Called when incoming message from the Pebble is received
// We are currently only checking the "message" appKey defined in appinfo.json/Settings
Pebble.addEventListener("appmessage", function(e) {
  console.log("Received Message: " + e.payload.message);
});

function toggleNavidad() {
  console.log("toggleNavidad");
  var urlConsulta='http://192.168.1.2:18080/domo/iPHC-status.php?q=a3&b=true';
  var urls={'on':'http://192.168.1.2:18080/domo/d.php?q=ssm+navOFF','off':'http://192.168.1.2:18080/domo/d.php?q=ssm+navON'};
  var json = null;

  xhrWrapper(urlConsulta, 'get', json, function(req) {
    if(req.status == 200) {
      var statusOrig=req.response.trim();
      console.log("respuestaCONSULTA="+statusOrig);
      xhrWrapper(urls[statusOrig], 'get', json, function(req) {
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
  xhr.open(type, url);
  if(data) {
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send(JSON.stringify(data));
  } else {
    xhr.send();
  }
}

function sendResultToPebble(result) {
    var iresult = result == 'off' ? 1 : 0;
    Pebble.sendAppMessage({
      'status': iresult
    }, messageSuccessHandler, messageFailureHandler);
}