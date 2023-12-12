#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <iterator>
#include "../common/header/data.hpp"

std::unordered_set<int> stringParaUnorderedSet( std::string& input);

std::string unorderedSetParaString( std::unordered_set<int>& mySet);

std::vector<twt::UserInfo> read_file( std::string& filename);

std::string format_data( twt::UserInfo& user);

void write_file( std::string& filename,  std::vector<twt::UserInfo>& users);

//Falta implementar get_next_id, salvar e carregar da database

#endif // DATABASE_HPP
