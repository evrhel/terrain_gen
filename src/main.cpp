#include "engine.h"

int main(int argc, char *argv[])
{
    initAll(argc, argv);

    while (beginFrame())
    {
        updateAll();
        endFrame();
    }

    quitAll();
    return 0;
}
