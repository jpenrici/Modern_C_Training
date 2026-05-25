/**
 * @file main.c
 * @brief A simple CLI Financial Transaction Storage using SQLite3 in C.
 */

#include <errno.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function Prototypes
[[nodiscard]] int initialize_database(sqlite3 **db);
[[nodiscard]] bool input(char *buffer, int size);

void create_transaction(sqlite3 *db);
void read_transactions(sqlite3 *db);
void update_transaction(sqlite3 *db);
void delete_transaction(sqlite3 *db);
void display_menu(void);

constexpr int DESC_LEN = 100;
constexpr int BUF_LEN = 32;

int main() {
  sqlite3 *db = nullptr;
  char buffer[BUF_LEN];

  // Initialize Database
  if (initialize_database(&db) != SQLITE_OK) {
    perror("Failed to initialize database");
    return EXIT_FAILURE;
  }

  printf("=========================================\n");
  printf("   Welcome to Finance Tracker CLI (C)    \n");
  printf("=========================================\n");

  // Main Menu Loop
  int choice;
  do {
    display_menu();
    printf("Enter your choice: ");

    if (!input(buffer, sizeof(buffer))) {
      printf("Read error. Exiting.\n");
      break;
    }

    char *end;
    errno = 0;
    long val = strtol(buffer, &end, 10);
    if (errno != 0 || end == buffer || *end != '\0') {
      printf("Invalid input. Please enter a number.\n");
      choice = 0;
      continue;
    }

    choice = (int)val;
    switch (choice) {
    case 1:
      create_transaction(db);
      break;
    case 2:
      read_transactions(db);
      break;
    case 3:
      update_transaction(db);
      break;
    case 4:
      delete_transaction(db);
      break;
    case 5:
      printf("\nExiting program. Goodbye!\n");
      break;
    default:
      printf("Invalid option! Please choose between 1 and 5.\n");
    }
  } while (choice != 5);

  // Close Database Connection
  sqlite3_close(db);

  return EXIT_SUCCESS;
}

/**
 * @brief Reads a line from stdin into `buffer` (at most `size-1` chars),
 *        stripping the trailing newline.
 *
 * @return true on success, false on EOF or read error.
 */
[[nodiscard]] bool input(char *buffer, int size) {
  if (fgets(buffer, size, stdin) == nullptr) {
    return false;
  }

  char *nl = strchr(buffer, '\n');
  if (nl != nullptr) {
    *nl = '\0';
  } else {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
      /* Reading character by character, draining everything that's left in the
       * buffer. */
    }
  }
  return true;
}

/**
 * @brief Opens the database file and creates the transactions table if it
 * doesn't exist.
 * @param db Pointer to the sqlite3 database structure pointer.
 * @return SQLITE_OK on success, or SQLite error code.
 */
[[nodiscard]] int initialize_database(sqlite3 **db) {
  int rc = sqlite3_open("finance.db", db);
  if (rc != SQLITE_OK) {
    perror("Cannot open database");
    return rc;
  }

  // SQL statement to create table
  const char *sql = "CREATE TABLE IF NOT EXISTS transactions ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "description TEXT NOT NULL,"
                    "amount REAL NOT NULL,"
                    "type TEXT CHECK(type IN ('Income', 'Expense')) NOT NULL"
                    ");";

  char *err_msg = nullptr;
  rc = sqlite3_exec(*db, sql, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    perror("SQL error during initialization");
    sqlite3_free(err_msg);
    return rc;
  }

  return SQLITE_OK;
}

/**
 * @brief Displays the CLI interactive menu.
 */
void display_menu() {
  printf("\n--- FINANCIAL TRANSACTIONS MENU ---\n");
  printf("1. Create   Transaction  (Insert)\n");
  printf("2. View All Transactions (Read)\n");
  printf("3. Update   Transaction  (Update)\n");
  printf("4. Delete   Transaction  (Delete)\n");
  printf("5. Exit\n");
  printf("------------------------------------\n");
}

/**
 * @brief CRUD [CREATE]: Inserts a new financial transaction into the database.
 */
void create_transaction(sqlite3 *db) {
  sqlite3_stmt *stmt;
  char description[DESC_LEN];
  char buffer[BUF_LEN];

  printf("\n--- Create Transaction ---\n");

  printf("Enter description: ");
  if (!input(description, sizeof(description))) {
    printf("Read error.\n");
    return;
  }

  printf("Enter amount: ");
  if (!input(buffer, sizeof(buffer))) {
    printf("Read error.\n");
    return;
  }

  char *end;
  errno = 0;
  double amount = strtod(buffer, &end);
  if (errno != 0 || end == buffer || *end != '\0' || amount < 0) {
    printf("Invalid amount numeric format.\n");
    return;
  }

  printf("Select Type (1 for Income, 2 for Expense): ");
  if (!input(buffer, sizeof(buffer))) {
    printf("Read error.\n");
    return;
  }
  long type_choice = strtol(buffer, &end, 10);
  if (end == buffer || *end != '\0' || (type_choice != 1 && type_choice != 2)) {
    printf("Invalid choice. Operation cancelled.\n");
    return;
  }
  const char *type =
      (type_choice == 1) ? "Income" : "Expense"; // pointer to literal

  // SQL with placeholders (?) to prevent SQL Injection
  const char *sql =
      "INSERT INTO transactions (description, amount, type) VALUES (?, ?, ?);";

  // Prepare the statement
  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    perror("Failed to prepare statement");
    return;
  }

  // Bind parameters
  sqlite3_bind_text(stmt, 1, description, -1, SQLITE_TRANSIENT);
  sqlite3_bind_double(stmt, 2, amount);
  sqlite3_bind_text(stmt, 3, type, -1, SQLITE_TRANSIENT);

  // Execute statement
  if (sqlite3_step(stmt) == SQLITE_DONE) {
    printf("Transaction added successfully!\n");
  } else {
    perror("Execution failed");
  }

  // Finalize statement to clean up memory
  sqlite3_finalize(stmt);
}

