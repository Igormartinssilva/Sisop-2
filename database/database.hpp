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

std::unordered_set<int> stringParaUnorderedSet(const std::string& input);

std::string unorderedSetParaString(const std::unordered_set<int>& mySet);

std::vector<twt::UserInfo> read_file(const std::string& filename);

std::string format_data(const twt::UserInfo& user);

void write_file(const std::string& filename, const std::vector<twt::UserInfo>& users);

//Falta implementar get_next_id, salvar e carregar da database

#endif // DATABASE_HPP
