#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

std::ofstream outputFile;
std::ifstream sourceCode, configuration;

int main(int argc, char **argv) {
  if (argc < 2 || argc > 4) {
    if (argc < 2) {
      std::cerr
          << "You need to input a source file to the program as an argument."
          << std::endl;
    } else {
      std::cerr << "You can only input a source file, an output file and a "
                   "configuration file to the program."
                << std::endl;
    }
    return 1;
  }
  sourceCode =
      std::ifstream(std::filesystem::absolute(std::filesystem::path(argv[1]))
                        .generic_string());
  configuration = std::ifstream(
      std::filesystem::absolute(
          std::filesystem::path(argc == 4 ? argv[3]
                                          : "~/.config/fileCombiner/config"))
          .generic_string());
  outputFile = std::ofstream(
      std::filesystem::absolute(
          std::filesystem::path(argc >= 3 ? argv[2] : "./autoGenerated.cpp"))
          .generic_string());

  std::vector<std::filesystem::path> filesInOrder;
  std::set<std::string> definesToRemove;

  {
    std::string line;
    bool flag = false;
    while (std::getline(configuration, line)) {
      if (line.empty()) {
        flag = true;
        continue;
      }
      if (flag)
        definesToRemove.insert(line);
      else
        filesInOrder.push_back(
            std::filesystem::absolute(std::filesystem::path(line)));
    }
    configuration.close();
  }
  std::vector<std::string> output;
  for (auto const &file : filesInOrder) {
    std::ifstream curLibrary(file.generic_string());
    std::string line;
    while (getline(curLibrary, line)) {
      output.push_back(line);
    }
    curLibrary.close();
  }
  std::string line;
  while (getline(sourceCode, line)) {
    output.push_back(line);
  }
  sourceCode.close();
  for (auto const &x : output) {
    if (definesToRemove.count(x))
      continue;
    else
      outputFile << x << std::endl;
  }
}
