#include "header/interface.hpp"

Parser::Parser(const std::string& configFile) {
    std::ifstream file(configFile);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonString = buffer.str();

    parseJSON(jsonString);
}

void Parser::parseJSON(const std::string& jsonString) {
    // Implementação do parsing JSON
    // ...

    // Exemplo de como adicionar valores ao mapa config
    config["interfaces"]["menu_principal"]["descricao"] = "Descrição do Menu Principal";
    config["interfaces"]["escrever_mensagem"]["descricao"] = "Escrever Mensagem";
}

std::string Parser::extractValue(const std::string& json, const std::string& key) {
    size_t pos = json.find(key);
    if (pos != std::string::npos) {
        size_t start = json.find("\"", pos + key.length() + 3);
        size_t end = json.find("\"", start + 1);
        return json.substr(start + 1, end - start - 1);
    }
    return "";
}

void Parser::printDescription(const std::string& interface, const std::string& key) const {
    // Uso de std::any_cast para extrair valores de diferentes tipos
    try {
        std::cout << key << " of " << interface << ": " << std::any_cast<std::string>(config.at("interfaces").at(interface).at(key)) << std::endl;
    } catch (const std::bad_any_cast&) {
        std::cerr << "Error: Unable to cast value to string." << std::endl;
    }
}


Interface::Interface(){
    user = User();
}

Interface::Interface(int userId){
	user = User(); // TODO: syste.getUser();
}

void Interface::mainMenu(){
	
}
