#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define MAX_FILE_SIZE 4096      // Макс. размер файла со скриптом
#define MAX_LINE_LENGTH 128     // Макс. длина строки кода
#define VAR_STORAGE_SIZE 64     // Макс. кол-во переменных
#define VAR_NAME_MEMORY 64      // Сколько памяти выделить для названия переменной

char var_names[VAR_STORAGE_SIZE][VAR_NAME_MEMORY];      // Массив с названиями переменных
long int var_values[VAR_STORAGE_SIZE];                  // Массив со значениями переменных
char script[MAX_FILE_SIZE][MAX_LINE_LENGTH];            // Массив со строками скрипта
int current_line,                                       // Текущая строка, на которой работает интерпретатор
    var_count = 0,                                      // Количество переменных
    line_count = 0,                                      // Количество строк в скрипте
    while_loop_active = 0,                              // Флаг, отвечающий за активность цикла while
    while_index = 0,                                    // Номер строки, с которой начинается тело цикла while
    global_index = 0;                                   // Глобальный индекс для циклов, чтобы не заморачиваться с указателями

char while_operand_1[VAR_NAME_MEMORY] = "",      // Первый операнд для условия цикла while (если он активен)
     while_operand_2[VAR_NAME_MEMORY] = "";      // Второй операнд
char while_operator;                             // Оператор условия 

void quitProgram() {
    printf("Goodbye!\n");
    exit(0);
}

// Проверка условий для цикла while
int checkWhileCondition() {
    int operand1, operand2;
    
    if (hasLetters(while_operand_1)) operand1 = getVar(while_operand_1);
    else operand1 = atoi(while_operand_1);

    if (hasLetters(while_operand_2)) operand2 = getVar(while_operand_2);
    else operand2 = atoi(while_operand_2);

    switch(while_operator) {
        case '>':
            return operand1 > operand2;
        case '<':
            return operand1 < operand2;
        case '=':
            return operand1 == operand2;
        default:
            printf("Error! The '%c' operator can't be used for a while loop!\n", while_operator);
            quitProgram();
    }
}

// Функция для проверки двух переменных string на равенство
int eqStrings(char *str1, char *str2) {
    return strcmp(str1, str2) == 0 ? 1 : 0;
}

int hasLetters(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if isalpha(str[i]) return 1;
    }
    return 0;
}

// Получение аргументов/параметров для операторов
const char* seekArgs(int row, int col, int seekType) {
    char result[64] = "";
    int i;
    if (seekType == 0) {    // Поиск наименования (буквы+цифры)
        for (i = col; script[row][i] != '\0'; i++) {
            if (script[row][i] == ' ') {
                if (strlen(result) == 0) continue;
                break;
            }
            if (isalpha(script[row][i]) || isdigit(script[row][i])) {
                result[strlen(result)] = script[row][i];
            } else {
                printf("Error! Unexpected symbol '%c' at line %d, column %d\n", script[row][i], row+1, i+1);
                quitProgram();
            }
        }
    } else if (seekType == 1) {     // Поиск числа
        for (i = col; script[row][i] != '\0'; i++) {
            if (script[row][i] == ' ' && strlen(result) == 0) continue;
            if (isdigit(script[row][i])) {
                result[strlen(result)] = script[row][i];
            } else {
                printf("Error! Unexpected symbol '%c' at line %d, column %d\n", script[row][i], row+1, i+1);
                quitProgram();
            }
        }
    }
    global_index = i;
    char* toreturn = result;
    return toreturn;
}

// Поиск ключевого слова "done" для активации цикла
int findDone(int index) {
    for (int i = index; i < line_count; i++) {
        if (eqStrings(seekArgs(i, 0, 0), "done")) return i;
    }
    return 0;
}

char seekOperator(int row, int col) {
    int i;
    for (i = col; script[row][i] != '\0'; i++) {
        if (script[row][i] == ' ') continue;
        if (script[row][i] == '+' || script[row][i] == '-' || script[row][i] == '>' || script[row][i] == '<') {
            global_index = i+1;
            return script[row][i];
        } else if (script[row][i] == '=' && script[row][i+1] == '=') {
            global_index = i+2;
            return '=';
        } else {
            printf("Error! Unexpected symbol '%c' at line %d, column %d\n", script[row][i], row+1, i+1);
            quitProgram();
        }
    }
    return '0';
}

