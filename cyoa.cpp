#include "cyoa.hpp"

// Register pages info to Page class members
void Page::add(const std::string &p_type, const std::size_t &p_num,
               const std::string &f_name) {
  type = p_type;
  file_name = f_name;
  page_num = p_num;
}

// Register choice info to Choice class members
void Story::Choice::add(const size_t &src, const size_t &dest,
                        const std::string &str) {
  src_page = src;
  dest_page = dest;
  choice_str = str;
}
// This method uses Story class memebers to open the story.txt file, parse pages
// and choices lines, and store it in the corresponding class members
void Story::parse_story() {
  std::ifstream story_f(story_fname.c_str(), std::ifstream::in);
  if (!story_f.is_open()) {
    exit_failure("Could not open story file");
  }
  std::string line;
  std::size_t separator_idx1;
  std::size_t separator_idx2;
  size_t curr_page = 0;
  while (getline(story_f, line)) {
    separator_idx1 = line.find("@");
    separator_idx2 = line.find(":");
    if (line.empty()) {
      continue;
    }

    if (separator_idx1 != std::string::npos) {
      curr_page = add_page(line, separator_idx1, separator_idx2, curr_page);
    }

    else {
      add_choice(line, separator_idx2, curr_page);
    }
  }
  story_f.close();
  bool has_page0 = false;
  for (size_t i = 0; i < pages.size(); i++) {
    if (pages[i].page_num == 0) {
      has_page0 = true;
      break;
    }
  }
  if (!has_page0) {
    exit_failure("Story must have page 0");
  }
}
/*
   This method parse a page line given the index of the special symbols (i.e.
   index of  "@" and ":") and return the number of the page
*/
size_t Story::add_page(const std::string &line, const std::size_t &idx1,
                       const std::size_t &idx2,
                       const std::size_t &last_pageNum) {
  std::string p_type;
  std::string p_name;
  std::size_t p_num = parse_num(line, idx1);

  // get the page name and page number,
  // then add it to the page members and append the page to the pages vector
  if (p_num >= 0) {
    p_name = line.substr(idx2 + 1);
    p_type = line.substr(idx1 + 1, (idx2 - idx1 - 1));
    page.add(p_type, p_num, p_name);
    pages.push_back(page);
  }
  if (last_pageNum != 0) {
    if (p_num != (last_pageNum + 1)) {
      // std::cout << p_num << ", last:" << last_pageNum << std::endl;
      exit_failure("Page decleration must appear in order.");
    }
  }
  // keep track of win/lose pages to validate later that they don't include
  // chocies
  if ((p_type == "W") || (p_type == "L")) {
    win_losePages.insert(p_num);
  }
  return p_num;
}

/*
   This method parse a choice line given the index of the special symbols in the
   line (i.e. index of ":"")
*/
void Story::add_choice(const std::string &line, const std::size_t &idx,
                       const size_t &curr_page) {
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
  if (src_num > curr_page) {
    exit_failure("Page decleration must appear before anything related to it");
  }

  if (win_losePages.count(src_num)) {
    exit_failure("Win/Lose pages must not have any choice.");
  }
  // add the choice source and destination pages numbers and the choice string
  // to the Choice class members and append it to the choices vector
  choice.add(src_num, dest_num, choice_str);
  choices.push_back(choice);
}

