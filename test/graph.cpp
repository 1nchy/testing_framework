#include "test.hpp"

size_t _traversal = 0;

ICY_CASE("graph") { // CASE 1
    ++_traversal;
    ICY_SUBCASE("dijkstra") { // SUBCASE 1
        ++_traversal;
    }
    ICY_SUBCASE("floyd") { // SUBCASE 2
        ++_traversal;
        ICY_SUBCASE("A") { // SUBCASE 3
            ++_traversal;
            EXPECT_EQ(_traversal, 8);
        }
        ICY_SUBCASE("B") { // SUBCASE 4
            ++_traversal;
        }
    }
    ICY_SUBCASE("bfs") {
        ++_traversal;
        ICY_SUBCASE("visit") {
            ++_traversal;
            ICY_SUBCASE("A") {
                ++_traversal;
            }
            ICY_SUBCASE("B") {
                ++_traversal;
            }
        }
        ICY_SUBCASE("modify") {
            ++_traversal;
            EXPECT_EQ(_traversal, 27);
        }
    }
    ICY_SUBCASE("dfs") {
        ++_traversal;
        EXPECT_EQ(_traversal, 29);
    }
}
ICY_CASE("undirected_graph") { // CASE 2
    ++_traversal;
    ICY_SUBCASE("all") { // SUBCASE 5
        ++_traversal;
        EXPECT_EQ(_traversal, 32);
    }
}