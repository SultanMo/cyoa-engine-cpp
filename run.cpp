#include "run.hpp"
// Register choices info Choice class members
void CustomStory::CChoice::add(const size_t &src, const size_t &dest,
                               const std::string &str) {
  src_page = src;
  dest_page = dest;
  choice_str = str;
  choice_var.first = "";
  // choice_var.clear();
}
// Overloaded add method with the extended functionality of step four
// (variables)
void CustomStory::CChoice::add(const size_t &src, const size_t &dest,
                               const std::string &str,
                               const std::pair<std::string, long int> &var) {
  src_page = src;
  dest_page = dest;
  choice_str = str;
  choice_var = var;
}

// This function uses Story class memebers to open the story.txt file, parse
// pages and choices lines, and store it in the corresponding class members.
// This perform similarly to the orignial method except it checks for the
// variables choice symbols
void CustomStory::parse_story() {
  std::ifstream story_f(story_fname.c_str(), std::ifstream::in);
  if (!story_f.is_open()) {
    exit_failure("Could not open story file");
  }
  std::string line;
  std::size_t separator_idx1;
  std::size_t separator_idx2;
  std::size_t separator_idx3;
  std::size_t separator_idx4;
  std::size_t separator_idx5;
  size_t curr_page = 0;

  while (getline(story_f, line)) {
    separator_idx1 = line.find("@");
    separator_idx2 = line.find(":");
    separator_idx3 = line.find("$");

    if (line.empty()) {
      continue;
    }
    // there are four situations for the story lines:
    // 1) page declaration
    if (separator_idx1 != std::string::npos) {
      curr_page = add_page(line, separator_idx1, separator_idx2, curr_page);
    }
    // 2) choice line
    else if (separator_idx2 != std::string::npos) {
      separator_idx4 = line.find("[");
      separator_idx5 = line.find("]");
      // 2.1) conditional choice line
      if ((separator_idx4 != std::string::npos) &&
          (separator_idx5 != std::string::npos)) {
        add_choice(line, separator_idx2, separator_idx4, separator_idx5);
      }
      // 2.2) original choice line
      else {
        add_choice(line, separator_idx2);
      }
    }
    // 3) variables declaration
    else if (separator_idx3 != std::string::npos) {
      add_variables(line, separator_idx3);
    }
  }
  story_f.close();
}
/*
   This method parse a choice line given the index of the special symbols (i.e.
   index of ":"")
*/
void CustomStory::add_choice(const std::string &line, const std::size_t &idx) {
  size_t src_num;
  size_t dest_num;
  std::stringstream str;

  int idx_last = line.find_last_of(":");
  std::string choice_str = line.substr(idx_last + 1);
  std::string src_page = line.substr(0, idx);
  std::string dest_page = line.substr(idx + 1, (idx_last - idx - 1));

  str << src_page << " " << dest_page;
  str >> src_num;
  str >> dest_num;

  choice.add(src_num, dest_num, choice_str);
  choices.push_back(choice);
}
/*
   This is an overloaded method that parse a conditional choice line given the
   index of the special symbols
*/
void CustomStory::add_choice(const std::string &line, const size_t &idx1,
                             const size_t &idx2, const size_t &idx3) {
  size_t src_num;
  size_t dest_num;
  std::stringstream str;

  int idx_last = line.find_last_of(":");

  std::string choice_str = line.substr(idx_last + 1);
  std::string src_page = line.substr(0, idx2);
  std::string var = line.substr(idx2 + 1, (idx3 - idx2 - 1));
  std::string dest_page = line.substr(idx1 + 1, (idx_last - idx1 - 1));

  str << src_page << " " << dest_page;
  str >> src_num;
  str >> dest_num;
  size_t var_idx = var.find("=");

  std::pair<std::string, long int> choice_var_pair;
  std::string var_num = var.substr(var_idx + 1);
  choice_var_pair.first = var.substr(0, var_idx);
  choice_var_pair.second = parse_num(var_num);
  // add the choice source and destination pages numbers and the choice string
  // to the Choice class members and append it to the choices vector
  choice.add(src_num, dest_num, choice_str, choice_var_pair);
  choices.push_back(choice);
}

// This method takes a line from the story and adds the variable to the chocie
// member
void CustomStory::add_variables(const std::string &line,
                                const std::size_t &idx1) {
  size_t p_num = parse_num(line, idx1);
  std::string var_name;
  std::string var_num;
  int idx2 = line.find("=");
  if (p_num >= 0) {
    var_name = line.substr(idx1 + 1, idx2 - 2);
    var_num = line.substr(idx2 + 1);
    all_vars[p_num][var_name] = parse_num(var_num);
  }
}

void CustomStory::verify_story() {
  bool win = false;
  bool lose = false;
  bool referenced_1 = false;
  bool referenced_2 = false;
  for (size_t i = 0; i < pages.size(); i++) {
    if (pages[i].type == "W") {
      win = true;
    } else if (pages[i].type == "L") {
      lose = true;
    }
    if (pages[i].page_num != 0) {
      for (size_t j = 0; j < choices.size(); j++) {
        if (pages[i].page_num == choices[j].dest_page) {
          referenced_1 = true;
          break;
        }
      }
    } else {
      continue;
    }
    if (!referenced_1) {
      exit_failure("Every choice must have a valid page");
    }
    referenced_1 = false;
  }
  if (!win || !lose) {
    exit_failure("Story file must include at least one WIN and one LOSE page");
  }

  for (size_t i = 0; i < choices.size(); i++) {
    for (size_t j = 0; j < pages.size(); j++) {
      if (choices[i].dest_page == pages[j].page_num) {
        referenced_2 = true;
        break;
      }
    }
    if (!referenced_2) {
      exit_failure("Every page must be referenced by a choice");
    }
    referenced_2 = false;
  }
}

