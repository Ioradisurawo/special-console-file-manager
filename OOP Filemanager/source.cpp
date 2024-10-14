#include "filemanager_console.h"
#include "filemanager_special.h"

int main()
{
    std::unique_ptr<FilemanagerBase> fm = std::make_unique<SpecialFilemanager>();
    if (fm->Run() == true)
    {
        fm = nullptr; fm = std::make_unique<SpecialFilemanager>();
        fm->Run();
    }
    return 0;
}