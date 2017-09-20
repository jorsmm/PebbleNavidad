module.exports = [
  { 
    "type": "heading", 
    "defaultValue": "Configuracion NAVIDAD"
  }, 
  { 
    "type": "text", 
    "defaultValue": "Hay que poner todas las URLs." 
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Servidor"
      },
      {
        "type": "input",
        "messageKey": "urlconsulta",
        "label": "URL de Consulta",
        "defaultValue": "http://192.168.0.2:18080/domo/iPHC-status.php?q=a3&b=true",
        "attributes": {
          "placeholder": "ej: http://192.168.0.2:18080/domo/iPHC-status.php?q=a3&b=true",
          "type": "text"
        }
      },
      {
        "type": "input",
        "messageKey": "urlnavon",
        "label": "URL Encender Navidad",
        "defaultValue": "http://192.168.0.2:18080/domo/d.php?q=ssm+navON",
        "attributes": {
          "placeholder": "ej: http://192.168.0.2:18080/domo/d.php?q=ssm+navON",
          "type": "text"
        }
      },
      {
        "type": "input",
        "messageKey": "urlnavoff",
        "label": "URL Apagar Navidad",
        "defaultValue": "http://192.168.0.2:18080/domo/d.php?q=ssm+navOFF",
        "attributes": {
          "placeholder": "ej: http://192.168.0.2:18080/domo/d.php?q=ssm+navOFF",
          "type": "text"
        }
      },
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Guardar"
  }
];
console.log("####Cargado config.js");