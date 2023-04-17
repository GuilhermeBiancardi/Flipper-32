#include <SPI.h>
#include <SD.h>

class SDCardManager {
public:

    //Constructor
    SDCardManager(int Pin) : CSPin(Pin) {}

    void Setup() {
        if (SD.begin(CSPin)) {
            Serial.println("SD Card pronto para uso.");
        } else {
            Serial.println("Falha na inicialização do SD Card.");
            return;
        }
    }

    bool Write(const char* file, const char* text) {

        char* FileName = SDCardFolderName(file);
        SDCardFile = SD.open(FileName, FILE_WRITE);

        if (SDCardFile) {
            SDCardFile.println(text);
            SDCardFile.close();
            return true;
        } else {
            return false;
        }

    }

    String Read(const char* file) {

        char* FileName = SDCardFolderName(file);
        String text;

        SDCardFile = SD.open(FileName);

        if (SDCardFile) {
            while (SDCardFile.available()) {
                text += (char)SDCardFile.read();
            }
            SDCardFile.close();
        }

        return text;

    }

    bool CheckExists(const char* path) {
        char* PathName = SDCardFolderName(path);
        return SD.exists(PathName);
    }

    bool Remove(const char* path) {
        char* PathName = SDCardFolderName(path);
        SDCardFile = SD.open(PathName);
        if (SDCardFile.isDirectory()) {
            RemoveDirectory(PathName, (String)PathName);
            return SD.rmdir(PathName);
        } else {
            return SD.remove(PathName);
        }
    }

    bool Rename(const char* name, const char* newname) {
        char* filename = SDCardFolderName(name);
        if (!CheckExists(newname)) {
            return SD.rename(filename, newname);
        } else {
            return false;
        }
    }

    bool FolderCreate(const char* foldername) {
        char* PathFolderName = SDCardFolderName(foldername);
        if (!CheckExists(PathFolderName)) {
            if (SD.mkdir(PathFolderName)) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    void ListDirectory(const char* directory, int tabSize) {
        File ListDirectoryHome;
        ListDirectoryHome = SD.open(directory);
        Serial.println("");
        List(ListDirectoryHome, tabSize);
    }

private:

    int CSPin;
    File SDCardFile;

    char* SDCardFolderName(const char* foldername) {
        String auxFolderName = "/" + String(foldername) + " ";
        char* newFolderName = new char[auxFolderName.length() + 1];
        auxFolderName.toCharArray(newFolderName, auxFolderName.length() + 1);
        return newFolderName;
    }

    void RemoveRecursive(File dir, String tempPath, String rootpath) {
        while (true) {
            File entry = dir.openNextFile();
            String localPath;

            Serial.println("");
            if (entry) {

                if (entry.isDirectory()) {

                    localPath = tempPath + rootpath + entry.name() + " ";
                    char folderBuf[localPath.length() + 1];
                    localPath.toCharArray(folderBuf, localPath.length());
                    RemoveRecursive(entry, folderBuf, rootpath);

                    if (SD.rmdir(folderBuf)) {
                        Serial.print("Deleted folder ");
                        Serial.println(folderBuf);
                        // FolderDeleteCount++;
                    } else {
                        Serial.print("Unable to delete folder ");
                        Serial.println(folderBuf);
                        // FailCount++;
                    }

                } else {

                    localPath = tempPath + rootpath + entry.name() + " ";
                    char charBuf[localPath.length() + 1];
                    localPath.toCharArray(charBuf, localPath.length());

                    if (SD.remove(charBuf)) {
                        Serial.print("Deleted ");
                        Serial.println(localPath);
                        // DeletedCount++;
                    } else {
                        Serial.print("Failed to delete ");
                        Serial.println(localPath);
                        // FailCount++;
                    }

                }

            } else {
                break;
            }
        }
    }

    void RemoveDirectory(const char* directory, String path) {
        File RemoveDirectory;
        RemoveDirectory = SD.open(directory);
        RemoveRecursive(RemoveDirectory, path, "/");
    }

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