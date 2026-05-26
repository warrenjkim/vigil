#include "vigil/db/database.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/error.h"
#include "pulse/core/result.h"

namespace vigil {

namespace {

using ::testing::Eq;
using ::testing::SizeIs;
using ::testing::StrEq;

class DatabaseTest : public ::testing::Test {
 protected:
  void SetUp() override { db = *Database::Open(":memory:"); }

  Database db;
};

TEST_F(DatabaseTest, ExecuteBadSql) {
  pulse::Result<void> result = db.Execute(R"sql(invalid sql)sql");
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kInternal));
}

TEST_F(DatabaseTest, ExecuteBadBinding) {
  ASSERT_TRUE(
      db.Execute(R"sql(CREATE TABLE TestTable (Id INTEGER PRIMARY KEY);)sql")
          .ok());
  pulse::Result<void> result =
      db.Execute(R"sql(INSERT INTO TestTable (Id) VALUES (:id);)sql",
                 {{":nonexistent", 1}});
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kInternal));
}

TEST_F(DatabaseTest, ExecuteEmptyResult) {
  ASSERT_TRUE(
      db.Execute(R"sql(CREATE TABLE TestTable (Id INTEGER PRIMARY KEY);)sql")
          .ok());

  int call_count = 0;
  ASSERT_TRUE(
      db.Execute(R"sql(SELECT Id FROM TestTable;)sql", {}, [&call_count](int) {
          call_count++;
        }).ok());

  EXPECT_THAT(call_count, Eq(0));
}

TEST_F(DatabaseTest, ExecuteCreateAndQuery) {
  ASSERT_TRUE(
      db.Execute(
            R"sql(CREATE TABLE TestTable (Id INTEGER PRIMARY KEY, Name TEXT NOT NULL);)sql")
          .ok());
  ASSERT_TRUE(
      db.Execute(
            R"sql(INSERT INTO TestTable (Id, Name) VALUES (:id, :name);)sql",
            /*parameters=*/{{":id", 1}, {":name", "test"}})
          .ok());
  ASSERT_TRUE(
      db.Execute(
            R"sql(INSERT INTO TestTable (Id, Name) VALUES (2, "test_2");)sql")
          .ok());

  std::vector<std::pair<int, std::string>> rows;
  ASSERT_TRUE(
      db.Execute(
            R"sql(SELECT Id, Name FROM TestTable;)sql", /*parameters=*/{},
            [&rows](int id, std::string name) { rows.push_back({id, name}); })
          .ok());

  ASSERT_THAT(rows, SizeIs(2));
  EXPECT_THAT(rows[0].first, Eq(1));
  EXPECT_THAT(rows[0].second, StrEq("test"));
  EXPECT_THAT(rows[1].first, Eq(2));
  EXPECT_THAT(rows[1].second, StrEq("test_2"));
}

}  // namespace

}  // namespace vigil