// Получение команды для чтения (параметр index указывает, с какого индекса начать читать)
void getCommand(int index) {
    char current_command[VAR_NAME_MEMORY] = "";
    int cci = 0;        // cci - Current Command Index
    int i;              // индекс i сохраняю вне цикла, чтобы потом продолжить читать строку с него
    global_index = 0;    // обнуляю global index на всякий случай

    // Делаю парсинг команды/переменной (выношу первое слово строки в переменную current_command)
    for (i = 0; script[index][i] != '\0'; i++) {
        if (isdigit(current_command[0])) {
            printf("Error! Name of a variable cannot start with digit! (line %d, col %d)\n", index+1, i+1);
            quitProgram();
        }
        if (script[index][i] == ' ') {
            if (strlen(current_command) == 0) continue;
            else break;
        }
        if (script[index][i] == '=') {
            if (strlen(current_command) > 0) break;
            else {
                printf("Error! You cannot assign a value to nothing! (line %d, col %d)\n", index+1, i+1);
                quitProgram();
            }
        }
        if (isalpha(script[index][i]) || isdigit(script[index][i])) {
            current_command[cci++] = script[index][i];
        } else {
            printf("Error! Unexpected symbol '%c' at line %d, column %d\n", script[index][i], index+1, i+1);
            quitProgram();
        }
    }

    if (eqStrings(current_command, "read")) {
        char varname[VAR_NAME_MEMORY] = "";
        strcpy(varname, seekArgs(index, i, 0));
        setVar(varname, read("Enter a number: "));
    } else if (eqStrings(current_command, "write")) {
        char varname[VAR_NAME_MEMORY] = "";
        strcpy(varname, seekArgs(index, i, 0));
        write(varname);
    } else if (eqStrings(current_command, "while")) {
        // Проверка на факт того, является ли цикл вложенным
        if (while_loop_active) {
            printf("Error! Nested loops are not allowed! (at line %d)\n", index+1);
            quitProgram();
        }
        if (!findDone(index+1)) {
            printf("Error! Cannot activate the loop without a 'done' keyword!\n", index+1);
            quitProgram();
        }
        strcpy(while_operand_1, seekArgs(index, i, 0));
        i = global_index;
        while_operator = seekOperator(index, i); // Оператор для операции
        i = global_index;
        strcpy(while_operand_2, seekArgs(index, i, 0));
        i = global_index;
        if (eqStrings(seekArgs(index, i, 0), "do")) { // Проверка на наличие ключевого слова "do" для запуска цикла
            while_loop_active = 1;
            while_index = index;
            if (!checkWhileCondition()) current_line = findDone(index+1);
            return;
        } else {
            printf("Error! While loop is not declared properly! (missing 'do' at line %d)\n", index+1);
            quitProgram();
        }
    } else if (eqStrings(current_command, "done")) {
        if (!while_loop_active) {
            printf("Error! Unexpected 'done' operator at line %d!\n", index+1);
            quitProgram();
        }
        if (checkWhileCondition()) current_line = while_index;
        else while_loop_active = 0;
    } else {
        int flag = 0;
        int temp = 0;
        for (i; script[index][i] != '\0'; i++) {
            if (script[index][i] == '=') {
                char tempstr[VAR_NAME_MEMORY];
                strcpy(tempstr, seekArgs(index, i+1, 0));
                i = global_index;
                if (hasLetters(tempstr)) temp += getVar(tempstr);  // Присваиваем значение другой переменной
                else temp += atoi(tempstr);                        // Присваиваем числовое значение
                flag = 1; // Флаг, говорящий о том, что операция выполнена
                break;
            }
        }
        if (!flag) {
            printf("Error! Command '%s' not found!\n", current_command);
            quitProgram();
        }
        char oper = seekOperator(index, i);
        i = global_index;
        if (oper == '+' || oper == '-') {
            char args2[VAR_NAME_MEMORY];
            strcpy(args2, seekArgs(index, i, 0));
            int toAdd;
            if (strlen(args2) > 0) {
                if (hasLetters(args2)) toAdd = getVar(args2);  // Присваиваем значение другой переменной
                else toAdd = atoi(args2);
            }
            temp += oper == '+' ? toAdd : -toAdd;
        } else if (oper != '0') {
            if (oper == '=') printf("Error! Wrong operator '==' at line %d col %d!\n", index+1, i+1);
            else printf("Error! Wrong operator '%c' at line %d col %d!\n", oper, index+1, i+1);
            quitProgram();
        }
        setVar(current_command, temp);
    }
    return;
}

// Получение переменной по её названию
int getVar(char* name) {
    for (int i = 0; i < var_count; i++) {
        if (eqStrings(var_names[i], name)) {
            int result = var_values[i];
            return result;
        }
    }
    printf("Error! Variable '%s' not found!\n", name);
    quitProgram();
}

// Объявить переменную или изменить значение уже существующей
int setVar(char* name, int value) {
    for (int i = 0; i < var_count; i++) {    // Поиск переменной, чтобы изменить её значение
        if (eqStrings(var_names[i], name)) {
            var_values[i] = value;
            return 1;
        }
    }
    // Далее идёт создание переменной, если она не найдена
    if (var_count < VAR_STORAGE_SIZE) {
        strcpy(var_names[var_count], name);
        var_values[var_count] = value;
        var_count++;
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
    line_count = index;
    fclose(fp);

    // Далее идёт выполнение скрипта
    for (current_line = 0; current_line < line_count; current_line++) {
        getCommand(current_line);
    }

    return 0;
}