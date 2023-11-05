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
    auto weight = [&](int x, int y) {
        return adj[x * n + y];
    };

    std::function<void(void)> search = [&]() {
        
    };
}