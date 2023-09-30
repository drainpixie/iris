#include <algorithm>
#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>

double JaroWinklerDistance(std::string s1, std::string s2) {
  std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
  std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);

  if (s1 == s2)
    return 1.0;

  const std::string::size_type m = s1.size();
  const std::string::size_type n = s2.size();
  const std::string::size_type max_distance = std::max(m, n) / 2 - 1;

  std::string::size_type match = 0;

  std::string hash_m(m, '0');
  std::string hash_n(n, '0');

  for (std::string::size_type i = 0; i < m; ++i) {
    for (std::string::size_type j =
             std::max(0, static_cast<int>(i) - static_cast<int>(max_distance));
         j < std::min(n, i + max_distance + 1); ++j) {

      // If there is a match
      if (s1[i] == s2[j] && hash_n[j] == '0') {
        hash_m[i] = '1';
        hash_n[j] = '1';
        match++;
        break;
      }
    }
  }

  if (match == 0)
    return 0.0;

  std::string::size_type transpositions = 0;
  std::string::size_type point = 0;

  for (std::string::size_type i = 0; i < m; ++i) {
    if (hash_m[i] == '1') {
      while (hash_n[point] == '0')
        point++;

      if (s1[i] != s2[point++])
        transpositions++;

      point++;
    }
  }

  transpositions /= 2;
  return (match / static_cast<double>(m) + match / static_cast<double>(n) +
          (match - transpositions) / static_cast<double>(match)) /
         3.0;
}

void InitializeScreen() {
  initscr();

  cbreak();
  keypad(stdscr, true);
  curs_set(false);
  set_escdelay(0);
}

void DisplayScreen(const std::vector<std::string> &items,
                   const std::string &input) {
  std::vector<std::pair<double, std::string>> itemDistances;

  for (auto &&item : items) {
    double distance = JaroWinklerDistance(item, input);
    itemDistances.push_back(std::make_pair(distance, item));
  }

  std::sort(itemDistances.begin(), itemDistances.end(),
            [](const std::pair<double, std::string> &a,
               const std::pair<double, std::string> &b) {
              return b.first < a.first;
            });

  clear();

  for (std::vector<std::pair<double, std::string>>::size_type i = 0;
       i < itemDistances.size(); i++) {
    if (i == 0)
      attron(A_REVERSE);

    printw("%f %s\n", itemDistances[i].first, itemDistances[i].second.c_str());

    if (i == 0)
      attroff(A_REVERSE);
  }
}

int main(int argc, char *argv[]) {
  InitializeScreen();

  int ch;
  int current = 0;

  std::string input;
  std::vector<std::string> words;

  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      words.push_back(argv[i]);
    }
  } else {
    std::string word;

    while (std::cin >> word) {
      words.push_back(word);
    }
  }

  while (true) {
    DisplayScreen(words, input);

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