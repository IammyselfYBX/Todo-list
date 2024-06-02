#include <stdio.h>
#include <string.h>
#include <time.h>
#if defined(_WIN16) || defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER)
#define localtime_r(T,Tm) (localtime_s(Tm,T) ? NULL : Tm)
#endif
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

int tLength = 0;
FILE *fp;

struct Todo
{
    char title[50];
    char createdAt[50];
    _Bool isCompleted;
} todos[20];

void saveToFile()
{
    fp = fopen("todos.bin", "w");
    if (!fp)
    {
        printf("Can't save your todo\n");
    }
    else
    {
        for (size_t i = 0; i < tLength; i++)
        {
            fwrite(&todos[i], sizeof(struct Todo), 1, fp);
        }
        fclose(fp);
    }
}

void getFileSize()
{
    fseek(fp, 0L, SEEK_END);
    unsigned int long size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    tLength = size / sizeof(struct Todo);
}

void readFromFile()
{
    fp = fopen("todos.bin", "r");
    if (!fp)
    {
        printf("We are not able to find any todos file\n");
    }
    else
    {
        getFileSize();
        for (size_t i = 0; i < tLength; i++)
        {
            fread(&todos[i], sizeof(struct Todo), 1, fp);
        }
        fclose(fp);
    }
}

void addTodo()
{
    // for todo title
    char userInput[50];
    printf("Type your todo \n>> ");
    scanf("%[^\n]s", userInput);
    strncpy(todos[tLength].title, userInput, 50);

    // add time
    char todoTime[50];
    struct tm cTime;
    time_t timeS = time(NULL);
    localtime_r(&timeS, &cTime);
    // 2/12 1:21
    snprintf(todoTime, 50, "%i/%i %i:%i", cTime.tm_mday, cTime.tm_mon + 1, cTime.tm_hour, cTime.tm_min);
    strcpy(todos[tLength].createdAt, todoTime);

    //set boolean to false
    todos[tLength].isCompleted = false;
    tLength++;
}

void printAllTodo()
{
#if defined(_WIN16) || defined(_WIN32) || defined(_WIN64)
  //printf("windows");
	system("cls");
#elif defined(__linux__)
  //	printf("linux");
  system("clear");
#elif defined(__APPLE__)
	// printf("apple");
  system("clear");
#else
	printf("未知系统，不知道如何清屏\n");
#endif
    printf("+----+-------------------------------------+--------------+-------------+\n");
    printf("| ID |             任务清单                |   创建时间   |   是否完成  |\n");
    printf("+----+-------------------------------------+--------------+-------------+\n");

    for (int i = 0; i < tLength; i++)
    {
        //if (todos[i].isCompleted)
        //{
        //    printf("\033[10m"); // 完成就是正常字体
        //}
        //else
        //{
        //    printf("\033[1m"); // 未完成就是粗体
        //}

        // 判断是否含有中文
        char c;
        char *str = todos[i].title;
        while(1)
        {
            c=*str++;
            // 如果不含中文
            if (c==0){   //如果到字符串尾则说明该字符串没有中文字符
               printf("|%3d | %-35s | %-12s | %-13s |\n", 
                       i + 1, todos[i].title, todos[i].createdAt, (!todos[i].isCompleted) ? " ❌  No  " : " ✅  Yes ");
               break;
            }
            // 如果含有中文
            if (c&0x80)        //如果字符高位为1且下一字符高位也是1则有中文字符
                if (*str & 0x80) {
                   printf("|%3d | %-37s | %-12s | %-13s |\n", 
                           i + 1, todos[i].title, todos[i].createdAt, (!todos[i].isCompleted) ? " ❌  No  " : " ✅  Yes ");
                   break;
                }
        }
        //printf("|%3d | %-35s | %-12s | %-13s |\n", i + 1, todos[i].title, todos[i].createdAt, (!todos[i].isCompleted) ? " ❌  No  " : " ✅  Yes ");
        printf("+----+-------------------------------------+--------------+-------------+\n");
    }
}

void markAsComplete()
{
    int todoId;
    printf("输入完成的ID \n>> ");
    scanf("%d", &todoId);
    todoId--;
    if (todoId < 0 || todoId > tLength)
    {
        printf("ID 无效 😑\n");
    }
    else
    {
        todos[todoId].isCompleted = true;
        printf("该任务已经完成 \n");
    }
}

void deleteTodo()
{
    int todoId;
    printf("输入完成任务的ID \n>> ");
    scanf("%d", &todoId);
    if (todoId < 0 || todoId > tLength)
    {
        printf("ID无效 😑\n");
    }
    else
    {
        todoId--;
        memmove(todos + todoId, todos + todoId + 1, (tLength - todoId - 1) * sizeof(*todos));
        tLength--;
        printf("第%d条任务已经删除 😵\n", todoId);
    }
}

void ShowOptions()
{
    char userChoice;
    printf("输入'A'：表示添加任务；'D'：删除任务；'C'：标记完成；'Q'：推出程序\n>> ");
    userChoice = getchar();
    userChoice = toupper(userChoice);
    getchar();
    switch (userChoice)
    {
    case 'A':
        addTodo();
        break;
    case 'D':
        deleteTodo();
        break;
    case 'C':
        markAsComplete();
        break;
    case 'Q':
        exit(0);
        break;
    default:
        printf("输入错误 😓\n");
        ShowOptions();
        break;
    }
    saveToFile();
    printAllTodo();
    getchar();
    ShowOptions();
}

void isThisFirstTime()
{
    if (access("todos.bin", F_OK) != -1)
    {
        readFromFile();
        printAllTodo();
        ShowOptions();
    }
    else
    {
        printf("欢迎使用TODO 应用\n");
        addTodo();
        saveToFile();
        printAllTodo();
        ShowOptions();
    }
}

int main()
{
#if defined(_WIN16) || defined(_WIN32) || defined(_WIN64)
  //printf("windows");
   system("chcp 65001");
	system("cls");
#elif defined(__linux__)
  //	printf("linux");
  system("clear");
#elif defined(__APPLE__)
	// printf("apple");
  system("clear");
#else
	printf("未知系统，不知道如何清屏\n");
#endif    
   
    //system("clear||@cls");
    printf("\033[32;1m");
    isThisFirstTime();
}
