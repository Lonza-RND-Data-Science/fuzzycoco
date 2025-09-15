#include "tests.h"
#include <sstream>
#include <regex>
#include "logging_logger.h"

using namespace fuzzy_coco;
using namespace logging;

TEST(Console_Logger, basic) {
  ostringstream oss;
  Console_Logger logger(L_null, oss);

  // by default logger is not activated
  logger << "coucou";
  EXPECT_TRUE(oss.str().empty());

  logger.activate();
  logger << "hello";
  EXPECT_EQ(oss.str(), "hello");
  
  logger.flush();
  EXPECT_EQ(oss.str(), "hello");

  oss.clear();
  logger << L_location;
  EXPECT_TRUE(regex_search(oss.str(), regex("logging_logger_test.cpp\\s*:\\s*\\d+")));
 
  oss.clear();
  logger << L_time;
  EXPECT_TRUE(regex_search(oss.str(), regex("\\d{4}-\\d{2}-\\d{2}")));
  // cerr << oss.str();


}

TEST(Logger, misc) {
  ostringstream oss;
  Console_Logger logger(L_null, oss);


  logger.activate();
  
  logger << L_flush << L_allwaysFlush << L_clearFlags 
    << L_concat << L_cout  << L_endl
    << L_location << L_null << L_startWithFlushing
    << L_tabs << L_time;

  {
    ostringstream oss;
    Console_Logger logger(L_startWithFlushing, oss);
    logger << L_endl;
  }

  EXPECT_FALSE(logger.open());

}

