#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <optional>
#include <cctype>
#include <stdlib.h>

#include "helper.h"
#include "parser.h"




int main() {
    // Leer archivo de entrada
    std::ifstream file("pruebaParser.txt"); // Asegúrate de que el archivo exista
    if (!file.is_open()) {
        //std::cerr << "No se pudo abrir el archivo 'codigo_fuente.txt'." << std::endl;
        return 1;
    }

    // Leer el contenido del archivo en un buffer
    std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Instanciar el archivo fuente con el contenido del archivo .txt
    SourceFile sourceFile{"codigo_fuente.txt", buffer};

    std::cout << "INFO SCAN - Start scanning...\n";
    
    // Crear el lexer usando el archivo fuente
    Lexer lexer(sourceFile);  // Instancia del lexer principal

    // Imprimir la lista de tokens del lexer
    //lexer.printTokens();

    // Crear una copia del lexer para usar en el parser
    Lexer lexerCopy = lexer; // Creamos una copia del lexer original

    // Instanciar el parser con la copia del lexer
    Parser parser(lexerCopy); // Pasamos la copia al parser
    parser.parse(); // Ejecutar el parser para analizar la sintaxis del código fuente

    return 0;
}