// This method iterate over the story file and print the story pages
// if user input is false, it print all the mentiond pages along with their
// choices and according to step 4 condtional choices rules if user_input is
// true, it print the pages that correspond to the user input choice
void CustomStory::print_story(bool user_input) {
  std::string page_fname;
  size_t curr_idx = 0;
  std::map<std::string, int long> store_vars; // this track the variables that
                                              // appeared until the current page
  while (curr_idx < pages.size()) {
    page_fname = get_filename(story_dir, pages[curr_idx].file_name);
    std::ifstream page_f(page_fname.c_str(), std::ifstream::in);
    if (!page_f.is_open()) {
      exit_failure("Could not open page file");
    }
    std::string line;
    if (!user_input) {
      std::cout << "Page " << pages[curr_idx].page_num << "\n==========\n";
    }
    while (getline(page_f, line)) {
      std::cout << line << std::endl;
    }
    std::cout << std::endl;
    size_t choices_idx = 1;
    std::pair<size_t, size_t> idx_map;
    std::vector<std::pair<size_t, size_t>> choices_arr;

    if (pages[curr_idx].type == "N") {
      size_t curr_pageNumber = pages[curr_idx].page_num;
      if (all_vars.find(curr_pageNumber) != all_vars.end()) {
        for (std::map<std::string, long int>::const_iterator it =
                 all_vars[curr_pageNumber].begin();
             it != all_vars[curr_pageNumber].end(); ++it) {
          store_vars[it->first] = it->second;
        }
      }

      std::cout << "What would you like to do?\n\n";
      std::vector<size_t>
          unavailable_choices; // keep track of unavailable choices to be
                               // rejected when entered by the user
      for (size_t j = 0; j < choices.size(); j++) {
        if (pages[curr_idx].page_num == choices[j].src_page) {
          // print choice for the standard case (without variables)
          if (choices[j].choice_var.first == "") {
            std::cout << choices_idx << "." << choices[j].choice_str
                      << std::endl;
          } else {
            // print the choice when the condition is met or it has not been set
            if (store_vars[choices[j].choice_var.first] ==
                choices[j].choice_var.second) {
              std::cout << choices_idx << "." << choices[j].choice_str
                        << std::endl;
            }
            // print unavailable when it the choice didn't match the condition
            else {
              std::cout << choices_idx << ".<UNAVAILABLE>" << std::endl;
              unavailable_choices.push_back(choices_idx);
            }
          }
          idx_map.first = choices_idx;
          idx_map.second = choices[j].dest_page;
          choices_arr.push_back(idx_map);
          choices_idx++;
        }
      }
      // when there is a user_input, we update the printed page from the choices
      // selected by the user. Otherwise, increment curr_idx to print the next
      // page
      if (user_input) {
        curr_idx =
            read_input((choices_idx - 1), choices_arr, unavailable_choices);
      } else {
        curr_idx++;
      }
    }
    // check if page type is win/lose and print the related message
    else if (pages[curr_idx].type == "W") {
      std::cout << "Congratulations! You have won. Hooray!" << std::endl;
      if (user_input) {
        break;
      } else {
        curr_idx++;
      }
    } else if (pages[curr_idx].type == "L") {
      std::cout << "Sorry, you have lost. Better luck next time!" << std::endl;
      if (user_input) {
        break;
      } else {
        curr_idx++;
      }
    } else {
      exit_failure("Unrecongnized story type");
    }
  }
}

// This method takes a number of how many choices (to check if the user input is
// outside the range of choices), and a vector of mapping between the index and
// the destination page and return the destination page.
size_t CustomStory::read_input(
    const size_t &choices_len,
    const std::vector<std::pair<size_t, size_t>> &choices_map,
    const std::vector<size_t> &unavailable_choices) {
  std::string line;
  std::stringstream ss;
  int choice_n;
  while (getline(std::cin, line)) {
    std::string n = line.substr(0, line.find(" "));
    ss << n;
    ss >> choice_n;
    // the number selected must be in a valid range or it's an avalilable
    // choice, otherwise ask for input again
    if ((choice_n < 1) || (choice_n > (int)choices_len)) {
      std::cout << "That is not a valid choice, please try again" << std::endl;
    } else if (std::find(unavailable_choices.begin(), unavailable_choices.end(),
                         choice_n) != unavailable_choices.end()) {
      std::cout
          << "That choice is not available at this time, please try again\n";
    } else {
      break;
    }
    ss.str("");
    ss.clear();
  }

  // return the destination page number that match the user input choice
  for (size_t i = 0; i < choices_map.size(); i++) {
    if (choice_n == (int)choices_map[i].first) {
      return (size_t)choices_map[i].second;
    }
  }
  exit_failure("check why choice not found in map");
  return -1;
}

int main(int argc, char **argv) {
  // if (argc != 2) {
  //   exit_failure("Program must provide story directory");
  // }

  CustomStory story("story");
  story.parse_story();
  story.verify_story();
  story.print_story(true);

  return EXIT_SUCCESS;
}
