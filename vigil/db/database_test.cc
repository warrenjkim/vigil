#include "vigil/db/database.h"

#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pulse/core/error.h"
#include "pulse/core/result.h"
#include "pulse/core/result_or_die.h"

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

TEST_F(DatabaseTest, IntegerColumnNonNull) {
  ASSERT_TRUE(
      db.Execute(
            R"sql(CREATE TABLE TestTable (Id INTEGER PRIMARY KEY, Value INTEGER);)sql")
          .ok());
  ASSERT_TRUE(
      db.Execute(
            R"sql(INSERT INTO TestTable (Id, Value) VALUES (:id, :value);)sql",
            /*parameters=*/{{":id", 1}, {":value", 42}})
          .ok());

  int value = -1;
  ASSERT_TRUE(db.Execute(R"sql(SELECT Value FROM TestTable WHERE Id = 1;)sql",
                         /*parameters=*/{}, [&value](int v) { value = v; })
                  .ok());

  EXPECT_THAT(value, Eq(42));
}

TEST_F(DatabaseTest, IntegerColumnNull) {
  ASSERT_TRUE(
      db.Execute(
            R"sql(CREATE TABLE TestTable (Id INTEGER PRIMARY KEY, Value INTEGER);)sql")
          .ok());
  ASSERT_TRUE(
      db.Execute(
            R"sql(INSERT INTO TestTable (Id, Value) VALUES (:id, :value);)sql",
            /*parameters=*/{{":id", 1}, {":value", std::optional<int>{}}})
          .ok());

  std::optional<int> value = -1;
  ASSERT_TRUE(db.Execute(R"sql(SELECT Value FROM TestTable WHERE Id = 1;)sql",
                         /*parameters=*/{},
                         [&value](std::optional<int> v) { value = v; })
                  .ok());

  EXPECT_FALSE(value.has_value());
}

TEST_F(DatabaseTest, TextColumnNonNull) {
  ASSERT_TRUE(
      db.Execute(
            R"sql(CREATE TABLE TestTable (Id INTEGER PRIMARY KEY, Value TEXT);)sql")
          .ok());
  ASSERT_TRUE(
      db.Execute(
            R"sql(INSERT INTO TestTable (Id, Value) VALUES (:id, :value);)sql",
            /*parameters=*/{{":id", 1}, {":value", std::string("hello")}})
          .ok());

  std::string value;
  ASSERT_TRUE(db.Execute(R"sql(SELECT Value FROM TestTable WHERE Id = 1;)sql",
                         /*parameters=*/{},
                         [&value](std::string v) { value = std::move(v); })
                  .ok());

  EXPECT_THAT(value, StrEq("hello"));
}

TEST_F(DatabaseTest, TextColumnNull) {
  ASSERT_TRUE(
      db.Execute(
            R"sql(CREATE TABLE TestTable (Id INTEGER PRIMARY KEY, Value TEXT);)sql")
          .ok());
  ASSERT_TRUE(
      db.Execute(
            R"sql(INSERT INTO TestTable (Id, Value) VALUES (:id, :value);)sql",
            /*parameters=*/{{":id", 1},
                            {":value", std::optional<std::string>{}}})
          .ok());

  std::optional<std::string> value = "sentinel";
  ASSERT_TRUE(db.Execute(R"sql(SELECT Value FROM TestTable WHERE Id = 1;)sql",
                         /*parameters=*/{},
                         [&value](std::optional<std::string> v) { value = v; })
                  .ok());

  EXPECT_FALSE(value.has_value());
}

TEST_F(DatabaseTest, DoubleColumnNonNull) {
  ASSERT_TRUE(
      db.Execute(
            R"sql(CREATE TABLE TestTable (Id INTEGER PRIMARY KEY, Value REAL);)sql")
          .ok());
  ASSERT_TRUE(
      db.Execute(
            R"sql(INSERT INTO TestTable (Id, Value) VALUES (:id, :value);)sql",
            /*parameters=*/{{":id", 1}, {":value", 3.14}})
          .ok());

  double value = -1.0;
  ASSERT_TRUE(db.Execute(R"sql(SELECT Value FROM TestTable WHERE Id = 1;)sql",
                         /*parameters=*/{}, [&value](double v) { value = v; })
                  .ok());

  EXPECT_THAT(value, Eq(3.14));
}

TEST_F(DatabaseTest, DoubleColumnNull) {
  ASSERT_TRUE(
      db.Execute(
            R"sql(CREATE TABLE TestTable (Id INTEGER PRIMARY KEY, Value REAL);)sql")
          .ok());
  ASSERT_TRUE(
      db.Execute(
            R"sql(INSERT INTO TestTable (Id, Value) VALUES (:id, :value);)sql",
            /*parameters=*/{{":id", 1}, {":value", std::optional<double>{}}})
          .ok());

  std::optional<double> value = -1.0;
  ASSERT_TRUE(db.Execute(R"sql(SELECT Value FROM TestTable WHERE Id = 1;)sql",
                         /*parameters=*/{},
                         [&value](std::optional<double> v) { value = v; })
                  .ok());

  EXPECT_FALSE(value.has_value());
}

