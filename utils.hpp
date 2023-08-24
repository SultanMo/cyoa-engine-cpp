#include <cstdlib>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <string>

std::string get_filename(std::string path, std::string filename);
void exit_failure(std::string msg);
std::list<size_t>::const_iterator increment_it(std::list<size_t>::const_iterator it);
long int parse_num(const std::string & str_num);
std::size_t parse_num(const std::string & line, const std::size_t & idx1);