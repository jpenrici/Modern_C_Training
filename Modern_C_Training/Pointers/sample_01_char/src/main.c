#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned int id;
  char *name; // pointer to a dynamic string
} User;

static inline void appendString(char *dest, const char *src, int *index) {
  if (dest == nullptr || src == nullptr || index == nullptr)
    return;
  for (int i = 0; src[i] != '\0'; ++i, (*index)++)
    dest[*index] = src[i];
}

void updateId(User *user, unsigned int new_id) {
  if (user == nullptr)
    return;
  user->id = new_id;
}

char *createGreeting(const char *name) {
  if (name == nullptr)
    return nullptr;

  // Dynamic allocation of char
  const char *suffix = ", welcome to C23!";
  size_t needed_space = strlen(name) + strlen(suffix) + 1;

  char *result = malloc(needed_space);
  if (result == nullptr)
    return nullptr;

  // Option 1
  // strcpy(result, name);
  // strcat(result, suffix);

  // Option 2
  // snprintf(result, needed_space, "%s%s", name, suffix);

  // Option 3 - Manual concatenation
  int index = 0;

  //for (int i = 0; name[i] != '\0'; ++i, ++index)
  //  result[index] = name[i];
  //for (int i = 0; suffix[i] != '\0'; ++i, ++index)
  //  result[index] = suffix[i];

  // Option 4 - Manual concatenation (inline function)
  appendString(result, name, &index);
  appendString(result, suffix, &index);
  result[index] = '\0';

  return result;
}

int main(void) {
  char *name = malloc(sizeof(char) * 50);
  if (name == nullptr)
    return 1;
  strcpy(name, "User 1"); // warning

  char **name_list = malloc(sizeof(char *) * 3); // array
  if (name_list == nullptr) {
    free(name);
    return 1;
  }

  name_list[0] = name;
  name_list[1] = "User 2";
  name_list[2] = "User 3";

  printf("List: %s, %s, %s\n", name_list[0], name_list[1], name_list[2]);

  User user1 = {.id = 1, .name = name_list[0]};
  User *user_ptr = &user1;

  printf("Original User -> ID: %u, Name: %s\n", user_ptr->id, user_ptr->name);
  updateId(user_ptr, 99);

  printf("Modified User -> ID: %u, Name: %s\n", user_ptr->id, user_ptr->name);

  char *greeting = createGreeting(user_ptr->name);
  if (greeting != nullptr)
    printf("%s\n", greeting);

  free(greeting);
  free(name);
  free(name_list);

  return 0;
}
