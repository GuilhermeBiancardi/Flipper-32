#include <WiFi.h>

char ir_html[] PROGMEM = R"rawliteral(

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
			input, button, textarea {
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
		<form id="save-file" action="/IRSAVE" method="GET">
            
            <fieldset>
                <legend>Dados Infra Vermelho:</legend>
                <h5>Nome do Arquivo:</h5>
                <small>Nome do arquivo que armazenará os dados recebidos.</small>
                <input type="text" id="ir_nome" class="form-control" name="ir_nome" value=""/>
                <br>
                <h5>Dados Capturados:</h5>
                <small>Ultimo Sinal Infra Vermelho Capturado.</small>
                <textarea id="ir_code" class="form-control" name="ir_code" rows="10" disabled></textarea>
            </fieldset>
            
            <br>
            
			<button>Salvar Arquivo</button>

		</form>

        <br>

        <button id="emular-sinal">Emular Sinal</button>

        <form action="/LOADIR" method="GET">
            <br>
			<button>Capturar Sinal</button>
		</form>

        <form action="/IR" method="GET">
            <br>
			<button>Atualizar</button>
		</form>
		
        <form action="/" method="GET">
            <br>
			<button>Voltar para Home</button>
		</form>

        <script>

            document.getElementById("save-file").addEventListener('submit', function(e) {
                if(document.getElementById("ir_nome").value.length < 1) {
                    alert("O nome do Arquivo precisa ter no mínimo 1 carctere.");
                    e.preventDefault();    
                }
            });

            document.getElementById("emular-sinal").addEventListener('click', function(e) {
                if(document.getElementById("ir_nome").value.length < 1) {
                    alert("O nome do Arquivo precisa ter no mínimo 1 carctere.");
                    e.preventDefault();    
                } else {
                    var xhttp = new XMLHttpRequest();
                    xhttp.onreadystatechange = function() {
                        if (this.status == 200) {
                            if(this.responseText != "") {
                                alert(this.responseText);
                            }
                        }
                    };
                    xhttp.open("GET", "/IRGET", true);
                    xhttp.send();
                }
            });

            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    if(this.responseText != "") {
                        document.getElementById("ir_code").value = this.responseText;
                    }
                }
            };
            xhttp.open("GET", "/IRDATA", true);
            xhttp.send();
        
        </script>
	</body>
</html>

)rawliteral";