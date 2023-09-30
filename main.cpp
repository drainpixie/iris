#include <algorithm>
#include <cstdlib>
#include <curses.h>
#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>

int LevenshteinDistance(std::string first, std::string second) {
  std::vector<std::vector<int>> matrix(first.size() + 1,
                                       std::vector<int>(second.size() + 1));

  for (std::string::size_type i = 0; i <= first.size(); i++) {
    matrix[i][0] = i;
  }

  for (std::string::size_type j = 0; j <= second.size(); j++) {
    matrix[0][j] = j;
  }

  for (std::string::size_type i = 1; i <= first.size(); i++) {
    for (std::string::size_type j = 1; j <= second.size(); j++) {
      int cost = first[i - 1] == second[j - 1] ? 0 : 1;

      matrix[i][j] = std::min({
          matrix[i - 1][j] + 1,       // deletion
          matrix[i][j - 1] + 1,       // insertion
          matrix[i - 1][j - 1] + cost // substitution
      });
    }
  }

  return matrix[first.size()][second.size()];
}

void RenderItems(const std::vector<std::string> &items,
                 const std::string &input) {
  std::vector<std::pair<int, std::string>> itemDistances;

  for (auto &&item : items) {
    int distance = LevenshteinDistance(item, input);
    itemDistances.push_back(std::make_pair(distance, item));
  }

  std::sort(
      itemDistances.begin(), itemDistances.end(),
      [](const std::pair<int, std::string> &a,
         const std::pair<int, std::string> &b) { return a.first < b.first; });

  clear();

  for (std::vector<std::pair<int, std::string>>::size_type i = 0;
       i < itemDistances.size(); i++) {
    if (i == 0)
      attron(A_REVERSE);

    printw("%d %s\n", itemDistances[i].first ,itemDistances[i].second.c_str());

    if (i == 0)
      attroff(A_REVERSE);
  }
}

int main() {
  initscr();

  cbreak();
  keypad(stdscr, true);
  curs_set(false);
  set_escdelay(0);

  int ch;
  int current = 0;

  std::string input;
  std::vector<std::string> words = {"Hello", "World", "Foo", "Bar",
                                    "Hallo", "Welt",  "Fee", "Baz"};

  while (true) {
    RenderItems(words, input);

    mvprintw(words.size(), 0, "find: %s", input.c_str());

    ch = getch();
    switch (ch) {
    case KEY_UP:
      current = std::max(0, current - 1);
      break;
    case KEY_DOWN:
      current = std::min(static_cast<int>(words.size()) - 1, current + 1);
      break;
    case 10: // enter, for some reason KEY_ENTER doesn't work
      endwin();
      std::cout << words[current] << std::endl;
      return EXIT_SUCCESS;
    case 27: // escape
      endwin();
      return EXIT_SUCCESS;
    case 127: // backspace
      if (!input.empty())
        input.pop_back();
      break;
    default:
      input.push_back(ch);
      break;
    }
  }

  return EXIT_SUCCESS;
}