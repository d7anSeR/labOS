#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <math.h>

using namespace std;

// Функция для вычисления квадрата числа
double calculateSquare(double num) {
    return num * num;
}

// Функция для вычисления корня квадратного числа
double calculateSqrt(double num) {
    return sqrt(num);
}

int main() {
    // Создание канала pipe для обмена данными между процессами
    int fd[2];
    if (pipe(fd) == -1) {
        cerr << "Ошибка при создании pipe" << endl;
        return 1;
    }

    // Создание процесса-ребенка для вычисления квадрата первого катета
    pid_t pid1 = fork();

    if (pid1 < 0) {
        cerr << "Ошибка при создании процесса" << endl;
        return 1;
    } else if (pid1 == 0) {
        // Это код процесса-ребенка 1

        // Закрываем чтение из pipe
        close(fd[0]);

        // Ввод длины первого катета
        double leg1 = 3;
        cout << "a = " << leg1<< endl;

        // Вычисление квадрата первого катета
        double square1 = calculateSquare(leg1);

        // Запись квадрата первого катета в pipe
        write(fd[1], &square1, sizeof(square1));

        // Закрываем запись в pipe и завершаем процесс-ребенок 1
        close(fd[1]);
        return 0;
    } else {
        // Создание процесса-ребенка для вычисления квадрата второго катета
        pid_t pid2 = fork();

        if (pid2 < 0) {
            cerr << "Ошибка при создании процесса" << endl;
            return 1;
        } else if (pid2 == 0) {
            // Это код процесса-ребенка 2

            // Закрываем чтение из pipe
            close(fd[0]);

            // Ввод длины второго катета
            double leg2 = 4;
            cout << "b = " << leg2<< endl;

            // Вычисление квадрата второго катета
            double square2 = calculateSquare(leg2);

            // Запись квадрата второго катета в pipe
            write(fd[1], &square2, sizeof(square2));

            // Закрываем запись в pipe и завершаем процесс-ребенок 2
            close(fd[1]);
            return 0;
        } else {
            // Закрываем запись в pipe
            close(fd[1]);

            // Ожидание завершения процессов-ребенков
            wait(NULL);
            wait(NULL);

            // Чтение квадратов катетов из pipe
            double square1, square2;
            read(fd[0], &square1, sizeof(square1));
            read(fd[0], &square2, sizeof(square2));

            // Закрываем чтение из pipe
            close(fd[0]);

            // Вычисление гипотенузы
            double hypotenuse = calculateSqrt(square1 + square2);

            // Вывод результата
            cout << "c = " << hypotenuse << endl;

            // Возвращаемся из родительского процесса
            return 0;
        }
    }
}