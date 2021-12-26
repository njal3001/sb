#include <stdint.h>

namespace SB
{
    class Game
    {
    private:
        uint64_t m_target_ticks;
        uint64_t m_prev_ticks;
        float m_elapsed; // In seconds

    public:
        Game(const unsigned int target_fps);
        void run();

    private:
        void limit_fps();
    };
}
