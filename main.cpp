#include <algorithm>
#include <curses.h>
#include <fcntl.h>
#include <iostream>
#include <ncurses.h>
#include <stddef.h>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

auto JaroWinklerDistance(std::string s1, std::string s2) {
  std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
  std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);

  if (s1 == s2)
    return 1.0;

  const auto m = s1.size();
  const auto n = s2.size();
  const auto max_distance = std::max(m, n) / 2 - 1;

  size_t match = 0;

  auto hash_m = std::string(m, '0');
  auto hash_n = std::string(n, '0');

  for (size_t i = 0; i < m; ++i) {
    for (size_t j =
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

  auto transpositions = 0;
  auto point = 0;

  for (size_t i = 0; i < m; ++i) {
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

auto InitializeScreen() {
  // initscr();
  newterm(NULL, stderr, stdin);

  noecho();
  cbreak();
  set_escdelay(0);
  curs_set(false);
  keypad(stdscr, true);
}

auto DisplayList(std::vector<std::string> &words, int &current,
                 std::string input) {
  auto max_display_items = LINES - 1; // 1 for the input line
  auto distances = std::vector<std::pair<double, size_t>>();

  for (size_t i = 0; i < words.size(); i++) {
    double distance = JaroWinklerDistance(words[i], input);
    distances.push_back(std::make_pair(distance, i));
  }

  std::sort(
      distances.begin(), distances.end(),
      [](const std::pair<double, size_t> a, const std::pair<double, size_t> b) {
        return b.first < a.first;
      });

  clear();

  for (size_t i = 0; i < distances.size(); i++) {
    // Uhh there's probably something better than static_cast
    if (i >= static_cast<size_t>(max_display_items))
      break;

    if (i == static_cast<size_t>(current))
      attron(A_REVERSE);

    printw("%f %s\n", distances[i].first, words[distances[i].second].c_str());

    if (i == static_cast<size_t>(current))
      attroff(A_REVERSE);
  }

  return distances;
}

auto DisplayInput(std::string input) {
  mvprintw(LINES - 1, 0, "find: %s", input.c_str());
}

auto CollectArgs(std::vector<std::string> &words, int argc, char *argv[]) {
  if (argc > 1) {
    for (int i = 1; i < argc; i++) {
      words.push_back(argv[i]);
    }
  } else {
    std::string line;

    while (std::getline(std::cin, line)) {
      words.push_back(line);
    }
  }

  close(STDIN_FILENO);

  auto fd = open("/dev/tty", O_RDONLY);
  if (fd < 0)
    exit(EXIT_FAILURE);

  auto result = dup2(fd, STDIN_FILENO);
  if (result < 0)
    exit(EXIT_FAILURE);
}

auto HandleInput(std::string &input,
                 std::vector<std::pair<double, size_t>> &distances,
                 std::vector<std::string> &words, int &current, int ch) {
  switch (ch) {
  case KEY_UP:
    current = std::max(0, current - 1);
    break;
  case KEY_DOWN:
    current = std::min(static_cast<int>(distances.size()) - 1, current + 1);
    break;
  case KEY_ENTER:
  case 10: // enter, for some reason KEY_ENTER doesn't work
    endwin();
    std::cout << words[distances[current].second] << std::endl;
    return true;
  case 27: // escape
    endwin();
    return true;
  case KEY_BACKSPACE:
  case KEY_DC:
  case 127: // backspace
    if (!input.empty())
      input.pop_back();
    break;
  default:
    input.push_back(ch);
    break;
  }

  return false;
}

int main(int argc, char *argv[]) {
  int ch, current = 0;

  std::string input;
  std::vector<std::string> words;

  for (size_t i = 0; i < words.size(); i++) {
    std::cout << words[i] << std::endl;
  }

  CollectArgs(words, argc, argv);
  InitializeScreen();

  while (true) {
    auto distances = DisplayList(words, current, input);
    DisplayInput(input);

    ch = getch();

    auto res = HandleInput(input, distances, words, current, ch);
    if (res)
      return EXIT_SUCCESS;
  }

  return EXIT_SUCCESS;
}
