#include "sb/ecs.h"
#include <algorithm>
#include "sb/collider.h"
#include "sb/enemy.h"

namespace SB
{
    using namespace Engine;

    Scene::Scene(const int tile_size, const int width, const int height)
        : m_tile_size(tile_size), m_width(width), m_height(height),
        m_collision_manager(CollisionManager(this)), m_navigation_manager(NavigationManager(this))
    {}

    Scene::~Scene()
    {
        for (auto e : m_to_add)
        {
            delete e;
        }

        for (auto e : m_entities)
        {
            delete e;
        }
    }

    Entity* Scene::add_entity(const Engine::Vec2& pos)
    {
        Entity* entity = new Entity(pos);
        entity->m_scene = this;

        m_to_add.push_back(entity);

        return entity;
    }

    void Scene::track_component(Component* component)
    {
        m_components[component->type()].push_back(component);
    }

    void Scene::untrack_component(Component* component)
    {
        assert(component->scene() == this);

        auto& c_vec = m_components[component->m_type];
        c_vec.erase(std::find(std::begin(c_vec), std::end(c_vec), component));
    }

    const NavigationManager* Scene::navigation_manager() const
    {
        return &m_navigation_manager;
    }

    void Scene::update(float elapsed)
    {
        update_lists();

        for (auto e : m_entities)
        {
            e->update_lists();
        }

        m_navigation_manager.update();

        for (auto& c_vec : m_components)
        {
            for (auto c : c_vec)
            {
                if (c->m_alive && c->entity()->m_alive)
                {
                    c->update(elapsed);
                }
            }
        }

        m_collision_manager.update();
    }

    void Scene::update_lists()
    {
        auto it = m_entities.begin();

        while (it != m_entities.end())
        {
            Entity* e = *it;
            if (!e->m_alive)
            {
                it = m_entities.erase(it);

                e->removed();
                delete e;
            }
            else
            {
                it++;
            }
        }

        for (auto entity : m_to_add)
        {
            m_entities.push_back(entity);
        }

        m_to_add.clear();
    }

    void Scene::render(Engine::Renderer* renderer)
    {
        for (const auto& c_vec : m_components)
        {
            for (auto c : c_vec)
            {
                if (c->alive() && c->visible && c->entity()->visible)
                {
                    c->render(renderer);
                }
            }
        }

        m_navigation_manager.render_grid(renderer);
    }

    void Scene::all(std::vector<Collider*>* out, const uint32_t mask) const
    {
        const uint8_t type = Component::Types::id<Collider>();
        auto& comp_vec = m_components[type];

        for (Component* comp : comp_vec)
        {
            auto collider = (Collider*)comp;
            if (collider->alive() && (collider->mask & mask))
            {
                out->push_back(collider);
            }
        }
    }

    size_t Scene::tile_size() const
    {
        return m_tile_size;
    }

    size_t Scene::width() const
    {
        return m_width;
    }

    size_t Scene::height() const
    {
        return m_height;
    }
}
