#include <SPI.h>
#include <SD.h>
class SDCardManager {
public:

    // Construtor da classe
    SDCardManager(int Pin): CSPin(Pin) {}

    /**
     * Função de configuração (setup) para inicializar o cartão SD.
     *
     * Essa função inicia o cartão SD utilizando a biblioteca SD.h do Arduino e verifica se a
     * inicialização foi bem-sucedida. Em caso de falha, uma mensagem de erro é exibida e a função
     * retorna. Caso contrário, é exibida uma mensagem indicando que o cartão SD está pronto para uso.
     *
     */
    void Setup() {
        // Inicializa o cartão SD utilizando o pino CS especificado (CSPin).
        if (SD.begin(CSPin, SPI, 4000000U, "/sd", 15U)) {
            Serial.println("SD Card pronto para uso.");
        } else {
            // Exibe uma mensagem de erro caso a inicialização do cartão SD falhe.
            Serial.println("Falha na inicialização do SD Card.");
            return;
        }
    }

    /**
     * Cria um arquivo no cartão SD.
     *
     * Caso o arquivo já exista essa função retornará false.
     *
     * @param file O nome do arquivo a ser escrito.
     * @param text O texto a ser escrito no arquivo.
     * @param delete_data Se true, caso o arquivo já exista, seu conteúdo é deletado.
     * @return true se a criação foi bem-sucedida, false caso contrário.
     *
     */
    bool FileCreate(const char* file, bool delete_data = false) {

        // Converte o nome do arquivo para inserir a barra / no inicio.
        char* FileName = SDCardFolderName(file);

        // Checa se o Arquivo já existe.
        if (!CheckExists(file)) {

            // Abre o arquivo no modo de escrita.
            SDCardFile = SD.open(FileName, FILE_WRITE);

            if (SDCardFile) {

                // Fecha o arquivo.
                SDCardFile.close();

                // Retorna true para indicar que a escrita foi bem-sucedida.
                return true;
            } else {
                // Retorna false caso não seja possível abrir o arquivo.
                return false;
            }

        } else {

            if (delete_data) {
                // Abre o arquivo no modo de escrita e zera seu conteúdo.
                SDCardFile = SD.open(FileName, FILE_WRITE);
                if (SDCardFile) {
                    SDCardFile.print("");
                    SDCardFile.close();
                }
            }

            // Retorna false arquivo já existe.
            return false;
        }

    }

    /**
     * Escreve um texto em um arquivo no cartão SD.
     *
     * @param file O nome do arquivo a ser escrito.
     * @param text O texto a ser escrito no arquivo.
     * @param mode Informe true para inserir o texto e pular uma linha no final.
     * @return bool Retorn true se a escrita foi bem-sucedida, false caso contrário.
     *
     */
    bool FileWrite(const char* file, const char* text, bool line_break = false) {

        // Checa se o Arquivo já existe.
        if (CheckExists(file)) {

            // Converte o nome do arquivo para inserir a barra / no inicio.
            char* FileName = SDCardFolderName(file);

            // Abre o arquivo no modo de escrita.
            SDCardFile = SD.open(FileName, FILE_APPEND);

            if (SDCardFile) {

                if (!line_break) {
                    // Escreve o texto no arquivo.
                    SDCardFile.print(text);
                } else {
                    // Escreve o texto no arquivo.
                    SDCardFile.println(text);
                }

                // Fecha o arquivo.
                SDCardFile.close();

                // Retorna true para indicar que a escrita foi bem-sucedida.
                return true;
            } else {
                // Retorna false caso não seja possível abrir o arquivo.
                return false;
            }

        } else {
            // Retorna false caso não seja possível abrir o arquivo.
            return false;
        }

    }

    /**
     * Função para ler o conteúdo de um arquivo no cartão SD.
     *
     * @param file O nome do arquivo a ser lido.
     * @return bool Uma string contendo o conteúdo do arquivo, ou uma string
     * vazia caso o arquivo não seja encontrado ou não possa ser lido.
     *
     */
    String FileRead(const char* file) {

        String text;

        // Checa se o Arquivo já existe.
        if (CheckExists(file)) {

            // Converte o nome do arquivo para inserir a barra / no inicio.
            char* FileName = SDCardFolderName(file);

            // Abre o arquivo no cartão SD para leitura.
            SDCardFile = SD.open(FileName);

            if (SDCardFile) {

                // Lê o conteúdo do arquivo linha por linha e adiciona à variável 'text'.
                while (SDCardFile.available()) {
                    text += (char) SDCardFile.read();
                }

                // Remove os espaços no começo e final da string
                text.trim();

                // Fecha o arquivo após a leitura.
                SDCardFile.close();

            }

        }

        // Retorna o conteúdo do arquivo lido, ou uma string vazia se o arquivo não for encontrado ou não puder ser lido.
        return text;

    }

