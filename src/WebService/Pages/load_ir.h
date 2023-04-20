#include <WiFi.h>

char load_ir_html[] PROGMEM = R"rawliteral(

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
		<h2>Esperando Sinal Infra Vermelho...</h2>
        <h5>Após o sinal ser lido você será redirecionado.</h5>
        
        <script>

            var interval = setInterval(function() {
                var xhttp = new XMLHttpRequest();
                xhttp.onreadystatechange = function() {
                    if (this.readyState == 4 && this.status == 200) {
                        if(this.responseText != "" && this.responseText == "1") {
                            clearInterval(interval);
                            window.location = "/IR";
                        }
                    }
                };
                xhttp.open("GET", "/IRSTATUS", true);
                xhttp.send();
            }, 500);
        
        </script>
	</body>
</html>

)rawliteral";