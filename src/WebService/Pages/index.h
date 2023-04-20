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

		<h2>Flipper 32</h2>
		
        <form action="/" method="GET">
            <br>
			<button>Atualizar</button>
		</form>
		
        <form action="/IR" method="GET">
            <br>
			<button>Infra Vermelho</button>
		</form>
		
        <form action="/CONFIG" method="GET">
            <br>
			<button>Configurações</button>
		</form>
        
	</body>
</html>

)rawliteral";