    /**
     * Função para verificar se um arquivo ou diretório existe no cartão SD.
     *
     * @param path O caminho do arquivo ou diretório a ser verificado.
     * @return boll Retorna true se o arquivo ou diretório existe, false caso contrário.
     *
     */
    bool CheckExists(const char* path) {
        // Converte o nome do arquivo para inserir a barra / no inicio.
        char* PathName = SDCardFolderName(path);
        // Utiliza a função SD.exists para verificar se o arquivo ou diretório existe.
        return SD.exists(PathName);
    }

    /**
     * Função para remover um arquivo ou diretório do cartão SD.
     *
     * @param path O caminho do arquivo ou diretório a ser removido.
     * @return bool Retorna true se o arquivo ou diretório foi removido com sucesso, false caso contrário.
     *
     */
    bool Delete(const char* path) {

        // Checa se o Arquivo já existe.
        if (CheckExists(path)) {

            // Converte o nome do arquivo para inserir a barra / no inicio.
            char* PathName = SDCardFolderName(path);

            // Abre o arquivo ou diretório no cartão SD.
            SDCardFile = SD.open(PathName);

            // Verifica se é um diretório.
            if (SDCardFile.isDirectory()) {
                // Remove o diretório e seu conteúdo utilizando a função SD.rmdir.
                RemoveDirectory(PathName, (String)PathName);
                return SD.rmdir(PathName);
            } else {
                // Remove o arquivo utilizando a função SD.remove.
                return SD.remove(PathName);
            }

        } else {
            // Retorna false caso não seja possível abrir o arquivo.
            return false;
        }

    }

    /**
     * Função para renomear um arquivo ou diretório no cartão SD.
     *
     * @param name O nome atual do arquivo ou diretório.
     * @param newname O novo nome para o arquivo ou diretório.
     * @return bool Retorna true se o arquivo ou diretório foi renomeado com sucesso, false caso contrário.
     *
     */
    bool Rename(const char* name, const char* newname) {
        // Converte o nome do arquivo para inserir a barra / no inicio.
        char* filename = SDCardFolderName(name);

        // Verifica se já existe um arquivo com o novo nome.
        if (!CheckExists(newname)) {
            // Renomeia o arquivo utilizando a função SD.rename.
            return SD.rename(filename, newname);
        } else {
            // Retorna false se o novo nome já existir.
            return false;
        }
    }

    /**
     * Função para criar um novo diretório no cartão SD.
     *
     * @param foldername O nome do diretório a ser criado.
     * @return bool Retorna true se o diretório foi criado com sucesso, false caso contrário.
     *
     */
    bool FolderCreate(const char* foldername) {
        // Converte o nome do arquivo para inserir a barra / no inicio.
        char* PathFolderName = SDCardFolderName(foldername);

        // Verifica se o diretório já existe utilizando a função CheckExists.
        if (!CheckExists(PathFolderName)) {
            // Cria o diretório utilizando a função SD.mkdir.
            if (SD.mkdir(PathFolderName)) {
                return true;
            } else {
                return false;
            }
        } else {
            // Retorna false se o diretório já existir.
            return false;
        }
    }


    /**
     * Função para listar o conteúdo de um diretório no cartão SD.
     *
     * @param directory O caminho do diretório a ser listado.
     * @param tabSize O tamanho da tabulação para a formatação da saída.
     *
     */
    void ListDirectory(const char* directory, int tabSize) {
        // Abre o diretório no cartão SD.
        File ListDirectoryHome;
        ListDirectoryHome = SD.open(directory);

        // Imprime uma nova linha para separar a saída anterior, se houver.
        Serial.println("");

        // Chama a função List para listar o conteúdo do diretório, passando o tamanho da tabulação como parâmetro.
        List(ListDirectoryHome, tabSize);
    }


private:

    // Pino CS
    int CSPin;
    // Instancia da Classe File
    File SDCardFile;

