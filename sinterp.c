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

void quit_program() {
    printf("Goodbye!\n");
    exit(0);
}

// Проверка условий для цикла while
int check_while_cond() {
    int operand1, operand2;
    
    if (has_letters(while_operand_1)) operand1 = get_var(while_operand_1);
    else operand1 = atoi(while_operand_1);

    if (has_letters(while_operand_2)) operand2 = get_var(while_operand_2);
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
            quit_program();
    }
}

// Функция для проверки двух переменных string на равенство
int eq_strings(char *str1, char *str2) {
    return strcmp(str1, str2) == 0 ? 1 : 0;
}

int has_letters(char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if isalpha(str[i]) return 1;
    }
    return 0;
}

// Получение аргументов/параметров для операторов
const char* seek_args(int row, int col, int seekType) {
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
                quit_program();
            }
        }
    } else if (seekType == 1) {     // Поиск числа
        for (i = col; script[row][i] != '\0'; i++) {
            if (script[row][i] == ' ' && strlen(result) == 0) continue;
            if (isdigit(script[row][i])) {
                result[strlen(result)] = script[row][i];
            } else {
                printf("Error! Unexpected symbol '%c' at line %d, column %d\n", script[row][i], row+1, i+1);
                quit_program();
            }
        }
    }
    global_index = i;
    char* toreturn = result;
    return toreturn;
}

// Поиск ключевого слова "done" для активации цикла
int find_keyword(int start_index, char* keyword) {
    for (int i = start_index; i < line_count; i++) {
        if (eq_strings(seek_args(i, 0, 0), keyword)) return i;
    }
    return 0;
}

char seek_operator(int row, int col) {
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
            quit_program();
        }
    }
    return '0';
}

// Получение команды для чтения (параметр index указывает, с какого индекса начать читать)
void get_command(int index) {
    char current_command[VAR_NAME_MEMORY] = "";
    int cci = 0;        // cci - Current Command Index
    int i;              // индекс i сохраняю вне цикла, чтобы потом продолжить читать строку с него
    global_index = 0;    // обнуляю global index на всякий случай

    // Делаю парсинг команды/переменной (выношу первое слово строки в переменную current_command)
    for (i = 0; script[index][i] != '\0'; i++) {
        if (isdigit(current_command[0])) {
            printf("Error! Name of a variable cannot start with digit! (line %d, col %d)\n", index+1, i+1);
            quit_program();
        }
        if (script[index][i] == ' ') {
            if (strlen(current_command) == 0) continue;
            else break;
        }
        if (script[index][i] == '=') {
            if (strlen(current_command) > 0) break;
            else {
                printf("Error! You cannot assign a value to nothing! (line %d, col %d)\n", index+1, i+1);
                quit_program();
            }
        }
        if (isalpha(script[index][i]) || isdigit(script[index][i])) {
            current_command[cci++] = script[index][i];
        } else {
            printf("Error! Unexpected symbol '%c' at line %d, column %d\n", script[index][i], index+1, i+1);
            quit_program();
        }
    }

    if (eq_strings(current_command, "read")) {
        char varname[VAR_NAME_MEMORY] = "";
        strcpy(varname, seek_args(index, i, 0));
        set_var(varname, read("Enter a number: "));
    } else if (eq_strings(current_command, "write")) {
        char varname[VAR_NAME_MEMORY] = "";
        strcpy(varname, seek_args(index, i, 0));
        write(varname);
    } else if (eq_strings(current_command, "while")) {
        // Проверка на факт того, является ли цикл вложенным
        if (while_loop_active) {
            printf("Error! Nested loops are not allowed! (at line %d)\n", index+1);
            quit_program();
        }
        if (!find_keyword(index+1, "done")) {
            printf("Error! Cannot activate the loop without a 'done' keyword!\n", index+1);
            quit_program();
        }
        strcpy(while_operand_1, seek_args(index, i, 0));
        i = global_index;
        while_operator = seek_operator(index, i); // Оператор для операции
        i = global_index;
        strcpy(while_operand_2, seek_args(index, i, 0));
        i = global_index;
        if (eq_strings(seek_args(index, i, 0), "do")) { // Проверка на наличие ключевого слова "do" для запуска цикла
            while_loop_active = 1;
            while_index = index;
            if (!check_while_cond()) current_line = find_keyword(index+1, "done");
            return;
        } else {
            printf("Error! While loop is not declared properly! (missing 'do' at line %d)\n", index+1);
            quit_program();
        }
    } else if (eq_strings(current_command, "done")) {
        if (!while_loop_active) {
            printf("Error! Unexpected 'done' operator at line %d!\n", index+1);
            quit_program();
        }
        if (check_while_cond()) current_line = while_index;
        else while_loop_active = 0;
    } else {
        int flag = 0;
        int temp = 0;
        for (i; script[index][i] != '\0'; i++) {
            if (script[index][i] == '=') {
                char tempstr[VAR_NAME_MEMORY];
                strcpy(tempstr, seek_args(index, i+1, 0));
                i = global_index;
                if (has_letters(tempstr)) temp += get_var(tempstr);  // Присваиваем значение другой переменной
                else temp += atoi(tempstr);                        // Присваиваем числовое значение
                flag = 1; // Флаг, говорящий о том, что операция выполнена
                break;
            }
        }
        if (!flag) {
            printf("Error! Command '%s' not found!\n", current_command);
            quit_program();
        }
        char oper = seek_operator(index, i);
        i = global_index;
        if (oper == '+' || oper == '-') {
            char args2[VAR_NAME_MEMORY];
            strcpy(args2, seek_args(index, i, 0));
            int toAdd;
            if (strlen(args2) > 0) {
                if (has_letters(args2)) toAdd = get_var(args2);  // Присваиваем значение другой переменной
                else toAdd = atoi(args2);
            }
            temp += oper == '+' ? toAdd : -toAdd;
        } else if (oper != '0') {
            if (oper == '=') printf("Error! Wrong operator '==' at line %d col %d!\n", index+1, i+1);
            else printf("Error! Wrong operator '%c' at line %d col %d!\n", oper, index+1, i+1);
            quit_program();
        }
        set_var(current_command, temp);
    }
    return;
}

// Получение переменной по её названию
int get_var(char* name) {
    for (int i = 0; i < var_count; i++) {
        if (eq_strings(var_names[i], name)) {
            int result = var_values[i];
            return result;
        }
    }
    printf("Error! Variable '%s' not found!\n", name);
    quit_program();
}

// Объявить переменную или изменить значение уже существующей
int set_var(char* name, int value) {
    for (int i = 0; i < var_count; i++) {    // Поиск переменной, чтобы изменить её значение
        if (eq_strings(var_names[i], name)) {
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
        quit_program();
    }
    return 1;
}

// Вывод значения переменной на экран
void write(char* arg) {
    printf("%d\n", get_var(arg));
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
        get_command(current_line);
    }

    return 0;
}