#pragma once
#include <string>
#include <memory>
#include <vector>

namespace loader
{

    class graph
    {
        struct impl;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        graph();

      public:
        ~graph();

      public:
        void add_node(std::string name);

      public:
        void add_edge(std::string from, std::string to);

      public:
        std::vector<std::string> resolve_dependencies();
    };
} // namespace loader