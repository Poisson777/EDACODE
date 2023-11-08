#include "head.h"

int sqrt(int x) {
    int l = 0, r = x, res = 0;
    while(l <= r) {
        int mid = (l + r) >> 1;
        if (mid * mid >= x) {
            r = mid - 1;
            res = mid;
        } else {
            l = mid + 1;
        }
    }
    return res;
}

std::vector<int> get_network(const std::string filename) {
    std::ifstream network_fs(filename);

    std::vector<int> mat;
    int x;
    while (network_fs >> x) {
        mat.emplace_back(x);
    }
    network_fs.close();

    return mat;
}

int str_to_int(const std::string &s, int l, int r) {
    int res = 0;
    for(int i = l; i < r; ++i) {
        res = (res << 3) + (res << 1);
        res += s[i] - '0';
    }
    return res;
}

bool is_digit(char c) {
    return c <= '9' && c >= '0';
}

Trie<int> get_node_die_table(const std::string filename) {
    // make node-to-die mapping table with Trie
    Trie<int> trie;
    std::ifstream node_fs(filename);
    std::string buffer;
    while(std::getline(node_fs, buffer)) {
        while(buffer.back() == '\r' || buffer.back() == ' ' || buffer.back() == '\n') buffer.pop_back();
        assert(buffer[0] == 'D' && buffer[1] == 'i' && buffer[2] == 'e');
        int l = 3, r = 3, n = buffer.size();
        while(is_digit(buffer[r])) ++r;
        int die_index = str_to_int(buffer, l, r);
        assert(buffer[r] == ':');
        ++r;
        for(l = r; l < n; l = r) {
            if(buffer[l] == ' ') {
                r = l + 1;
                continue;
            }

            r = l;
            while(buffer[r] != ' ' && r < n) {
                ++r;
            }
            trie.add(buffer, l, r, die_index);
        }
    }
    node_fs.close();
    return trie;
}

Trie<int> get_die_FPGA_table(const std::string filename) {
    // make die-to-FPGA mapping table with Trie
    Trie<int> trie;
    std::ifstream die_fs(filename);
    std::string buffer;
    while(std::getline(die_fs, buffer)) {
        while(buffer.back() == '\r' || buffer.back() == ' ' || buffer.back() == '\n') buffer.pop_back();
        assert(buffer[0] == 'F' && buffer[1] == 'P' && buffer[2] == 'G' && buffer[3] == 'A');
        int l = 4, r = 4, n = buffer.size();
        while(is_digit(buffer[r])) ++r;
        int FPGA_index = str_to_int(buffer, l, r);
        assert(buffer[r] == ':');
        ++r;
        for(l = r; l < n; l = r) {
            if(buffer[l] == ' ') {
                r = l + 1;
                continue;
            }

            r = l;
            while(buffer[r] != ' ' && r < n) {
                ++r;
            }

            assert(r - l > 3);
            assert(buffer[l] == 'D' && buffer[l + 1] == 'i' && buffer[l + 2] == 'e');
            trie.add(buffer, l + 3, r, FPGA_index); // auto skip "Die"
        }
    }
    die_fs.close();
    return trie;
}

std::string filename_prefix() {
    std::string input_id = "1";
    // std::cin >> input_id;
    const std::string prefix = "./TestCase20230927/testcase";
    const std::string suffix = "/design.";
    return prefix + input_id + suffix;
}

void layout() {
    std::vector<int> adj;
    get_network(filename_prefix() + "die.network").swap(adj);
    int n = sqrt(adj.size());

    // get the weight of edge
    std::function<int(int,int)> weight = [&](int x, int y) {
        return adj[x * n + y];
    };

    Trie<int> gs = get_node_die_table(filename_prefix() + "die.position");
    Trie<int> dies = get_die_FPGA_table(filename_prefix() + "fpga.die"); // auto skip "Die" with assertions

    auto search = [&](int die_s, const std::vector<int> &dies_l) {
        struct Elem {
            int v, fa, dist;
            Elem(int v, int fa, int dist) : v(v), fa(fa), dist(dist) {}
        };

        std::queue<Elem> q;
        std::vector<int> from(n), vis(n);
        q.emplace(die_s, -1, 0);
        while(!q.empty()) {
            auto [v, fa, dist] = q.front();
            q.pop();

            if(vis[v]) continue;

            from[v] = fa;

            for(int to = 0; to < n; ++to) if(to != v && !vis[to]) {
                q.emplace(to, v, dist + 1);
            }
        }

        std::vector<std::pair<int,int>> edges;
        for(int l : dies_l) {
            for(int fa = from[l]; fa != -1;) {
                edges.emplace_back(fa, l);
                int temp = fa;
                fa = from[fa];
                from[temp] = -1;
            }
        }

        return edges;
    };

    while(true) {
        std::ifstream net_fs(filename_prefix() + "net");

        std::string node;
        char sl, temp;

        net_fs >> node >> sl >> temp;
        assert(net_fs && sl == 's'); // check if file is empty and the first is 's'
        int die_s = gs.query(node);
        std::vector<int> dies_l;

        while(true) {
            net_fs >> node >> sl;
            if(!net_fs || sl == 's') {
                net_fs >> temp;
                break;
            }
            dies_l.emplace_back(gs.query(node));
        }

        std::vector<std::pair<int,int>> res = search(die_s, dies_l);
        // TODO: decrease edge weight
    }

    return;
}