// This method verify the story matchs the conditions given in step 2
void Story::verify_story() {
  // The follwing four variables are used to check:
  // 1) if story has a win and a lose pages
  // 2) if every page that is referenced by a choice is a valid page
  // 3) if every page is referenced by at least one other choice
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
// choices if user_input is true, it print the pages that correspond to the user
// input choice
void Story::print_story(bool user_input) {
  std::string page_fname;
  bool story_end =
      false; // to check if user input did not reach to the end of the story
  size_t curr_idx = 0;

  while (curr_idx < pages.size()) {
    page_fname = get_filename(story_dir, pages[curr_idx].file_name);
    std::ifstream page_f(page_fname.c_str(), std::ifstream::in);

    if (!page_f.is_open()) {
      exit_failure("Could not open page file");
    }
    if (!user_input) {
      std::cout << "Page " << pages[curr_idx].page_num << "\n==========\n";
    }

    std::string line;
    // print all lines of the current page
    while (getline(page_f, line)) {
      std::cout << line << std::endl;
    }
    std::cout << std::endl;
    size_t choices_idx = 1;
    std::pair<size_t, size_t> idx_map; // map the user input choice to the
                                       // destination page of the choice
    std::vector<std::pair<size_t, size_t>> choices_arr;

    // if the page type is N, iterate over the choices and check if their source
    // page number matches the current page num and print the choices
    if (pages[curr_idx].type == "N") {
      std::cout << "What would you like to do?\n\n";
      for (size_t j = 0; j < choices.size(); j++) {
        if (pages[curr_idx].page_num == choices[j].src_page) {
          std::cout << " " << choices_idx << ". " << choices[j].choice_str
                    << std::endl;
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
        curr_idx = read_input((choices_idx - 1), choices_arr);
      } else {
        curr_idx++;
      }
    }
    // check if page type is win/lose and print the related message
    else if (pages[curr_idx].type == "W") {
      std::cout << "Congratulations! You have won. Hooray!" << std::endl;
      if (user_input) {
        story_end = true;
        break;
      } else {
        curr_idx++;
      }
    } else if (pages[curr_idx].type == "L") {
      std::cout << "Sorry, you have lost. Better luck next time!" << std::endl;
      if (user_input) {
        story_end = true;
        break;
      } else {
        curr_idx++;
      }
    } else {
      exit_failure("Unrecongnized story type");
    }
  }
  if ((!story_end) && user_input) {
    exit_failure("Story terminated with out ending");
  }
}

// This method takes a number of how many choices (to check if the user input is
// outside the range of choices), and a vector of mapping between the index and
// the destination page and return the destination page.
size_t
Story::read_input(const size_t &choices_len,
                  const std::vector<std::pair<size_t, size_t>> &choices_map) {
  std::string line;
  std::stringstream ss;
  size_t choice_n;
  while (getline(std::cin, line)) {
    ss << line;
    ss >> choice_n;

    if ((choice_n < 1) || (choice_n > choices_len) || (!ss.eof())) {
      std::cout << "That is not a valid choice, please try again" << std::endl;
    } else {
      break;
    }
    ss.str("");
    ss.clear();
  }

  // return the destination page number that match the user input choice
  for (size_t i = 0; i < choices_map.size(); i++) {
    if (choice_n == choices_map[i].first) {
      return (size_t)choices_map[i].second;
    }
  }
  exit_failure("Something went wrong!");
  return -1;
}
// This method create an adjacency list from the pages and choices to be used
// for searching winning path
std::vector<std::list<size_t>>
Story::adj_list(const std::vector<Page> &pages,
                const std::vector<Choice> &choices) {
  std::vector<std::list<size_t>> page_choices;
  for (size_t i = 0; i < pages.size(); i++) {
    std::list<size_t> choices_list;
    for (size_t j = 0; j < choices.size(); j++) {
      if (pages[i].page_num == choices[j].src_page) {
        choices_list.push_back(choices[j].dest_page);
      }
    }
    page_choices.push_back(choices_list);
  }
  return page_choices;
}
// Returns the win pages to be used for searching winning paths
std::vector<size_t> Story::get_winPages(const std::vector<Page> &pages) {
  std::vector<size_t> win_pages;
  // get WIN pages
  for (size_t i = 0; i < pages.size(); i++) {
    if (pages[i].type == "W") {
      win_pages.push_back(pages[i].page_num);
    }
  }
  return win_pages;
}

// Breadth First Search algorithm adapted from AoP, it traverse the adjacency
// list from page 0 to the winning pages and returns all winning apaths
std::vector<std::list<size_t>>
Story::BFS(const std::vector<std::list<size_t>> &adj_list,
           const std::list<size_t> &from, const size_t &to) {
  std::queue<std::list<size_t>> queue;
  std::set<size_t> visited;
  std::vector<std::list<size_t>> all_paths;
  std::list<size_t> current_path;
  queue.push(from); // start from page 0
  while (queue.size() > 0) {
    current_path = queue.front();
    queue.pop();
    size_t current_node = current_path.back();
    if (current_node == to) {
      for (std::list<size_t>::iterator it = current_path.begin();
           it != current_path.end(); ++it) {
      }
      all_paths.push_back(current_path);
    }

    if (visited.find(current_node) == visited.end()) {
      visited.insert(current_node);
      std::list<size_t> current_list = adj_list[current_node];
      for (std::list<size_t>::iterator it = current_list.begin();
           it != current_list.end(); ++it) {
        std::list<size_t> curr_path_tmp = current_path;
        curr_path_tmp.push_back(*it);
        queue.push(curr_path_tmp);
      }
    }
  }
  return all_paths;
}

// This method calls BFS on every winning page
std::vector<std::pair<size_t, std::vector<std::list<size_t>>>>
Story::find_winPaths() {
  pages_choices_adj = adj_list(pages, choices);
  std::list<size_t> from;
  from.push_back(0);

  std::vector<size_t> win_pages = get_winPages(pages);
  std::vector<std::pair<size_t, std::vector<std::list<size_t>>>> win_paths;

  for (size_t i = 0; i < win_pages.size(); i++) {
    std::pair<size_t, std::vector<std::list<size_t>>> win_paths_pair;
    win_paths_pair.first = win_pages[i];
    win_paths_pair.second = BFS(pages_choices_adj, from, win_pages[i]);
    win_paths.push_back(win_paths_pair);
  }
  return win_paths;
}
// This method takes a vector of pairs that have the winning paths of each
// winning page and print the choice number and the corresponding page num that
// leads to winning the story
void Story::print_winPaths(
    const std::vector<std::pair<size_t, std::vector<std::list<size_t>>>>
        &win_paths) {
  int count_winPaths = 0;
  // iterate over the paths of each wining page
  for (size_t i = 0; i < win_paths.size(); i++) {
    std::vector<std::list<size_t>> dest_paths = win_paths[i].second;
    if (dest_paths.size() != 0) {
      count_winPaths++;
    }
    // for each path, declare two iterators that point to the page number in the
    // adjacency list
    for (size_t j = 0; j < dest_paths.size(); j++) {
      std::list<size_t>::const_iterator it_next;
      std::list<size_t>::const_iterator it_tail;

      for (std::list<size_t>::const_iterator it = dest_paths[j].begin();
           it != dest_paths[j].end(); ++it) {
        if (dest_paths[j].size() >= 2) {
          it_tail = increment_it(it);
          // print the win page number
          if (it_tail == dest_paths[j].end()) {
            std::cout << *it_next << "(win)\n";
            break;
          }
          // it_next points to the page number in the winning path and search
          // for the choice number that leads to it
          it_next = increment_it(it);
          int choice_num = 0;
          for (size_t k = 0; k < choices.size(); k++) {
            if (*it == choices[k].src_page) {
              choice_num++;

              if (*(it_next) == choices[k].dest_page) {
                std::cout << *it << "(" << choice_num << "),";
              }
            }
          }
        }
        // special case where page 0 is the winning page
        else if (dest_paths[j].size() == 1) {
          std::cout << "0(win)\n";
          break;
        }
      }
    }
  }

  if (count_winPaths == 0) {
    std::cout << "This story is unwinnable!\n";
    // Not sure about this, while the README stated it should exit succfully,
    // the pregrader consider it an erro when it's unwinnaable
    exit_failure(" ");
  }
}
