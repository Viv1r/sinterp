#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define MAX_FILE_SIZE 4096      // Макс. размер файла со скриптом
#define MAX_LINE_LENGTH 128      // Макс. длина строки кода
#define VAR_STORAGE_SIZE 64     // Макс. кол-во переменных
#define VAR_NAME_MEMORY 64      // Сколько памяти выделить для названия переменной

char varNames[VAR_STORAGE_SIZE][VAR_NAME_MEMORY];          // Массив с названиями переменных
long int varValues[VAR_STORAGE_SIZE];                   // Массив со значениями переменных
char script[MAX_FILE_SIZE][MAX_LINE_LENGTH];            // Массив со строками скрипта
int varCount = 0,                                       // Количество переменных
    lineCount = 0,                                      // Количество строк в скрипте
    whileLoop = 0;                                      // Флаг, отвечающий за активность цикла while

void quitProgram() {
    printf("Goodbye!\n");
    exit(0);
}

// Функция для проверки двух переменных string на равенство
int eqStrings(char *str1, char *str2) {
    return strcmp(str1, str2) == 0 ? 1 : 0;
}

// Получение аргументов/параметров для операторов
const char* seekArgs(int row, int col, int seekType) {
    char result[64] = "";
    if (seekType == 0) {    // Поиск наименования (буквы+цифры)
        for (int i = col; script[row][i] != '\0'; i++) {
            if (script[row][i] == ' ' && strlen(result) == 0) continue;
            if (isalpha(script[row][i]) || isdigit(script[row][i])) {
                result[strlen(result)] = script[row][i];
            } else {
                printf("[SA] Error! Unexpected symbol '%c' at line %d, column %d\n", script[row][i], row+1, i+1);
                quitProgram();
            }
        }
    } else if (seekType == 1) {     // Поиск числа
        for (int i = col; script[row][i] != '\0'; i++) {
            if (script[row][i] == ' ' && strlen(result) == 0) continue;
            if (isdigit(script[row][i])) {
                result[strlen(result)] = script[row][i];
            } else {
                printf("[SA] Error! Unexpected symbol '%c' at line %d, column %d\n", script[row][i], row+1, i+1);
                quitProgram();
            }
        }
    }
    char* toreturn = result;
    return toreturn;
}

// Получение команды для чтения (параметр index указывает, с какого индекса начать читать)
void getCommand(int index) {
    char currentCommand[VAR_NAME_MEMORY] = "";
    int cci = 0; // cci - Current Command Index
    int i;       // индекс i сохраняю вне цикла, чтобы потом продолжить читать строку с него

    // Делаю парсинг команды/переменной (выношу первое слово строки в переменную currentCommand)
    for (i = 0; (script[index][i] != ' ' || strlen(currentCommand) == 0); i++) {
        if (script[index][i] == ' ') continue;
        if (isalpha(script[index][i]) || isdigit(script[index][i])) {
            currentCommand[cci++] = script[index][i];
        }
    }

    if (eqStrings(currentCommand, "read")) {
        char varname[VAR_NAME_MEMORY] = "";
        strcpy(varname, seekArgs(index, i, 0));
        setVar(varname, read("Enter a number: "));
    } else if (eqStrings(currentCommand, "write")) {
        char varname[VAR_NAME_MEMORY] = "";
        strcpy(varname, seekArgs(index, i, 0));
        write(varname);
    } else if (eqStrings(currentCommand, "while")) {
        // char args = seekArgs(index, i, 1);
    } else {
        int flag = 0;
        for (i; script[index][i] != '\0'; i++) {
            if (script[index][i] == '=') {
                char temp[64];
                strcpy(temp, seekArgs(index, i+1, 1));
                setVar(currentCommand, atoi(temp));
                flag = 1; // Флаг, говорящий о том, что операция выполнена
                break;
            }
        }
        if (flag == 0) {
            printf("Error! Command '%s' not found!\n", currentCommand);
            quitProgram();
        }
    }
    /*
    for (int j = i; script[index][j] != '\0'; j++) {
        if (script[index][j] == ' ') {
            
            continue;
        } else if (script[index][j] == '=') {
            continue;
        } else {
            printf("[GC] Error! Unexpected symbol '%c' at line %d, column %d\n", script[index][j], index+1, j+1);
            quitProgram();
        }
    } */
    return;
}

// Получение переменной по её названию
int getVar(char* name) {
    for (int i = 0; i < varCount; i++) {
        if (eqStrings(varNames[i], name)) {
            int result = varValues[i];
            return result;
        }
    }
    printf("Error! Variable '%s' not found!\n", name);
    quitProgram();
}

// Объявить переменную или изменить значение уже существующей
int setVar(char* name, int value) {
    for (int i = 0; i < varCount; i++) {    // Поиск переменной, чтобы изменить её значение
        if (eqStrings(varNames[i], name)) {
            varValues[i] = value;
            return 1;
        }
    }
    // Далее идёт создание переменной, если она не найдена
    if (varCount < VAR_STORAGE_SIZE) {
        strcpy(varNames[varCount], name);
        varValues[varCount] = value;
        varCount++;
    } else {
        printf("Error! Too many variables! Max: %d\n", VAR_STORAGE_SIZE); // Если превышен лимит переменных, выдаёт ошибку
        quitProgram();
    }
    return 1;
}

// Вывод значения переменной на экран
void write(char* arg) {
    printf("%d\n", getVar(arg));
    return;
}

// Ввод значения пользователем
int read(char* title) {
    int result;
    printf("%s", title);
    scanf("%d", &result);
    return result;
}

int main(int argc, char **argv) {
    printf("SCRIPT TO EXECUTE (%s):\n", argv[1]);

    FILE *fp;
    fp = fopen(argv[1], "r");

    int index = 0;
    while(fgets(script[index], MAX_LINE_LENGTH, fp)) 
	{
        if (script[index][strlen(script[index]) - 1] == '\n') {
            script[index][strlen(script[index]) - 1] = '\0';
        } else {
            script[index][strlen(script[index])] = '\0';
        }
        index++;
    }
    lineCount = index;

    for (int i = 0; i < lineCount; i++) printf("||   %s\n", script[i]);
    fclose(fp);

    printf("\nSTARTING...\n\n");
    // Далее идёт выполнение скрипта
    for (int i = 0; i < lineCount; i++) {
        getCommand(i);
    }

    return 0;
}