/**
 * @brief CRUD [READ]: Selects and prints all rows from the transactions table.
 */
void read_transactions(sqlite3 *db) {
  sqlite3_stmt *stmt;
  const char *sql = "SELECT id, description, amount, type FROM transactions;";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    perror("Failed to prepare read statement");
    return;
  }

  printf("\n--- Financial Ledger ---\n");
  printf("%-4s | %-30s | %-10s | %-7s\n", "ID", "Description", "Amount",
         "Type");
  printf("-------------------------------------------------------------\n");

  int count = 0;
  // Step through results row by row
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const unsigned char *desc = sqlite3_column_text(stmt, 1);
    double amount = sqlite3_column_double(stmt, 2);
    const unsigned char *type = sqlite3_column_text(stmt, 3);

    printf("%-4d | %-30s | $%-9.2f | %-7s\n", id, desc, amount, type);
    count++;
  }

  if (count == 0) {
    printf("No transactions found in the database.\n");
  }
  printf("-------------------------------------------------------------\n");

  sqlite3_finalize(stmt);
}

/**
 * @brief CRUD [UPDATE]: Modifies description and amount of an existing
 * transaction by ID.
 */
void update_transaction(sqlite3 *db) {
  sqlite3_stmt *stmt;
  char buffer[BUF_LEN];

  printf("\n--- Update Transaction ---\n");
  printf("Enter the ID of the transaction to update: ");
  if (!input(buffer, sizeof(buffer))) {
    printf("Read error.\n");
    return;
  }
  char *end;
  errno = 0;
  long id = strtol(buffer, &end, 10);
  if (errno != 0 || end == buffer || *end != '\0' || id <= 0) {
    printf("Invalid ID format.\n");
    return;
  }

  // Verify the ID exists before asking the user for new data
  const char *check_sql = "SELECT 1 FROM transactions WHERE id = ?;";
  if (sqlite3_prepare_v2(db, check_sql, -1, &stmt, nullptr) != SQLITE_OK) {
    perror("Failed to prepare check statement");
    return;
  }
  sqlite3_bind_int(stmt, 1, (int)id);
  bool found = sqlite3_step(stmt) == SQLITE_ROW;
  sqlite3_finalize(stmt);

  if (!found) {
    printf("No transaction found with ID %ld.\n", id);
    return;
  }

  char new_description[DESC_LEN];
  printf("Enter new description: ");
  if (!input(new_description, sizeof(new_description))) {
    printf("Read error.\n");
    return;
  }

  printf("Enter new amount: ");
  if (!input(buffer, sizeof(buffer))) {
    printf("Read error.\n");
    return;
  }
  errno = 0;
  double new_amount = strtod(buffer, &end);
  if (errno != 0 || end == buffer || *end != '\0' || new_amount < 0) {
    printf("Invalid amount numeric format.\n");
    return;
  }

  const char *sql =
      "UPDATE transactions SET description = ?, amount = ? WHERE id = ?;";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    perror("Failed to prepare update statement");
    return;
  }

  sqlite3_bind_text(stmt, 1, new_description, -1, SQLITE_TRANSIENT);
  sqlite3_bind_double(stmt, 2, new_amount);
  sqlite3_bind_int(stmt, 3, (int)id);

  if (sqlite3_step(stmt) == SQLITE_DONE) {
    printf("Transaction ID %ld updated successfully.\n", id);
  } else {
    perror("Update execution failed");
  }

  sqlite3_finalize(stmt);
}

/**
 * @brief CRUD [DELETE]: Deletes a transaction from the database by ID.
 */
void delete_transaction(sqlite3 *db) {
  sqlite3_stmt *stmt;
  char buffer[BUF_LEN];

  printf("\n--- Delete Transaction ---\n");
  printf("Enter the ID of the transaction to delete: ");
  if (!input(buffer, sizeof(buffer))) {
    printf("Read error.\n");
    return;
  }
  char *end;
  errno = 0;
  long id = strtol(buffer, &end, 10);
  if (errno != 0 || end == buffer || *end != '\0' || id <= 0) {
    printf("Invalid ID format.\n");
    return;
  }

  const char *sql = "DELETE FROM transactions WHERE id = ?;";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    perror("Failed to prepare delete statement");
    return;
  }

  sqlite3_bind_int(stmt, 1, id);

  if (sqlite3_step(stmt) == SQLITE_DONE) {
    if (sqlite3_changes(db) > 0) {
      printf("Transaction ID %ld deleted successfully.\n", id);
    } else {
      printf("No transaction found with ID %ld.\n", id);
    }
  } else {
    perror("Delete execution failed");
  }

  sqlite3_finalize(stmt);
}
