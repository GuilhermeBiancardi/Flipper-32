# Filipper 32

Flipper 32 é um projeto com o objetivo de copiar parcialmente as funcionalidades de um Flipper Zero para a plataforma ESP32. 

1. **Tarefas relacionadas a ESP32**

        Plataforma Utilizada, ESP32 Wroom Dev Kit 38 Pinos:

     ![ESP32 Wroom Dev Kit](https://http2.mlstatic.com/D_NQ_NP_923346-MLB43359398569_092020-O.jpg)

   - [x] Criar SoftAP para conexão direta via WiFi
   - [x] Salvar dados customizaveis do SoftAP
   - [x] Salvar dados customizaveis do WiFi
   - [ ] Disponibilizar funções em forma de Classe
   - [ ] Obter as Telas diretamente do SDCard

2. **Tarefas relacionadas a NFC 13.56Mhz**

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
   - [ ] Integração com a UI do Sistema
   - [ ] Bruteforce de Keys

3. **Tarefas relacionadas ao SDCard**

        Módulo utilizado, Micro SD Card: 

     ![Micro SD Card](https://www.institutodigital.com.br/wp-content/uploads/2020/08/modulo-leitor-cartao-micro-sd-3.jpg)

   - [x] Criação de Arquivos
   - [x] Escrita de Arquivos
   - [x] Leitura de Arquivos
   - [x] Remoção de Arquivos
   - [x] Criação de Diretórios
   - [x] Remoção de Diretórios
   - [x] Listagem de Diretórios
   - [ ] Criar uma pasta padrão do sistema
   - [ ] Integração com a UI do Sistema
   
4. **Tarefas relacionadas ao Infra Vermelho**

        Peças utilizadas, Led Emissor e Receptor Tl1838 Vs1838 38khz: 

     ![Leds Infra Vermelho](https://cf.shopee.com.br/file/9df3b03281de181d8d3c050fa51ba083)

   - [ ] Captar Sinais
   - [ ] Emitir Sinais
   - [ ] Integração como o módulo SDCard
     - [ ] Salvar Sinais capturados
   - [ ] Integração com a UI do Sistema
   - [ ] Disponibilizar funções em forma de Classe
  
  
5. **Tarefas relacionadas a UI do Sistema**
   - [ ] Tela Inicial
   - [x] Tela de Loading
   - [x] Tela de Configuração do SoftAP
   - [x] Tela de Configuração do WiFi
   - [ ] Tela NFC
   - [ ] Tela Infra Vermelho
   - [ ] Migrar UI do Sistema do código para o SDCard. (Isso vai possibilitar desenvolver intervaces melhores sem comprometer o armazenamento limitado da ESP32, porém vai exigir que o dispositivo montado tenha um cartão Micro SD).