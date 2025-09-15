#include "tests.h"
#include <cstdio>
#include <fstream>
#include <filesystem>

#include "file_utils.h"

using namespace fuzzy_coco;
using namespace std::filesystem;
using namespace FileUtils;


string CSV1 = 
R"(CASE;Sepal.Length;Sepal.Width;Petal.Length;Petal.Width;OUT
subject2;4.9;3;1.4;0.2;0
subject3;4.7;3.2;1.3;0.2;0
subject4;4.6;3.1;1.5;0.2;0
subject5;5;3.6;1.4;0.2;0
)";


TEST(utils, parseCSV_string) {
  vector<vector<string>> tokens;
  parseCSV(CSV1, tokens);

  EXPECT_EQ(tokens.size(), 5);
  for (auto line: tokens) {
    EXPECT_EQ(line.size(), 6);
  }
}

TEST(utils, parseCSV_file) {
  vector<vector<string>> tokens;

  // no file --> exception
  EXPECT_THROW(parseCSV(path("file_that_does_not.exist"), tokens), filesystem_error);

  path temp_filename = tmpnam(nullptr);
  {
    ofstream out(temp_filename);
    out << CSV1 << endl;
    out.close();
  }
  cerr << temp_filename << endl;

  parseCSV(temp_filename, tokens);

  vector<vector<string>> tokens_ref;
  parseCSV(CSV1, tokens_ref);

  EXPECT_EQ(tokens, tokens_ref);

  // ============= slurp
  string content = slurp(temp_filename);
  // N.B: content has an extra \n due to the last empty line
  content.resize(content.size() - 1);
  EXPECT_EQ(content, CSV1);

  remove(temp_filename);
}

TEST(utils, mkdir_if_needed) {
  path dirpath = temp_directory_path() / "mydir";
  path filepath = dirpath / "toto.txt";
  
  mkdir_if_needed(filepath);
  EXPECT_TRUE(is_directory(dirpath));

  EXPECT_NO_THROW(mkdir_if_needed(filepath));
  EXPECT_TRUE(is_directory(dirpath)); 

  remove(dirpath);
  EXPECT_FALSE(exists(dirpath));
}

TEST(utils, writeCSV) {
  DataFrame df(CSV1, true);
  ostringstream out;
  writeCSV(out, df);

  string csv = out.str();

  DataFrame df2(csv, true);
  cerr << df << df2;

}
