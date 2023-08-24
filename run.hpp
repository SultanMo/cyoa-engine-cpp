#include "cyoa.hpp"

// This is a story class that expand the function to work with choice variables
class CustomStory : public Story {
  class CChoice {
    size_t src_page;
    size_t dest_page;
    std::string choice_str;
    std::pair<std::string, long int> choice_var;

   public:
    void add(const size_t & src, const size_t & dest, const std::string & str);
    void add(const size_t & src,
             const size_t & dest,
             const std::string & str,
             const std::pair<std::string, long int> & var);
    friend class CustomStory;
  };
  CChoice choice;
  std::vector<CChoice> choices;
  std::map<size_t, std::map<std::string, int long> > all_vars;

 public:
  CustomStory(){};
  CustomStory(char * story_path) {
    story_dir = story_path;
    story_fname = get_filename(story_dir, "story.txt");
  }
  void parse_story();
  void add_choice(const std::string & line, const std::size_t & idx);
  void add_choice(const std::string & line,
                  const size_t & idx1,
                  const size_t & idx2,
                  const size_t & idx3);
  void add_variables(const std::string & line, const std::size_t & idx1);
  void verify_story();
  void print_story(bool user_input);
  size_t read_input(const size_t & choices_len,
                    const std::vector<std::pair<size_t, size_t> > & choices_map,
                    const std::vector<size_t> & unavailable_choices);
};
