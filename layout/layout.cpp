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
    if(!network_fs.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << ".\n";
        exit(1);
    }

    std::vector<int> mat;
    int x;
    while (network_fs >> x) {
        mat.emplace_back(x);
    }
    network_fs.close();

    return mat;
}

bool is_digit(char c) {
    return c <= '9' && c >= '0';
}

int str_to_int(const std::string &s, int l, int r) {
    int res = 0;
    for(int i = l; i < r; ++i) {
        assert(is_digit(s[i]));
        res = (res << 3) + (res << 1);
        res += s[i] - '0';
    }
    return res;
}

Trie<int> get_node_die_table(const std::string filename) {
    // make node-to-die mapping table with Trie
    Trie<int> trie;
    std::ifstream node_fs(filename);
    if(!node_fs.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << ".\n";
        exit(1);
    }

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

std::vector<int> get_die_FPGA_table(const std::string filename) {
    // make die-to-FPGA mapping table with std::vector<int>
    std::vector<int> res;
    std::ifstream die_fs(filename);
    if(!die_fs.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << ".\n";
        exit(1);
    }

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
            int die_index = str_to_int(buffer, l + 3, r);
            if(die_index >= (int)res.size()) res.resize(die_index + 1);
            res[die_index] = FPGA_index;
        }
    }
    die_fs.close();
    return res;
}

std::string filename_prefix() {
    std::string input_id = "1";
    // std::cin >> input_id;
    const std::string prefix = "./TestCase20231027/testcase";
    const std::string suffix = "/design.";
    return prefix + input_id + suffix;
}

void layout() {
    std::vector<int> adj;
    get_network(filename_prefix() + "die.network").swap(adj);
    
    int n = sqrt(adj.size());

    // get the flow of edges, read-n-write, directly modify the reference variable
    auto flow = [&adj, &n](int x, int y) -> int& {
        // assert(x != y);
        if(x < y) std::swap(x, y);
        return adj[x * n + y];
    };

    // get the weight of edges, read-only
    auto weight = [&adj, &n](int x, int y) -> int {
        // assert(x != y);
        if(x > y) std::swap(x, y);
        return adj[x * n + y];
    };

    Trie<int> gs = get_node_die_table(filename_prefix() + "die.position");
    std::vector<int> dies = get_die_FPGA_table(filename_prefix() + "fpga.die"); // auto skip "Die" with assertions

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

            for(int to = 0; to < n; ++to) if(weight(v, to) < flow(v, to) && to != v && !vis[to]) {
                q.emplace(to, v, dist + 1);
            }
        }

        std::vector<std::pair<int,int>> edges;
        for(int l : dies_l) {
            for(int fa = from[l]; fa != -1;) {
                edges.emplace_back(fa, l);
                if(dies[fa] == dies[l]) ++flow(fa, l);
                int temp = fa;
                fa = from[fa];
                from[temp] = -1;
            }
        }

        return edges;
    };

    int line = 0, net_id;
    char sl, temp;
    std::string node;
    std::string filename = filename_prefix() + "net";
    std::ifstream net_fs(filename);
    if(!net_fs.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << ".\n";
        exit(1);
    }

    net_fs >> node >> sl >> temp;
    ++line;
    std::ofstream network_out(filename_prefix() + "route.out");

    while(true) {
        net_id = line;
        assert(net_fs && sl == 's'); // check if file is empty and the first is 's'
        int die_s = gs.query(node);
        std::vector<int> dies_l;

        while(true) {
            net_fs >> node >> sl;
            ++line;
            if(sl == 's') {
                net_fs >> temp;
                break;
            }else if(!net_fs) {
                break;
            }
            dies_l.emplace_back(gs.query(node));
        }

        std::vector<std::pair<int,int>> res = search(die_s, dies_l);
        // TODO: output the result
        std::cout << net_id << '\n';
    }

    net_fs.close();

    return;
}