#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ANSI Escape Codes for Colors
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

// Data Structures

typedef enum { DEBIT, CREDIT } TransactionType;

typedef struct {
  unsigned int id;
  char description[50];
  double amount;
  TransactionType type;
} Transaction;

typedef struct {
  Transaction *transactions; // pointer
  size_t count;
} FinancialLedger;

typedef struct {
  const FinancialLedger *ledger;
  size_t current_index;
} LedgerIterator;

// Function Pointers (Callback) Definitions
typedef bool (*TransactionFilter)(const Transaction *txt);
//      ^        ^                      ^
//      return   function name(alias)   parameter

// Itarator Implementation
LedgerIterator ledger_get_iterator(const FinancialLedger *ledger) {
  return (LedgerIterator){.ledger = ledger, .current_index = 0};
}

const Transaction *ledger_next(LedgerIterator *iterator,
                               TransactionFilter filter) {

  while (iterator->current_index < iterator->ledger->count) {
    const Transaction *tx =
        &iterator->ledger->transactions[iterator->current_index];
    iterator->current_index++;

    if (filter == nullptr || filter(tx)) {
      return tx;
    }
  }

  return nullptr;
}

// Filter Functions (Callbacks)
bool filter_only_credits(const Transaction *tx) { return tx->type == CREDIT; }

bool filter_high_value_debits(const Transaction *tx) {
  return tx->type == DEBIT && tx->amount >= 1000.0;
}

// MAIN
int main(void) {

  // Sample financial data - Static array (fixed size on the stack)
  Transaction tx_pool[] = {
      {.id = 1,
       .description = "Salary Deposit",
       .amount = 5000.0,
       .type = CREDIT},
      {.id = 2,
       .description = "Grocery Shopping",
       .amount = 150.5,
       .type = DEBIT},
      {.id = 3, .description = "New Laptop", .amount = 1200.0, .type = DEBIT},
      {.id = 4,
       .description = "Freelance Project",
       .amount = 850.0,
       .type = CREDIT},
      {.id = 5,
       .description = "Eletricity Bill",
       .amount = 95.0,
       .type = DEBIT}};

  FinancialLedger ledger1 = {
      .transactions = tx_pool, // The pointer only points to the static array.
      .count = sizeof(tx_pool) / sizeof(Transaction)};

  printf(ANSI_COLOR_BLUE "--- Scenario 1: Calculating Total Credit Balance "
                         "---\n" ANSI_COLOR_RESET);
  LedgerIterator credit_iter = ledger_get_iterator(&ledger1);
  const Transaction *tx = nullptr;
  double total_credit = 0.0;

  // Passing 'filter_only_credits' function pointer as a callback
  while ((tx = ledger_next(&credit_iter, filter_only_credits)) != nullptr) {
    printf("Processing Credit [%d] - %s: " ANSI_COLOR_GREEN
           "+$%.2f" ANSI_COLOR_RESET "\n",
           tx->id, tx->description, tx->amount);
    total_credit += tx->amount;
  }
  printf("Total Credit Generated: " ANSI_COLOR_GREEN "$%.2f" ANSI_COLOR_RESET
         "\n\n",
         total_credit);

  printf(ANSI_COLOR_BLUE "--- Scenario 2: Auditing High Value Debits (>= "
                         "$1000) ---\n" ANSI_COLOR_RESET);
  LedgerIterator audit_iter = ledger_get_iterator(&ledger1);

  // Passing 'filter_high_value_debits' function pointer as a callback
  while ((tx = ledger_next(&audit_iter, filter_high_value_debits)) != NULL) {
    printf(ANSI_COLOR_RED "ALERT: Audit Flagged [%d] %s" ANSI_COLOR_RESET
                          " - Amount: " ANSI_COLOR_YELLOW
                          "$%.2f" ANSI_COLOR_RESET "\n",
           tx->id, tx->description, tx->amount);
  }

  return 0;
}
