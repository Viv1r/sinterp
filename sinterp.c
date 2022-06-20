#include <stdio.h>

// Получение команды для чтения (параметр index указывает, с какого индекса начать читать)
void getCommand(char* script, int index) {
    printf("%s\n", script);
    return;
}

// Получение аргументов/параметров для операторов
void seek(char* script, int index) {
    return;
}

void print(char* arg) {

}

void read(char* arg) {

}

void throwError(int errorCode, int arg1, int arg2, char arg3) {
    switch(errorCode) {
        case 1:
            printf("Error! Unexpected symbol '%c' at line %d, column %d", arg3, arg1, arg2);
            break;
        case 2:
            printf("Error! Missing semicolon at line %d, column %d", arg1, arg2);
            break;
        default:
            printf("Error! Something went wrong... check the script file.");
            break;
    }
    printf("\n");
}

int main(void) {
    const int MAX_FILE_SIZE = 1024;

    FILE *fp;
    fp = fopen("script.al", "r");
    // Строка со скриптом
    char buff[MAX_FILE_SIZE];
    fgets(buff, MAX_FILE_SIZE, (FILE*)fp);
    fclose(fp);

                        // Убираются пробелы из кода (пока не знаю, пригодится ли это)

                        // char readyToGo[MAX_FILE_SIZE];
                        // int current = 0;
                        
                        // for (int i = 0; i < MAX_FILE_SIZE; i++) {
                        //     if (buff[i] != ' ') {
                        //         readyToGo[current++] = buff[i];
                        //     }
                        // }


    return 0;
}