TEST_F(DatabaseTest, InitializeAppliesSchema) {
  Database db = pulse::unwrap_or_die(Database::Open(":memory:"));
  ASSERT_TRUE(db.Initialize().ok());

  int count = -1;
  ASSERT_TRUE(db.Execute("SELECT COUNT(*) FROM Accounts;", {}, [&count](int c) {
                  count = c;
                }).ok());
  EXPECT_THAT(count, Eq(0));

  int version = -1;
  ASSERT_TRUE(db.Execute("PRAGMA user_version;", {}, [&version](int v) {
                  version = v;
                }).ok());
  EXPECT_THAT(version, Eq(1));
}

TEST_F(DatabaseTest, InitializeIsIdempotent) {
  Database db = pulse::unwrap_or_die(Database::Open(":memory:"));
  ASSERT_TRUE(db.Initialize().ok());
  EXPECT_TRUE(db.Initialize().ok());
}

TEST_F(DatabaseTest, RejectsUnknownVersion) {
  Database db = pulse::unwrap_or_die(Database::Open(":memory:"));
  ASSERT_TRUE(db.Execute("PRAGMA user_version = 99;").ok());

  pulse::Result<void> result = db.Initialize();
  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kInternal));
}

class WrapInTransactionTest : public ::testing::Test {
 protected:
  void SetUp() override {
    db_ = pulse::unwrap_or_die(Database::Open(":memory:"));
    ASSERT_TRUE(db_.Execute(R"sql(
        CREATE TABLE TestTable (Id INTEGER PRIMARY KEY, Value TEXT NOT NULL);
    )sql")
                    .ok());
  }

  Database db_;
};

TEST_F(WrapInTransactionTest, CommitsOnSuccess) {
  pulse::Result<void> result =
      WrapInTransaction(&db_, [this]() -> pulse::Result<void> {
        return db_.Execute(
            R"sql(INSERT INTO TestTable (Id, Value) VALUES (1, 'hello');)sql");
      });

  ASSERT_TRUE(result.ok());

  int count = 0;
  ASSERT_TRUE(db_.Execute("SELECT COUNT(*) FROM TestTable;", {},
                          [&count](int c) { count = c; })
                  .ok());
  EXPECT_THAT(count, Eq(1));
}

TEST_F(WrapInTransactionTest, RollsBackOnFailure) {
  pulse::Result<void> result =
      WrapInTransaction(&db_, [this]() -> pulse::Result<void> {
        if (pulse::Result<void> err = db_.Execute(
                R"sql(INSERT INTO TestTable (Id, Value) VALUES (1, 'hello');)sql");
            !err.ok()) {
          return err;
        }

        return pulse::Error{.code = pulse::Error::Code::kInternal,
                            .message = "forced failure"};
      });

  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kInternal));

  int count = 0;
  ASSERT_TRUE(db_.Execute("SELECT COUNT(*) FROM TestTable;", {},
                          [&count](int c) { count = c; })
                  .ok());
  EXPECT_THAT(count, Eq(0));
}

TEST_F(WrapInTransactionTest, MultipleWritesAtomic) {
  pulse::Result<void> result =
      WrapInTransaction(&db_, [this]() -> pulse::Result<void> {
        if (pulse::Result<void> err = db_.Execute(
                R"sql(INSERT INTO TestTable (Id, Value) VALUES (1, 'hello');)sql");
            !err.ok()) {
          return err;
        }

        if (pulse::Result<void> err = db_.Execute(
                R"sql(INSERT INTO TestTable (Id, Value) VALUES (2, 'world');)sql");
            !err.ok()) {
          return err;
        }

        return pulse::Result<void>{};
      });

  ASSERT_TRUE(result.ok());

  int count = 0;
  ASSERT_TRUE(db_.Execute("SELECT COUNT(*) FROM TestTable;", {},
                          [&count](int c) { count = c; })
                  .ok());
  EXPECT_THAT(count, Eq(2));
}

TEST_F(WrapInTransactionTest, MultipleWritesRollbackOnFailure) {
  pulse::Result<void> result =
      WrapInTransaction(&db_, [this]() -> pulse::Result<void> {
        if (pulse::Result<void> err = db_.Execute(
                R"sql(INSERT INTO TestTable (Id, Value) VALUES (1, 'hello');)sql");
            !err.ok()) {
          return err;
        }

        return pulse::Error{.code = pulse::Error::Code::kInternal,
                            .message = "forced failure"};
      });

  EXPECT_FALSE(result.ok());
  EXPECT_THAT(result.error().code, Eq(pulse::Error::Code::kInternal));

  int count = 0;
  ASSERT_TRUE(db_.Execute("SELECT COUNT(*) FROM TestTable;", {},
                          [&count](int c) { count = c; })
                  .ok());
  EXPECT_THAT(count, Eq(0));
}

}  // namespace

}  // namespace vigil
