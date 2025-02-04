#include "filemanager_console.h"
#include "filemanager_special.h"
#include <curses.h>
#include <fstream>
// https://www.ibiblio.org/pub/linux/docs/howto/other-formats/pdf/NCURSES-Programming-HOWTO.pdf

class PDCursesConsole
{
public:
    PDCursesConsole()
    {
        setlocale(LC_ALL, "Russian");
        initscr(); /* Start curses mode */
        init_pair(0, BACKGROUND_GREEN, FOREGROUND_BLUE);
        printw("Hello World !!!"); /* Print Hello World */
        refresh(); /* Print it on to the real screen */
        keypad(stdscr, true); // enable func keys
        noecho(); 
        printw("\nx: %d\ny: %d", stdscr->_maxx, stdscr->_maxy);
        addch(ACS_BULLET);
        wprintw(stdscr, "Нюклиар страйк инбаунд: "); /* print to exact screen */


        getch(); /* Wait for user input */
        getch(); /* Wait for user input */
    }
    ~PDCursesConsole()
    {
        endwin(); /* End curses mode */
    }
};

//
void GenerateFiles(std::filesystem::path initial_path)
{
    int max_depth = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(initial_path))
    {
        std::cout << entry << "\n";
    }
}

int main()
{
    //std::unique_ptr<FilemanagerBase> fm = std::make_unique<ConsoleFilemanager>();
    //if (fm->Run() == true)
    //{
    //    fm = nullptr; fm = std::make_unique<SpecialFilemanager>();
    //    fm->Run();
    //}

    GenerateFiles("D:\\");

    return 0;
}