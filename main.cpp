#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <cassert>

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

int main() {
  return 0;
}