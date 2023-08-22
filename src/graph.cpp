#include "graph.hpp"
#include "logger.hpp"

#include <algorithm>

namespace loader
{
    struct node
    {
        std::string name;
        std::vector<std::shared_ptr<node>> edges;

      public:
        bool handled;
    };

    struct graph::impl
    {
        using node_ptr = std::shared_ptr<node>;

      public:
        std::vector<node_ptr> nodes;

      public:
        void solve_recurse(std::vector<std::string> &, const node_ptr &);
    };

    graph::graph() : m_impl(std::make_unique<impl>()) {}

    graph::~graph() = default;

    void graph::add_node(std::string name)
    {
        auto node = std::make_shared<struct node>();
        node->name = std::move(name);

        m_impl->nodes.emplace_back(node);
    }

    void graph::add_edge(std::string from, std::string to)
    {
        auto &nodes = m_impl->nodes;

        auto from_node = std::find_if(nodes.begin(), nodes.end(), [&](auto &x) { return x->name == from; });
        auto to_node = std::find_if(nodes.begin(), nodes.end(), [&](auto &x) { return x->name == to; });

        if (from_node == nodes.end() || to_node == nodes.end())
        {
            logger::get()->error(R"(can't create edge because "{}" or "{}" does not exist)", from, to);
            return;
        }

        (*from_node)->edges.emplace_back(*to_node);
    }

    // NOLINTNEXTLINE(misc-no-recursion)
    void graph::impl::solve_recurse(std::vector<std::string> &rtn, const node_ptr &node)
    {
        node->handled = true;

        for (const auto &other : node->edges)
        {
            if (std::ranges::find(rtn, other->name) == rtn.end() && other->handled)
            {
                logger::get()->warn(R"(circular dependency "{0}" -> "{1}", "{0}" will be loaded first)", node->name,
                                    other->name);

                continue;
            }

            if (other->handled)
            {
                continue;
            }

            solve_recurse(rtn, other);
        }

        if (std::ranges::find(rtn, node->name) != rtn.end())
        {
            return;
        }

        rtn.emplace_back(node->name);
    }

    std::vector<std::string> graph::resolve_dependencies()
    {
        std::vector<std::string> rtn;

        if (m_impl->nodes.empty())
        {
            return rtn;
        }

        m_impl->solve_recurse(rtn, m_impl->nodes.at(0));

        return rtn;
    }
} // namespace loader