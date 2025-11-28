// io, memory, string, etc..
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// randomness..
#include <windows.h>
#include <bcrypt.h>

#define FILE_NAME "words.txt"
#define STAGE_ROWS 7

//          +---+          Topic: fuck niggers
//          |   |          Right: 69
//          O   |          Wrong: 420
//         /|\  |
//         / \  |
//              |          Word: _ate _iggers
//       ==========        Wrong guesses: h, n

const char *stages[][STAGE_ROWS] = {
    {   // Stage 0
        "   +---+\n",
        "   |   |\n",
        "       |\n",
        "       |\n",
        "       |\n",
        "       |\n",
        "==========\n"
    },

    {   // Stage 1
        "   +---+\n",
        "   |   |\n",
        "   O   |\n",
        "       |\n",
        "       |\n",
        "       |\n",
        "==========\n"
    },

    {   // Stage 2
        "   +---+\n",
        "   |   |\n",
        "   O   |\n",
        "   |   |\n",
        "       |\n",
        "       |\n",
        "==========\n"
    },

    {   // Stage 3
        "   +---+\n",
        "   |   |\n",
        "   O   |\n",
        "  /|   |\n",
        "       |\n",
        "       |\n",
        "==========\n"
    },

    {   // Stage 4
        "   +---+\n",
        "   |   |\n",
        "   O   |\n",
        "  /|\\  |\n",
        "       |\n",
        "       |\n",
        "==========\n"
    },

    {   // Stage 5
        "   +---+\n",
        "   |   |\n",
        "   O   |\n",
        "  /|\\  |\n",
        "  /    |\n",
        "       |\n",
        "==========\n"
    },

    {   // Stage 6
        "   +---+\n",
        "   |   |\n",
        "   O   |\n",
        "  /|\\  |\n",
        "  / \\  |\n",
        "       |\n",
        "==========\n"
    }
};

void init(char **word);
void logic(char *word, size_t word_length);

int main(int argc, char **argv) {
    char *word = NULL;
    init(&word);

    size_t word_length = strlen(word);
    logic(word, word_length);
    return 0;
}

void print_hangman(int stage, char topic[], int right, int wrong, char secret_word[], char wrong_guesses[]);
void check_guess(char c, char *word, int *stage, int *right, int *wrong, char *secret_word, char *wrong_guesses, size_t word_length);

void logic(char *secret_word, size_t word_length) {
    char *topic = "Animals";
    int right = 0, wrong = 0;
    int stage = 0;

    char *word = malloc(word_length + 1);
    if (!word) {
        printf("memory allocation failed (logic)\n");
        return;
    }

    for (size_t i = 0; i < word_length; i++) {
        if (secret_word[i] == ' ') {
            word[i] = ' ';
        } else word[i] = '_';
    }

    word[word_length] = '\0';

    char *wrong_guesses = malloc(128);
    if (!wrong_guesses) {
        free(word);
        return;
    }
    wrong_guesses[0] = '\0';

    size_t total_letters = 0;
    for (size_t i = 0; i < word_length; i++)
        if (secret_word[i] != ' ') total_letters++;

    char c;
    while (stage < 6 && right < total_letters) {
        print_hangman(stage, topic, right, wrong, word, wrong_guesses);

        printf("Please enter your guess: ");
        scanf(" %c", &c);
        while (getchar() != '\n');

        check_guess(c, word, &stage, &right, &wrong, secret_word, wrong_guesses, word_length);
    }

    print_hangman(stage, topic, right, wrong, word, wrong_guesses);
    if (right == total_letters) {
        printf("Congratulations! You guessed the word(s): \"%s\"\n", secret_word);
    } else printf("Game Over! The word(s) was: \"%s\"\n", secret_word);

    free(word);
    free(wrong_guesses);
}