    /**
     * Função para formatar o nome de um diretório no cartão SD incluindo uma / no início.
     *
     * @param foldername O nome do diretório a ser formatado.
     * @return O novo nome do diretório formatado como uma string de char.
     *
     */
    char* SDCardFolderName(const char* foldername) {

        // Adiciona uma barra "/" no início e um espaço " " no final do nome do diretório.
        String auxFolderName = "/" + String(foldername) + " ";

        // Aloca dinamicamente memória para a nova string de char do nome do diretório formatado.
        char* newFolderName = new char[auxFolderName.length() + 1];

        // Converte a string auxFolderName para um array de char e copia para newFolderName.
        auxFolderName.toCharArray(newFolderName, auxFolderName.length() + 1);

        // Retorna o novo nome do diretório formatado.
        return newFolderName;

    }

    /**
     * Função para remover arquivos e diretórios de forma recursiva em um diretório.
     *
     * @param dir O diretório a ser removido.
     * @param tempPath O caminho temporário para o arquivo/diretório atual.
     * @param rootpath O caminho raiz do diretório.
     *
     */
    void RemoveRecursive(File dir, String tempPath, String rootpath) {

        // Loop para percorrer todos os arquivos e diretórios no diretório.
        while (true) {

            // Abre o próximo arquivo
            File entry = dir.openNextFile();
            String localPath;

            Serial.println("");
            if (entry) {

                if (entry.isDirectory()) {
                    // Se for um diretório, chama o mesmo recursivamente.
                    localPath = tempPath + rootpath + entry.name() + " ";
                    char folderBuf[localPath.length() + 1];
                    localPath.toCharArray(folderBuf, localPath.length());
                    RemoveRecursive(entry, folderBuf, rootpath);

                    // Remove o diretório atual.
                    if (SD.rmdir(folderBuf)) {
                        Serial.print("Deleted folder ");
                        Serial.println(folderBuf);
                    } else {
                        Serial.print("Unable to delete folder ");
                        Serial.println(folderBuf);
                    }

                } else {
                    // Se for um arquivo, remove-o.
                    localPath = tempPath + rootpath + entry.name() + " ";
                    char charBuf[localPath.length() + 1];
                    localPath.toCharArray(charBuf, localPath.length());

                    if (SD.remove(charBuf)) {
                        Serial.print("Deleted ");
                        Serial.println(localPath);
                    } else {
                        Serial.print("Failed to delete ");
                        Serial.println(localPath);
                    }

                }

            } else {
                break;
            }
        }

    }

    /**
     * Função para chamar corretamente a função de delete recursivo.
     *
     * @param directory O diretório a ser removido.
     * @param path O caminho do diretório atual.
     *
     */
    void RemoveDirectory(const char* directory, String path) {
        File RemoveDirectory;
        RemoveDirectory = SD.open(directory);
        RemoveRecursive(RemoveDirectory, path, "/");
    }

    /**
     * Função para listar os arquivos e subdiretórios em um diretório de um cartão de memória SD.
     *
     * @param directory Um objeto da classe File que representa o diretório a ser listado.
     * @param tabSize O tamanho da tabulação para a impressão da listagem.
     *
     */
    void List(File directory, int tabSize) {
        while (true) {
            File entry = directory.openNextFile();
            if (!entry) {
                break;
            }
            for (uint8_t i = 0; i < tabSize; i++) {
                if (!entry.isDirectory()) {
                    Serial.print("|--- ");
                } else {
                    Serial.print("|    ");
                }
            }
            if (entry.isDirectory()) {
                Serial.print("| ");
                Serial.print(entry.name());
                Serial.println("/");
                List(entry, tabSize + 1);
            } else {
                Serial.print(entry.name());
                Serial.print(" Size: ");
                Serial.println(entry.size(), DEC);
            }
            entry.close();
        }
    }

};

/*

SD.begin(pinCS) // Inicia o cartão SD
SD.exists("filename") // Verifica se o arquivo existe
SD.open("filepath", mode) // Abre o arquivo do modo desejado
SD.close() // Fecha o arquivo atual
SD.remove("filename") // Apaga o arquivo
SD.mkdir("foldername") // Cria um diretório no cartão
SD.name() // Retorna o nome do arquivo atual
SD.print(data) // Escreve na mesma linha
SD.println(data) // Escreve e pula linha
SD.read() // Lê o arquivo
SD.avaliable() // Checa se existe espaço para escrita no arquivo

*/