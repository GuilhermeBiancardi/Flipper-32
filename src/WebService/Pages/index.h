#include <WiFi.h>

char index_html[] PROGMEM = R"rawliteral(

<!DOCTYPE HTML>
<html>
	<head>
		<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
        <meta name="apple-mobile-web-app-capable" content="yes">
        <meta name="viewport" content="user-scalable=yes, initial-scale=1.0, maximum-scale=5.0, minimal-ui">
		<meta http-equiv="content-type" content="text/html; charset=UTF-8">
		<style>
            html {
                font-family: Arial;
                background-color: #202124;
                color: #c0c0c0;
            }
            h2 {
                text-align: center;
                font-size: 2em;
                color: #337ab7;
            }
            h5 {
                margin: 0 10px;
                font-size: 1em;
                padding: 0;
                border-bottom: 1px solid #c0c0c0;
            }
			input, button {
				padding: 10px;
                margin: 0 10px;
                font-size: 1.2em;
                border: 1px solid transparent;
                border-radius: 4px;
                
			}
            button{
                background-color: #337ab7;
                color: #fff;
            }
            form, fieldset {
                display: flex;
                flex-direction: column;
                flex-wrap: nowrap;
                align-content: center;
            }
            fieldset {
                border: 1px solid #9aa0a6;
                padding: 15px;
            }
            legend {
                font-size: 1.2em;
            }
            small {
                margin: 5px 10px;
                font-style: italic;
                font-size: 0.7em;
            }
		</style>
	</head>
	<body>

		<h2>Configurações do <span id="name-hardware">PlugWifi</span></h2>
		<form id="save-config" action="/GET" method="GET">
            
            <fieldset>
                <legend>Dados deste Dispositivo:</legend>
                <h5>Nome do Dispositivo:</h5>
                <small>Esse será o nome reconhecido pela Alexa, também será o nome da rede WiFi criada por esse dispositivo para conexão e configuração.</small>
                <input type="text" id="hardware" class="form-control" name="hardware" value="PlugWifi"/>
                <br>
                <h5>Senha do Dispositivo:</h5>
                <small>Senha da rede WiFi que esse dispositivo irá criar. ATENÇÃO: O mínimo são 8 caracteres.</small>
                <input type="text" id="passhard" class="form-control" name="passhard" value="12345678"/>
                <br>
                <h5>Seu IP Local:</h5>
                <small>Se este dispositivo estiver conectado em uma rede local o IP obtido aparecerá aqui, esse IP pode ser usado para configurar este dispositivo via rede, sem a necessidade de conexão com a rede WiFi criada por ele.</small>
                <input type="text" id="ip" class="form-control" name="ip" placeholder="IP Local" disabled/>
            </fieldset>
            
            <br>
            
            <fieldset>
                <legend>Dados da Rede WiFi</legend>
                <h5>Nome da Rede Wifi 2.4:</h5>
                <small>Nome de uma rede WiFi (SSID) para conexão, ATENÇÃO: A rede precisa ser 2.4Ghz, a configuração é case-sensitive ou seja diferencia maiúsculas e minúsculas, então certifique-se de que os dados estejam 100% iguais ao da rede.</small>
                <input type="text" id="wifissid" class="form-control" name="wifissid" placeholder="Nome da Rede Wifi"/>
                <br>
                <h5>Senha da Rede Wifi 2.4:</h5>
                <small>Senha da rede WiFi para conexão (também é case-sensitive).</small>
                <input type="password" id="password" class="form-control" name="password" placeholder="Senha da Rede Wifi"/>
            </fieldset>

			<br>
			<button>Salvar Informações</button>

		</form>
		
        <form action="/RESET" method="GET">
            <br>
			<button>Padrão de Fábrica</button>
		</form>
		
        <form action="/RESTART" method="GET">
            <br>
			<button>Reiniciar Dispositivo</button>
		</form>

        <script>

            document.getElementById("save-config").addEventListener('submit', function(e) {
                if(document.getElementById("hardware").value.length < 1) {
                    alert("O nome do Dispositivo precisa ter no mínimo 1 carctere.");
                    e.preventDefault();    
                }
                if(document.getElementById("passhard").value.length < 8) {
                    alert("A senha do Dispositivo precisa ter no mínimo 8 caracteres.");
                    e.preventDefault();
                }
            });

            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    if(this.responseText != "") {
                        document.getElementById("hardware").value = this.responseText;
                        document.getElementById("name-hardware").innerHTML = this.responseText;
                    }
                }
            };
            xhttp.open("GET", "/hardware", true);
            xhttp.send();

            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("wifissid").value = this.responseText;
                }
            };
            xhttp.open("GET", "/ssid", true);
            xhttp.send();

            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("password").value = this.responseText;
                }
            };
            xhttp.open("GET", "/password", true);
            xhttp.send();

            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("ip").value = this.responseText;
                }
            };
            xhttp.open("GET", "/ip", true);
            xhttp.send();

            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    if(this.responseText != "") {
                        document.getElementById("passhard").value = this.responseText;
                    }
                }
            };
            xhttp.open("GET", "/passhard", true);
            xhttp.send();
        
        </script>
	</body>
</html>

)rawliteral";