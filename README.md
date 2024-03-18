# Filipper 32

Flipper 32 é um projeto com o objetivo de copiar parcialmente as funcionalidades de um Flipper Zero para a plataforma ESP32. 

      ATENÇÃO: A UI do projeto está na raiz dentro da pasta SDCard. Para que o projeto funcione
      corretamente todo o conteúdo dessa pasta deve ser copiado para o SDCard!
      ATENÇÃO: Somente o conteúdo da pasta SDCard deve ser movido, a pasta SDCard não deverá ser copiada.

0. **Primeiro Acesso**

      Após liga-lo pela primeira vez uma rede WiFi com o nome Fliper32 aparecerá, conecte nesta rede com a senha 12345678, feito isso
      abra um navegador e digite 192.168.4.1 na barra de pesquisa e espere os arquivos carregarem no CACHE, clique em Config e
      mude os dados de conexão local ou configure dados de WiFi para acessar o Fliper32 em uma rede local.

1. **Tarefas Gerais**
   
   - [ ] Criar esquemático com as ligações.
   - [ ] Desenvolver GERBER da placa de circuito impresso
   - [ ] Desenvolver modelo 3D da case do Projeto

2. **Tarefas relacionadas a ESP32**

        Plataforma Utilizada, ESP32 Wroom Dev Kit 38 Pinos:

     ![ESP32 Wroom Dev Kit](https://http2.mlstatic.com/D_NQ_NP_923346-MLB43359398569_092020-O.jpg)

   - [x] Criar SoftAP para conexão direta via WiFi
   - [x] Salvar dados customizaveis do SoftAP
   - [x] Salvar dados customizaveis do WiFi
   - [ ] Disponibilizar funções em forma de Classe
   - [x] Obter as Telas diretamente do SDCard
   -  - [x] Resolver problema que reinicia a ESP ao carregar muitos arquivos.
   - [x] Criar websocket para comunicação em tempo real com o webservice.

3. **Tarefas relacionadas a NFC 13.56Mhz**

        Módulo utilizado, PN532: 

     ![PN532](https://soldered.com/productdata/2023/01/dsc_5091_1-1024x683-1.jpg)

   - [x] Disponibilizar funções em forma de Classe
   - [x] Leitura da Tag
   - [x] Escrita na Tag
   - [ ] Clonar Tag
   - [ ] Emulação de Tag (Verificar viabilidade)
   - [ ] Integração com o módulo SDCard
     - [ ] Salvar uma lista de Keys conhecidas
     - [ ] Salvar Tags lidas
   - [x] Integração com a UI do Sistema
   - [ ] Bruteforce de Keys
   - [x] Importar dados da Tag em JSON

4. **Tarefas relacionadas ao SDCard**

        Módulo utilizado, Micro SD Card: 

     ![Micro SD Card](https://www.institutodigital.com.br/wp-content/uploads/2020/08/modulo-leitor-cartao-micro-sd-3.jpg)

   - [x] Criação de Arquivos
   - [x] Escrita de Arquivos
   - [x] Leitura de Arquivos
   - [x] Remoção de Arquivos
   - [x] Criação de Diretórios
   - [x] Remoção de Diretórios
   - [x] Listagem de Diretórios
   - [x] Criar uma pasta padrão do sistema
   - [x] Integração com a UI do Sistema
   
5. **Tarefas relacionadas ao Infra Vermelho**

        Peças utilizadas, Led Emissor e Receptor Tl1838 Vs1838 38khz: 

     ![Leds Infra Vermelho](https://cf.shopee.com.br/file/9df3b03281de181d8d3c050fa51ba083)

   - [x] Captar Sinais
   - [x] Emitir Sinais
   - [x] Clonar Sinais
   - [x] Integração como o módulo SDCard
     - [x] Salvar Sinais capturados
   - [x] Integração com a UI do Sistema
   - [x] Disponibilizar funções em forma de Classe
  
  
5. **Tarefas relacionadas a UI do Sistema**
   - [x] Remover UI antiga do código
   - [ ] Tela Inicial
     - [ ] Adicionar informações importantes da Placa.
     - [ ] Adicionar informações importantes do Leitor NFC
     - [ ] Adicionar informações importantes do Leitor Infra-Vermelho
   - [x] Tela de Loading (Cache)
   - [x] Tela de Configuração do SoftAP
     - [x] Mudar SSID
     - [x] Mudar Senha
   - [x] Tela de Configuração do WiFi
     - [x] Mudar SSID
     - [x] Mudar Senha
   - [ ] Tela NFC
     - [x] Ler dados da tag NFC
     - [x] Gravar dados na tag NFC
     - [x] Listar arquivos salvos no cartão de memória
     - [ ] Salvar dados lidos em arquivo no cartão de memória
     - [ ] Ler arquivos salvos no cartão de memória
     - [ ] Definir key para leitura.
   - [ ] Tela Infra Vermelho
     - [ ] Ler sinais IR
     - [ ] Simular sinais IR
     - [ ] Listar arquivos salvos no cartão de memória
     - [ ] Salvar dados lidos em arquivo no cartão de memória
     - [ ] Ler arquivos salvos no cartão de memória
   - [x] Migrar UI integrada para o SDCard. (Isso vai possibilitar desenvolver interfaces melhores sem comprometer o armazenamento limitado da memória interna da ESP32, porém vai exigir que o dispositivo montado tenha um cartão Micro SD).