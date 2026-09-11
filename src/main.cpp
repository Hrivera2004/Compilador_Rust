#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

std::string readFile(const std::string& fileName){
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cerr << fileName << ":0:0: error: no se pudo abrir el archivo\n";
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
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
