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

std::vector<int> get_network(std::string filename) {
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

Trie<int> get_node_die_table(std::string filename) {
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

    Trie<int> trie = get_node_die_table(filename_prefix() + "die.position");

    std::function<void(void)> search = [&]() {
        
    };
}