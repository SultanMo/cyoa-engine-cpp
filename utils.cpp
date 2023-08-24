#include "utils.hpp"

// This function parse the file name directory
std::string get_filename(std::string path, std::string filename) {
  std::stringstream file_dir;
  file_dir << path << "/" << filename;
  return file_dir.str();
}

// This function prints the error message and exits the program with
// EXIT_FAILURE.
void exit_failure(std::string msg) {
  std::cerr << msg << std::endl;
  exit(EXIT_FAILURE);
}

std::list<size_t>::const_iterator
increment_it(std::list<size_t>::const_iterator it) {
  std::advance(it, 1);
  return it;
}
// This function parse a number with type string and return it in a long integer
long int parse_num(const std::string &str_num) {
  long int num;
  std::stringstream ss;
  ss << str_num;
  ss >> num;
  ss.str("");
  ss.clear();
  // iterate over every charecter to ensure it is a valid number that can be
  // stored in a long int
  if (num >= 0) {
    for (std::string::const_iterator it = str_num.begin(); it != str_num.end();
         ++it) {
      if (!isdigit(*it)) {
        exit_failure("Conditional choice must have a valid format");
      }
    }
  }
  return num;
}
// This is an overload parse_num function when there's a special seperator to
// parse
std::size_t parse_num(const std::string &line, const std::size_t &idx1) {
  size_t num;
  std::stringstream str;
  std::string num_s;
  num_s = line.substr(0, idx1);
  str << num_s;
  str >> num;
  if (num >= 0) {
    for (std::string::const_iterator it = num_s.begin(); it != num_s.end();
         ++it) {
      if (!isdigit(*it)) {
        exit_failure("Number must be a valid integer and greater than 0");
      }
    }
  }
  return num;
}
