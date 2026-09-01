#include <fstream>
#include <iostream>
#include <string>

String readFile(char* arg){
    const std::string fileName = arg;
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cerr << fileName << ":0:0: error: no se pudo abrir el archivo\n";
        return 1;
    }

    std::stringstream buffer;
    std::string file_contents = buffer.str();
    
    file.close();
    return file_contents;
}


int main(int argc, char* argv[]) {
   if (argc != 2) {
        std::cerr << "uso: " << argv[0] << " <archivo.txt>\n";
        return 1;
    }
    

    return 0;
}
