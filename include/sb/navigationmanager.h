#pragma once
#include "engine/graphics/renderer.h"
#include "engine/maths/vec2.h"
#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace SB
{
    class Scene;
    class Collider;

    class NavigationManager
    {
    private:
        struct GridNode
        {
            enum class State
            {
                UNEXPLORED,
                OPEN,
                EXPLORED
            };

            size_t x, y;
            float g_val;
            float h_val;
            const GridNode* parent;
            State state;

            GridNode();
            float distance(const size_t x, const size_t y);
        };

    private:
        static constexpr float refresh_time = 0.5f;
        std::vector<bool> m_grid;

        Scene *m_scene;

    public:
        NavigationManager(Scene *scene);

        void update();
        size_t grid_index(const Engine::Vec2& pos) const;

        void render_grid(Engine::Renderer* renderer);

    private:
        size_t grid_index(const size_t x, const size_t y) const;
        bool in_bound(const int x, const int y) const;
        bool can_move(const int x, const int y) const;
        void prop_path(std::unordered_map<size_t, GridNode> *nodes,
                const GridNode *parent);
    };
}
