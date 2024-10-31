#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// adds character to end of string
void add_character_to_string(char *str, char c) {
  int len = strlen(str);
  str[len] = c;
  str[len + 1] = '\0';
}

// splits string by delimiter and adds nulls into array
void split(char *cmd, char *words[], char delimiter) {
  int word_count = 0;
  char *next_char = cmd;
  char current_word[10000];
  strcpy(current_word, "");

  while (*next_char != '\0') {
    if (*next_char == delimiter) {
      words[word_count++] = strdup(current_word);
      strcpy(current_word, "");
    } else {
      add_character_to_string(current_word, *next_char);
    }
    ++next_char;
  }

  words[word_count++] = strdup(current_word);
  words[word_count] = NULL;
}
// true = found
bool find_absolute_path(char *cmd, char *absolute_path) {

  char *directories[1000];

  split(getenv("PATH"), directories, ':');

  // break path up by colon and look in array until i find path plus cmd

  for (int i = 0; directories[i] != NULL; i++) {
    char path[1000];
    strcpy(path, directories[i]);
    add_character_to_string(path, '/');
    strcat(path, cmd);
    if (access(path, X_OK) == 0) {
      strcpy(absolute_path, path);
      return true;
    }
  }
  return false;
}

int main(int argc, char *argv[]) {

  int input_fd = 0;
  // if argument is not a dash then open input file
  if (strcmp(argv[1], "-") != 0) {
    input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
      printf("Error: File not found\n");
      return 1;
    }
  }

  int output_fd = 0;
  // if argument is not a dash then open output file
  if (strcmp(argv[3], "-") != 0) {
    output_fd = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd == -1) {
      printf("Error: File not found\n");
      return 1;
    }
  }

  char absolute_path[1000];
  char *words[1000];

  // splits the command by spaces and puts into words array
  split(argv[2], words, ' ');

  if (words[0] == NULL) {
    printf("Error: No command provided\n");
    return 1;
  }
  //if cant find a path then return error
  if (!find_absolute_path(words[0], absolute_path)) {
    printf("Error: Command not found: %s\n", words[0]);
    return 1;
  }

  if (input_fd != 0) {
    dup2(input_fd, STDIN_FILENO);
    close(input_fd);
  }

  if (output_fd != 0) {
    dup2(output_fd, STDOUT_FILENO);
    close(output_fd);
  }

  execve(absolute_path, words, NULL);
  printf("Error: execv failed\n");
  return 1;
}