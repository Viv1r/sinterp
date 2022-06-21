#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define MAX_FILE_SIZE 4096      // Макс. размер файла со скриптом
#define MAX_LINE_LENGTH 64      // Макс. длина строки кода
#define VAR_STORAGE_SIZE 64     // Макс. кол-во переменных
#define VAR_MEMORY 128           // Сколько памяти выделить для названия переменной

char varNames[VAR_STORAGE_SIZE][VAR_MEMORY/2];          // Массив с названиями переменных
long int varValues[VAR_STORAGE_SIZE];                   // Массив со значениями переменных
int varCount = 0;

// Получение команды для чтения (параметр index указывает, с какого индекса начать читать)
void getCommand(char* script, int index) {
    char currentCommand[16];
    int cci = 0; // cci - Current Command Index
    for (int i = index; i != '\0'; i++) {
        if (script[i] == ' ') continue;
        if (!isalpha(script[i])) {
            if (script[i] == '(') {
                // seekArgs(script, i, !!!!TOSEEK!!!!);
            }
            printf("Error! Unexpected symbol '%c' at line %d, column %d", script[i], i, i);
        }
        currentCommand[cci] = script[i];
    }
    return;
}

// Получение аргументов/параметров для операторов
void seekArgs(char *script, int index, char *toSeek) {
    char current;
    for (int i = index; (script[i] != ')') && (script[i] != '\0'); i++) {
        continue;
    }
    return;
}

void print(char *arg) {
    printf("%s\n", arg);
    return;
}

char *read(char* title) {
    char result[64];
    scanf(title, &result);
    const char *toreturn = malloc(64);
    return result;
}

int getVar(char* name) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(varNames[i], name) == 0) {
            int result = varValues[i];
            return result;
        }
    }
    printf("Error! Variable '%s' not found!\n", name);
    return 0;
}

int setVar(char* name, int value) {
    for (int i = 0; i < varCount; i++) {    // Поиск переменной, чтобы изменить её значение
        if (strcmp(varNames[i], name) == 0) {
            varValues[i] = value;
            return 1;
        }
    }
    // Далее идёт создание переменной, если она не найдена
    if (varCount < VAR_STORAGE_SIZE) {
        strcpy(varNames[varCount], name);
        varValues[varCount++] = value;
    } else {
        printf("Error! Too many variables! Max: %d", VAR_STORAGE_SIZE); // Если превышен лимит переменных, выдаёт ошибку
        return 0;
    }
    return 1;
}

int main(void) {
    printf("START\n");

    char rawScript[MAX_FILE_SIZE];
    FILE *fp;
    fp = fopen("script.al", "r");
    fgets(rawScript, MAX_FILE_SIZE, (FILE*)fp);
    fclose(fp);

    setVar("test", 123);
    setVar("test2", 4345);
    setVar("foofoofoo", 9000);
    printf("%d\n%d\n%d\n", getVar("test"), getVar("test2"), getVar("foofoofoo"));

    // Это работает! Пора делать коммит.

    return 0;
}