void print_hangman(int stage, char topic[], int right, int wrong, char secret_word[], char wrong_guesses[]) {
    printf("\n");
    size_t length = strlen(wrong_guesses);

    for (int i = 0; i < STAGE_ROWS; i++) {
        int add_info = (i == 0 || i == 1 || i == 2 || i == 5 || i == 6);

        if (add_info) {
            size_t length = strcspn(stages[stage][i], "\n");
            printf("%.*s", (int) length, stages[stage][i]);
        } else printf("%s", stages[stage][i]);

        if (i == 0) printf("          Topic: %s\n", topic);
        if (i == 1) printf("          Right: %d\n", right);
        if (i == 2) printf("          Wrong: %d\n", wrong);
        if (i == 5) printf("          Word(s): %s\n", secret_word);
        if (i == 6) {
            printf("        Wrong guesses:");
            if (length == 0) printf("\n");

            for (int j = 0; j < length; j++) {

                if (j < length - 1) {
                    printf(" '%c',", wrong_guesses[j]);
                } else printf(" '%c'\n", wrong_guesses[j]);

            }
        }
    }

    printf("\n");
}

void check_guess(char c, char *word, int *stage, int *right, int *wrong, char *secret_word, char *wrong_guesses, size_t word_length) {
    int found = 0;

    c = tolower(c);
    for (size_t i = 0; i < word_length; i++) {
        if (tolower(secret_word[i]) != c) continue;
        found = 1;

        if (tolower(word[i]) == c) continue;
        word[i] = c;
        (*right)++;
    }

    size_t length = strlen(wrong_guesses);
    for (int i = 0; i < length; i++) {
        if (tolower(wrong_guesses[i]) == c) found = 1;
    }

    if (found) return;
    wrong_guesses[length] = c;
    wrong_guesses[length + 1] = '\0';

    (*wrong)++;
    (*stage)++;
}

char **read_words(size_t *words_size);
char *random_word(char **words, size_t words_size);

void init(char **word) {
    size_t words_size = 0;

    char **words = read_words(&words_size);
    if (words == NULL || words_size == 0) {
        printf("no words available (init)\n");
        *word = NULL;
        return;
    }
    *word = random_word(words, words_size);

    for (size_t i = 0; i < words_size; i++) free(words[i]);
    free(words);
}

char **read_words(size_t *words_size) {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        printf("failed to open file\n");
        return NULL;
    }

    char **words = NULL;
    char buffer[128];

    while (fgets(buffer, sizeof(buffer), file)) {
        // strcspn: string complementary span ->
        // it returns the first occurance of the second parameter in the first parameter's array
        buffer[strcspn(buffer, "\n")] = '\0'; // replace "new line" with "null terminal"

        // in case of managing the original pointer:
        // if the memory allocation fails, it returns NULL but you have overwritten the original memory
        // so it causes memory leak because you lose the only pointer to the previously allocated memory (can't free() it)
        // that's why you use a temporary pointer
        char **temp = realloc(words, sizeof(char*) * (*words_size + 1)); // +1 for (n + 1)th element
        if (temp == NULL) {
            printf("realloc failed (read)\n");
            free(words);

            return NULL;
        }
        words = temp;

        // size of buffer +1 for null terminal
        words[*words_size] = malloc(strlen(buffer) + 1);
        if (words[*words_size] == NULL) {
            printf("malloc failed (read)\n");

            for (size_t i = 0; i < *words_size; i++)
                free(words[i]);
            free(words);

            return NULL;
        }
        strcpy(words[*words_size], buffer);

        (*words_size)++;
    }

    fclose(file);
    return words;
}

char *random_word(char **words, size_t words_size) {
    if (words_size == 0) return NULL;

    unsigned int r;
    NTSTATUS status = BCryptGenRandom(NULL, (PUCHAR) &r, sizeof(r), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (!BCRYPT_SUCCESS(status)) {
        printf("random generation failed\n");
        return NULL;
    }

    size_t length = strlen(words[r % words_size]);
    char *word_copy = malloc(length + 1);
    if (!word_copy) {
        printf("memory allocation failed (random)\n");

        return NULL;
    }
    strcpy(word_copy, words[r % words_size]);

    return word_copy;
}
