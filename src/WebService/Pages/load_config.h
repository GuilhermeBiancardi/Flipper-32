#include <WiFi.h>

char load_config_html[] PROGMEM = R"rawliteral(

<!DOCTYPE HTML>
<html>
	<head>
		<meta name="viewport" content="user-scalable=yes, initial-scale=1.0, maximum-scale=5.0, minimal-ui">
		<meta http-equiv="content-type" content="text/html; charset=UTF-8">
		<style>
			html {
                font-family: Arial;
                display: inline-block;
                margin: 0px auto;
                text-align: center;
                background-color: #202124;
                color: #c0c0c0;
                font-size: 2em;
			}
		</style>
	</head>
	<body>
		<h2>Aguarde...</h2>
        <h5>Após a conclusão do processo o dispositivo será reiniciado.</h5>
        <script>

            setTimeout(function () {
                var xhttp = new XMLHttpRequest();
                xhttp.open("GET", "/reload", true);
                xhttp.send();
            }, 5000 ) ;

            setTimeout(function () {
                window.location = "/";
            }, 7000 );
        
        </script>
	</body>
</html>

)rawliteral";