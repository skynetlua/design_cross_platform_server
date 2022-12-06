
//CMake include_directories(src)
#include <Robot.h>

int main()
{
    Robot robot;
    robot.start();
    robot.update();
    getchar();
    return 0;
}