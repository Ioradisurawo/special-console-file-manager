#include "filemanager_console.h"
#include "filemanager_special.h"
#include "language_select.h"
#include "localization.h"

int main()
{
    try
    {
        std::unique_ptr<LanguageSelect> ls_menu = std::make_unique<LanguageSelect>();
        ls_menu->Run();

        std::unique_ptr<FilemanagerBase> fm = std::make_unique<ConsoleFilemanager>();
        if (fm->Run() == true)
        {
            fm = nullptr; fm = std::make_unique<SpecialFilemanager>();
            fm->Run();
        }
    }
    catch (std::exception e)
    {
        std::cout << "Something went wrong\n\n" << e.what() << "\n\n";
        system("pause");
    }

    return 0;
}