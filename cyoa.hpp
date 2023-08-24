#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "utils.hpp"

// Page class contains all the info about a single page                                                                            
class Page {
 public:
  std::string type;  // page type: N, W, L                                                                                         
  std::string file_name;
  std::size_t page_num;

  Page(){};
  void add(const std::string & p_type,
           const std::size_t & p_num,
           const std::string & f_name);
  friend class Story;
};

// Story class includes methods that parse, read from user, and process the story for all the steps (except step 4 which will inehrent and extend the functionality of this class)                                                                                   
class Story {
 protected:
  std::string story_dir;
  std::string story_fname;
  std::set<size_t> win_losePages;
  Page page;
  std::vector<Page> pages;

  class Choice {
    size_t src_page;
    size_t dest_page;
    std::string choice_str;

   public:
    void add(const size_t & src, const size_t & dest, const std::string & str);
    friend class Story;
  };

  Choice choice;
  std::vector<Choice> choices;
  std::vector<std::list<size_t> > pages_choices_adj;

 public:
  Story(){};
  Story(char * story_path) :
      story_dir(story_path), story_fname(get_filename(story_dir, "story.txt")) {}

  void parse_story();
  size_t add_page(const std::string & line,
                  const std::size_t & idx1,
                  const std::size_t & idx2,
                  const std::size_t & last_pageNum);
  void add_choice(const std::string & line,
                  const std::size_t & idx,
                  const size_t & curr_page);
  void verify_story();
  void print_story(bool user_input);
  size_t read_input(const size_t & choices_len,
                    const std::vector<std::pair<size_t, size_t> > & choices_map);
  std::vector<size_t> get_winPages(const std::vector<Page> & pages);
  std::vector<std::list<size_t> > adj_list(const std::vector<Page> & pages,
                                           const std::vector<Choice> & choices);
  std::vector<std::list<size_t> > BFS(const std::vector<std::list<size_t> > & adj_list,
                                      const std::list<size_t> & from,
                                      const size_t & to);
  std::vector<std::pair<size_t, std::vector<std::list<size_t> > > > find_winPaths();
  void print_winPaths(
      const std::vector<std::pair<size_t, std::vector<std::list<size_t> > > > &
          win_paths);
};

