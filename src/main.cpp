#ifdef CLI_ONLY

int main (int argc, char * argv[]) {
    printf("CLI currently not supported, please wait for later or sumn\n");
    fflush(stdout);
}


#else

#include <cstdint>
#include "binIncludes.cpp"
#include "Instance.cpp"

int main(int argc, char * argv[])
{    
    if (argc > 1)
        printf("CLI currently not supported, please wait for later or sumn\n");

    Instance instance;

    // Font stuff
    instance.addMonospaceFont(bin_font_data, bin_font_size, bin_codepages, bin_codepages_size);

    while (instance.isWindowOpen())
    {
        instance.ProcessEvents();
        instance.Update();
    }

    return 0;
}

#endif