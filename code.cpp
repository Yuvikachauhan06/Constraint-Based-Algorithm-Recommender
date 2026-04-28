#include <bits/stdc++.h>
using namespace std;

struct Candidate {
    string name;
    int score;
    string timeComplexity;
    string spaceComplexity;
    vector<string> reasons;
    vector<string> warnings;
    bool rejected = false;  
};

struct AnalysisResult {
    string problem;
    string constraints;
    int n;
    string requiredComplexity;
    vector<Candidate> candidates;
    Candidate best;
    vector<string> reasoningTrace;
    vector<Candidate> rejectedCandidates;
    int confidence;   
};

class AlgorithmAdvisor {
private:
    unordered_map<string, vector<string>> dataset;
    unordered_map<string, pair<string, string>> algorithmComplexity;

    void initComplexityMap() {
        algorithmComplexity["SLIDING_WINDOW"] = {"O(N)", "O(1)"};
        algorithmComplexity["MONOTONIC_STACK"] = {"O(N)", "O(N)"};
        algorithmComplexity["HEAP"] = {"O(N log K)", "O(K)"};
        algorithmComplexity["DIJKSTRA"] = {"O(E log V)", "O(V + E)"};
        algorithmComplexity["BELLMAN_FORD"] = {"O(V * E)", "O(V)"};
        algorithmComplexity["BFS"] = {"O(V + E)", "O(V)"};
        algorithmComplexity["DFS"] = {"O(V + E)", "O(V)"};
        algorithmComplexity["DP_LIS"] = {"O(N log N)", "O(N)"};
        algorithmComplexity["DP"] = {"O(N^2) or better", "O(N)"};
        algorithmComplexity["BACKTRACKING"] = {"O(N!)", "O(N)"};
        algorithmComplexity["NQUEEN"] = {"O(N!)", "O(N)"};
        algorithmComplexity["SEGMENT_TREE"] = {"O(log N) per op", "O(N)"};
        algorithmComplexity["FENWICK_TREE"] = {"O(log N) per op", "O(N)"};
        algorithmComplexity["KMP"] = {"O(N + M)", "O(M)"};
        algorithmComplexity["RABIN_KARP"] = {"O(N + M)", "O(1)"};
        algorithmComplexity["TRIE"] = {"O(L) per word", "O(alphabet * L)"};
        algorithmComplexity["GAME_THEORY"] = {"O(N)", "O(N)"};
        algorithmComplexity["MINIMAX"] = {"O(B^D)", "O(D)"};
        algorithmComplexity["GREEDY"] = {"O(N log N)", "O(1)"};
        algorithmComplexity["INTERVAL"] = {"O(N log N)", "O(1)"};
        algorithmComplexity["BIPARTITE"] = {"O(V + E)", "O(V)"};
        algorithmComplexity["UNION_FIND"] = {"O(alpha(N))", "O(N)"};
        algorithmComplexity["MST"] = {"O(E log V)", "O(V + E)"};
        algorithmComplexity["TOPO_SORT"] = {"O(V + E)", "O(V)"};
        algorithmComplexity["FLOYD_WARSHALL"] = {"O(V^3)", "O(V^2)"};
        algorithmComplexity["FFT"] = {"O(N log N)", "O(N)"};
        algorithmComplexity["MO_ALGORITHM"] = {"O((N+Q)√N)", "O(√N)"};
        algorithmComplexity["CHINESE_REMAINDER"] = {"O(N log M)", "O(1)"};
        algorithmComplexity["SUFFIX_ARRAY"] = {"O(N log N)", "O(N)"};
        algorithmComplexity["SLIDING_WINDOW"] = {"O(N)", "O(1)"};
        algorithmComplexity["TWO_POINTER"] = {"O(N)", "O(1)"};
        algorithmComplexity["BINARY_SEARCH"] = {"O(log N)", "O(1)"};
        algorithmComplexity["KADANE"] = {"O(N)", "O(1)"};
        algorithmComplexity["CYCLE_DETECTION"] = {"O(V + E)", "O(V)"};
        algorithmComplexity["PERMUTATIONS"] = {"O(N!)", "O(N)"};
        algorithmComplexity["SUBSETS"] = {"O(2^N)", "O(N)"};
    }

    int extractN(const string& s) {
        int mx = 0, num = 0;
        for(char c : s) {
            if(isdigit(c)) num = num * 10 + (c - '0');
            else {
                if(num > mx) mx = num;
                num = 0;
            }
        }
        if(num > mx) mx = num;
        return mx;
    }

    string inferRequiredComplexity(int n) {
        if(n == 0) return "Unknown (N not specified)";
        if(n <= 20) return "O(2^N) or O(N!) acceptable (Exponential)";
        if(n <= 50) return "O(2^(N/2)) or O(N^3) acceptable";
        if(n <= 200) return "O(N^3) acceptable";
        if(n <= 2000) return "O(N^2) acceptable";
        if(n <= 50000) return "O(N log N) required";
        if(n <= 200000) return "O(N log N) or O(N√N) required";
        return "O(N) or O(N log N) strictly required";
    }

    void loadDataset() {
        ifstream file("problems.txt");
        if(!file) {
            dataset["DP_LIS"] = {"subsequence", "increasing", "lis"};
            dataset["DIJKSTRA"] = {"shortest", "path", "weighted", "dijkstra"};
            dataset["BACKTRACKING"] = {"permutation", "backtracking", "all combinations"};
            dataset["NQUEEN"] = {"queen", "chess", "nqueen"};
            dataset["BIPARTITE"] = {"bipartite", "two color"};
            dataset["SEGMENT_TREE"] = {"range", "query", "update"};
            dataset["KMP"] = {"pattern", "matching", "substring"};
            dataset["GAME_THEORY"] = {"player", "stone", "game", "nim"};
            dataset["BINARY_SEARCH"] = {"binary search", "sorted", "lower bound", "upper bound"};
            return;
        }

        string line;
        while(getline(file, line)) {
            if(line.empty() || line[0]=='#') continue;
            stringstream ss(line);
            string category, word;
            getline(ss, category, '|');
            vector<string> words;
            while(ss >> word) words.push_back(word);
            dataset[category] = words;
        }
    }

    string normalize(string s) {
        for(char &c : s) {
            c = tolower(c);
            if(!isalnum(c)) c = ' ';
        }
        return s;
    }

public:
    AlgorithmAdvisor() {
        loadDataset();
        initComplexityMap();
    }

    AnalysisResult analyze(const string& problem, const string& constraints) {
        AnalysisResult result;
        result.problem = problem;
        result.constraints = constraints;

        string norm = normalize(problem + " " + constraints);
        int n = extractN(constraints);
        result.n = n;
        result.requiredComplexity = inferRequiredComplexity(n);

        unordered_map<string, int> scores;
        unordered_map<string, vector<string>> reasons;
        
        // -------- GRAPH ALGORITHMS --------
        if(norm.find("shortest path") != string::npos || norm.find("shortest") != string::npos) {
            scores["DIJKSTRA"] += 25;
            reasons["DIJKSTRA"].push_back("'shortest path' keyword detected");
            scores["BELLMAN_FORD"] += 15;
            reasons["BELLMAN_FORD"].push_back("Alternative: handles negative edges");

            if(norm.find("negative") != string::npos) {
                scores["BELLMAN_FORD"] += 25;
                reasons["BELLMAN_FORD"].push_back("Negative edges detected - Dijkstra fails");
                scores["DIJKSTRA"] -= 15;
                reasons["DIJKSTRA"].push_back("Cannot handle negative weights");
            }

            if(norm.find("weighted") != string::npos) {
                scores["DIJKSTRA"] += 15;
                reasons["DIJKSTRA"].push_back("Weighted graph detected");
                scores["BFS"] -= 10;
            }

            if(norm.find("all pairs") != string::npos) {
                scores["FLOYD_WARSHALL"] += 30;
                reasons["FLOYD_WARSHALL"].push_back("All-pairs shortest path needed");
            }
        }
        if(norm.find("subarray") != string::npos ||
   norm.find("window") != string::npos) {
    scores["SLIDING_WINDOW"] += 30;
    reasons["SLIDING_WINDOW"].push_back("Subarray/window pattern detected");
}

if(norm.find("at most") != string::npos ||
   norm.find("at least") != string::npos) {
    scores["SLIDING_WINDOW"] += 15;
    reasons["SLIDING_WINDOW"].push_back("Constraint-based window condition");
}
if(norm.find("next greater") != string::npos ||
   norm.find("nearest greater") != string::npos ||
   norm.find("nearest smaller") != string::npos) {
    scores["MONOTONIC_STACK"] += 35;
    reasons["MONOTONIC_STACK"].push_back("Next/Nearest element pattern");
}
if(norm.find("k largest") != string::npos ||
   norm.find("k smallest") != string::npos ||
   norm.find("top k") != string::npos) {
    scores["HEAP"] += 35;
    reasons["HEAP"].push_back("Top-K element selection");
}
        // -------- DP/LIS --------
        if(norm.find("subsequence") != string::npos && norm.find("increasing") != string::npos) {
            scores["DP_LIS"] += 35;
            reasons["DP_LIS"].push_back("Longest Increasing Subsequence pattern");
        }

        // -------- BACKTRACKING --------
        if((norm.find("permutation") != string::npos || norm.find("all combinations") != string::npos) && n <= 20) {
            scores["BACKTRACKING"] += 25;
            reasons["BACKTRACKING"].push_back("All permutations/combinations needed");
            if(n <= 20) {
                scores["BACKTRACKING"] += 15;
                reasons["BACKTRACKING"].push_back("N <= 20 -> backtracking feasible");
            }
        }

        // -------- N-QUEENS --------
        if(norm.find("queen") != string::npos || norm.find("nqueen") != string::npos) {
            scores["NQUEEN"] += 40;
            reasons["NQUEEN"].push_back("N-Queens problem detected");
        }

        // -------- RANGE QUERIES --------
        if((norm.find("range") != string::npos || norm.find("query") != string::npos) &&
           (norm.find("update") != string::npos)) {
            scores["SEGMENT_TREE"] += 35;
            reasons["SEGMENT_TREE"].push_back("Range query + point/range update required");
            scores["FENWICK_TREE"] += 30;
            reasons["FENWICK_TREE"].push_back("Alternative: Fenwick Tree (simpler)");
        }

        // -------- STRING MATCHING --------
        if(norm.find("pattern") != string::npos || norm.find("substring") != string::npos) {
            scores["KMP"] += 30;
            reasons["KMP"].push_back("Pattern matching in string");
            scores["RABIN_KARP"] += 20;
            reasons["RABIN_KARP"].push_back("Alternative: Rabin-Karp (rolling hash)");
        }

        // -------- GAME THEORY --------
        if(norm.find("player") != string::npos || norm.find("stone") != string::npos ||
           norm.find("win") != string::npos || norm.find("nim") != string::npos) {
            scores["GAME_THEORY"] += 35;
            reasons["GAME_THEORY"].push_back("Two-player game detected");
            scores["MINIMAX"] += 25;
            reasons["MINIMAX"].push_back("Minimax for optimal decision making");
        }

        // -------- BIPARTITE --------
        if(norm.find("bipartite") != string::npos) {
            scores["BIPARTITE"] += 40;
            reasons["BIPARTITE"].push_back("Bipartite graph detection required");
        }

        // -------- INTERVAL --------
        if(norm.find("interval") != string::npos || norm.find("overlap") != string::npos) {
            scores["INTERVAL"] += 35;
            reasons["INTERVAL"].push_back("Interval scheduling/merging pattern");
            scores["GREEDY"] += 15;
        }

        if(norm.find("cycle") != string::npos) {
            scores["CYCLE_DETECTION"] += 35;
            reasons["CYCLE_DETECTION"].push_back("Cycle detection in graph required");
        }

        // -------- MST --------
        if(norm.find("spanning tree") != string::npos || norm.find("mst") != string::npos) {
            scores["MST"] += 35;
            reasons["MST"].push_back("Minimum Spanning Tree required");
        }

        // -------- MAX SUBARRAY --------
        if(norm.find("maximum subarray") != string::npos || norm.find("kadane") != string::npos) {
            scores["KADANE"] += 40;
            reasons["KADANE"].push_back("Maximum subarray sum pattern");
        }

        // -------- COUNT WAYS (DP) --------
        if(norm.find("ways") != string::npos || norm.find("count") != string::npos) {
            scores["DP"] += 20;
            reasons["DP"].push_back("Counting ways -> Dynamic Programming");
        }

        // -------- BINARY SEARCH ON ANSWER (Aggressive Cows style) --------
        if(norm.find("minimum maximum") != string::npos ||
           norm.find("maximize minimum") != string::npos ||
           norm.find("maximum minimum") != string::npos) {
            scores["BINARY_SEARCH"] += 45;
            reasons["BINARY_SEARCH"].push_back("Maximin/Minmax pattern -> Binary Search on Answer");
        }

        if((norm.find("aggressive") != string::npos || norm.find("cows") != string::npos) &&
           (norm.find("place") != string::npos || norm.find("distance") != string::npos)) {
            scores["BINARY_SEARCH"] += 50;
            reasons["BINARY_SEARCH"].push_back("Aggressive Cows / placement problem -> Binary Search on Answer");
        }

        // -------- BINARY SEARCH ON SORTED ARRAY --------
        if(norm.find("sorted") != string::npos) {
            if(norm.find("search") != string::npos || norm.find("find") != string::npos) {
                scores["BINARY_SEARCH"] += 30;
                reasons["BINARY_SEARCH"].push_back("Search in sorted array -> Binary Search");
            }
        }

        // -------- LOWER/UPPER BOUND --------
        if(norm.find("lower bound") != string::npos || norm.find("upper bound") != string::npos) {
            scores["BINARY_SEARCH"] += 35;
            reasons["BINARY_SEARCH"].push_back("Lower/Upper bound search -> Binary Search");
        }

        // -------- MONOTONIC CONDITION --------
        if(norm.find("monotonic") != string::npos) {
            scores["BINARY_SEARCH"] += 40;
            reasons["BINARY_SEARCH"].push_back("Monotonic condition -> Binary Search on Answer");
        }

        // -------- PREDICATE CHECK (typical binary search pattern) --------
        if((norm.find("can we") != string::npos || norm.find("possible") != string::npos) &&
           (norm.find("maximum") != string::npos || norm.find("minimum") != string::npos)) {
            scores["BINARY_SEARCH"] += 35;
            reasons["BINARY_SEARCH"].push_back("Feasibility check pattern -> Binary Search on Answer");
        }

        // -------- ADVANCED ALGORITHMS --------
        if(norm.find("convolution") != string::npos || norm.find("polynomial") != string::npos) {
            scores["FFT"] += 45;
            reasons["FFT"].push_back("FFT for convolution/polynomial multiplication");
        }

        if(norm.find("crt") != string::npos || norm.find("chinese remainder") != string::npos) {
            scores["CHINESE_REMAINDER"] += 40;
            reasons["CHINESE_REMAINDER"].push_back("Chinese Remainder Theorem for congruences");
        }

        // -------- CONSTRAINT-BASED SCORING --------
        if(n <= 20) {
            if(scores["BACKTRACKING"] > 0) scores["BACKTRACKING"] += 10;
        }

        if(n > 50000 && n <= 200000) {
            if(scores["DIJKSTRA"] > 0) scores["DIJKSTRA"] += 5;
            if(scores["SEGMENT_TREE"] > 0) scores["SEGMENT_TREE"] += 5;
            if(scores["KMP"] > 0) scores["KMP"] += 5;
            reasons["DIJKSTRA"].push_back("Large N -> O(N log N) algorithm required");
        }

        if(n > 200000) {
            if(scores["FLOYD_WARSHALL"] > 0) scores["FLOYD_WARSHALL"] -= 10;
        }

        // -------- DATASET MATCHING (lower weight) --------
        for(auto& entry : dataset) {
            for(string keyword : entry.second) {
                if(norm.find(keyword) != string::npos) {
                    scores[entry.first] += 2;
                    reasons[entry.first].push_back("Keyword: '" + keyword + "'");
                }
            }
        }

        // -------- BUILD CANDIDATE LIST --------
        vector<Candidate> candidates;
        for(auto& p : scores) {
        if(p.second > 0) {
        Candidate c;
        c.name = p.first;
        c.score = p.second;
        c.reasons = reasons[p.first];

        // complexity assignment 
        if(algorithmComplexity.count(p.first)) {
            c.timeComplexity = algorithmComplexity[p.first].first;
            c.spaceComplexity = algorithmComplexity[p.first].second;
        } else {
            c.timeComplexity = "Depends on implementation";
            c.spaceComplexity = "Depends on implementation";
        }
    string t = c.timeComplexity;

if(!c.rejected) {

    // Exponential
    if((t.find("!") != string::npos || t.find("2^") != string::npos) && n > 20) {
        c.rejected = true;
        c.warnings.push_back("Exponential time complexity not feasible");
    }

    // Cubic
    else if(t.find("^3") != string::npos && n > 1000) {
        c.rejected = true;
        c.warnings.push_back("Cubic time complexity too slow");
    }

    // Quadratic (optional but strong)
    else if(t.find("^2") != string::npos && n > 50000) {
        c.rejected = true;
        c.warnings.push_back("Quadratic time complexity too slow");
    }
}


        candidates.push_back(c);  // existing line
    }
}

        vector<Candidate> valid, rejected;

        for(auto &c : candidates) {
        if(c.rejected) rejected.push_back(c);
        else valid.push_back(c);
    }
// sort only valid
sort(valid.begin(), valid.end(), [](const Candidate& a, const Candidate& b) {
    return a.score > b.score;
}); 
// assign to result
result.candidates = valid;
result.rejectedCandidates = rejected;
// best algorithm
if(valid.empty() || valid[0].score < 5) {
    result.best = Candidate{"UNKNOWN", 0, "?", "?", {}, {}};
    result.confidence = 0;
} else {
    result.best = valid[0];
}
// keep top 5
if(valid.size() > 5) valid.resize(5);
int total = 0;
for(auto &c : valid) total += c.score;

if(total > 0)
    result.confidence = (result.best.score * 100) / total;
else
    result.confidence = 0;


        // Build reasoning trace
        result.reasoningTrace.push_back("========== ANALYSIS TRACE ==========");
        result.reasoningTrace.push_back("1. CONSTRAINT PARSING:");
        result.reasoningTrace.push_back("   N = " + (n == 0 ? "Not specified" : to_string(n)));
        result.reasoningTrace.push_back("   Required complexity: " + result.requiredComplexity);
        result.reasoningTrace.push_back("");
        result.reasoningTrace.push_back("2. PATTERN DETECTION & SCORING:");
        for(auto& c : candidates) {
            result.reasoningTrace.push_back("   [" + c.name + "] Score: " + to_string(c.score));
            for(auto& r : c.reasons) {
                string temp = r;
                if(temp.length() > 60) temp = temp.substr(0, 57) + "...";
                result.reasoningTrace.push_back("      * " + temp);
            }
        }
        result.reasoningTrace.push_back("");
        result.reasoningTrace.push_back("3. FINAL RECOMMENDATION:");
        result.reasoningTrace.push_back("   Selected: " + result.best.name);

        return result;
    }
};

// ==================== DISPLAY FUNCTIONS ====================

void displayResult(const AnalysisResult& result) {
    cout << "\n";
    cout << "===============================================================================\n";
    cout << "                         ALGORITHM ADVISOR v2.0                                \n";
    cout << "                       (Rule-Based Expert System)                     \n";
    cout << "===============================================================================\n";

    cout << "\n[INPUT]\n";
    cout << "-------------------------------------------------------------------------------\n";

    string problem = result.problem;
    cout << "Problem      : " << problem << "\n";
    cout << "Constraints  : " << result.constraints << "\n";
    if(result.n > 0) {
        cout << "Extracted N  : " << result.n << "\n";
        cout << "Required     : " << result.requiredComplexity << "\n";
    }
    cout << "-------------------------------------------------------------------------------\n";

    cout << "\nREASONING TRACE\n";
    cout << "-------------------------------------------------------------------------------\n";
    for(const string& line : result.reasoningTrace) {
        cout << line << "\n";
    }
    cout << "-------------------------------------------------------------------------------\n";

    cout << "\nCANDIDATE ALGORITHMS (Ranked by score)\n";
    cout << "-------------------------------------------------------------------------------\n";
    printf("%-20s | %-10s | %-25s | %-15s\n", "Algorithm", "Score", "Time Complexity", "Space Complexity");
    cout << "---------------------+------------+---------------------------+-----------------\n";
    for(const auto& c : result.candidates) {
        printf("%-19s | %-10d | %-25s | %-15s\n",
               c.name.c_str(), c.score, c.timeComplexity.c_str(), c.spaceComplexity.c_str());
    }
    cout << "-------------------------------------------------------------------------------\n";

    cout << "\nBEST RECOMMENDATION\n";
    cout << "-------------------------------------------------------------------------------\n";
    cout << "Algorithm       : " << result.best.name << "\n";
    cout << "Time Complexity : " << result.best.timeComplexity << "\n";
    cout << "Space Complexity: " << result.best.spaceComplexity << "\n";

    if(!result.best.warnings.empty()) {
        cout << "\nWARNINGS:\n";
        for(const auto& w : result.best.warnings) {
            cout << "  * " << w << "\n";
        }
    }
    cout << "\nREJECTED ALGORITHMS\n";
cout << "-------------------------------------------------------------------------------\n";

if(result.rejectedCandidates.empty()) {
    cout << "None\n";
} else {
    for(const auto &c : result.rejectedCandidates) {
        cout << c.name << " rejected:\n";
        for(const auto &w : c.warnings) {
            cout << "  * " << w << "\n";
        }
    }
}
    cout << "\nWHY THIS ALGORITHM?\n";
    for(const auto& r : result.best.reasons) {
        cout << "  * " << r << "\n";
    }
    cout << "-------------------------------------------------------------------------------\n";

    cout << "\nIMPLEMENTATION TEMPLATE\n";
    cout << "-------------------------------------------------------------------------------\n";

    if(result.best.name == "DIJKSTRA") {
        cout << R"(
// Dijkstra's Algorithm - O(E log V)
vector<long long> dijkstra(int n, vector<vector<pair<int, int>>>& adj, int src) {
    vector<long long> dist(n, LLONG_MAX);
    priority_queue<pair<long long, int>, vector<pair<long long, int>>, greater<>> pq;
    dist[src] = 0;
    pq.push({0, src});

    while(!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if(d > dist[u]) continue;
        for(auto [v, w] : adj[u]) {
            if(dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}
)";
    }
    else if(result.best.name == "DP_LIS") {
        cout << R"(
// Longest Increasing Subsequence - O(N log N)
int LIS(vector<int>& arr) {
    vector<int> tails;
    for(int x : arr) {
        auto it = lower_bound(tails.begin(), tails.end(), x);
        if(it == tails.end()) tails.push_back(x);
        else *it = x;
    }
    return tails.size();
}
)";
    }
    else if(result.best.name == "BACKTRACKING") {
        cout << R"(
// Backtracking Template - O(N!)
void backtrack(vector<int>& nums, vector<bool>& used, vector<int>& curr, vector<vector<int>>& result) {
    if(curr.size() == nums.size()) {
        result.push_back(curr);
        return;
    }
    for(int i = 0; i < nums.size(); i++) {
        if(!used[i]) {
            used[i] = true;
            curr.push_back(nums[i]);
            backtrack(nums, used, curr, result);
            curr.pop_back();
            used[i] = false;
        }
    }
}
)";
    }
    else if(result.best.name == "NQUEEN") {
        cout << R"(
// N-Queens - Backtracking O(N!)
void solveNQueens(int n, int row, vector<int>& cols, int& count) {
    if(row == n) {
        count++;
        return;
    }
    for(int col = 0; col < n; col++) {
        bool safe = true;
        for(int prev = 0; prev < row; prev++) {
            if(cols[prev] == col || abs(cols[prev] - col) == abs(prev - row)) {
                safe = false;
                break;
            }
        }
        if(safe) {
            cols[row] = col;
            solveNQueens(n, row + 1, cols, count);
        }
    }
}
)";
    }
    else if(result.best.name == "SEGMENT_TREE") {
        cout << R"(
// Segment Tree - O(log N) per operation
class SegmentTree {
    vector<int> tree;
    int n;
public:
    SegmentTree(vector<int>& arr) {
        n = arr.size();
        tree.resize(4 * n);
        build(arr, 1, 0, n - 1);
    }

    void build(vector<int>& arr, int node, int l, int r) {
        if(l == r) tree[node] = arr[l];
        else {
            int mid = (l + r) / 2;
            build(arr, node*2, l, mid);
            build(arr, node*2+1, mid+1, r);
            tree[node] = tree[node*2] + tree[node*2+1];
        }
    }

    void update(int idx, int val, int node, int l, int r) {
        if(l == r) tree[node] = val;
        else {
            int mid = (l + r) / 2;
            if(idx <= mid) update(idx, val, node*2, l, mid);
            else update(idx, val, node*2+1, mid+1, r);
            tree[node] = tree[node*2] + tree[node*2+1];
        }
    }

    int query(int ql, int qr, int node, int l, int r) {
        if(qr < l || ql > r) return 0;
        if(ql <= l && r <= qr) return tree[node];
        int mid = (l + r) / 2;
        return query(ql, qr, node*2, l, mid) + query(ql, qr, node*2+1, mid+1, r);
    }
};
)";
    }
    else if(result.best.name == "KMP") {
        cout << R"(
// KMP String Matching - O(N + M)
vector<int> buildKMP(string& pattern) {
    vector<int> lps(pattern.size());
    int len = 0;
    for(int i = 1; i < pattern.size(); ) {
        if(pattern[i] == pattern[len]) lps[i++] = ++len;
        else if(len) len = lps[len-1];
        else lps[i++] = 0;
    }
    return lps;
}

vector<int> kmpSearch(string& text, string& pattern) {
    vector<int> lps = buildKMP(pattern);
    vector<int> matches;
    int j = 0;
    for(int i = 0; i < text.size(); ) {
        if(text[i] == pattern[j]) i++, j++;
        if(j == pattern.size()) {
            matches.push_back(i - j);
            j = lps[j-1];
        }
        else if(i < text.size() && text[i] != pattern[j]) {
            if(j) j = lps[j-1];
            else i++;
        }
    }
    return matches;
}
)";
    }
    else if(result.best.name == "BIPARTITE") {
        cout << R"(
// Bipartite Graph Check - O(V + E)
bool isBipartite(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<int> color(n, -1);

    for(int i = 0; i < n; i++) {
        if(color[i] == -1) {
            queue<int> q;
            q.push(i);
            color[i] = 0;

            while(!q.empty()) {
                int u = q.front(); q.pop();
                for(int v : graph[u]) {
                    if(color[v] == -1) {
                        color[v] = color[u] ^ 1;
                        q.push(v);
                    }
                    else if(color[v] == color[u]) return false;
                }
            }
        }
    }
    return true;
}
)";
    }
    else if(result.best.name == "KADANE") {
        cout << R"(
// Kadane's Algorithm - O(N)
int maxSubarraySum(vector<int>& arr) {
    int maxSoFar = arr[0];
    int maxEndingHere = arr[0];

    for(int i = 1; i < arr.size(); i++) {
        maxEndingHere = max(arr[i], maxEndingHere + arr[i]);
        maxSoFar = max(maxSoFar, maxEndingHere);
    }
    return maxSoFar;
}
)";
    }
    else if(result.best.name == "BINARY_SEARCH") {
        cout << R"(
// Binary Search on Answer Template - O(log R) * O(check)
// For problems like: Aggressive Cows, Painter's Partition, etc.

bool canPlace(vector<int>& stalls, int cows, int minDist) {
    int count = 1;
    int lastPos = stalls[0];

    for(int i = 1; i < stalls.size(); i++) {
        if(stalls[i] - lastPos >= minDist) {
            count++;
            lastPos = stalls[i];
            if(count >= cows) return true;
        }
    }
    return count >= cows;
}

int aggressiveCows(vector<int>& stalls, int cows) {
    sort(stalls.begin(), stalls.end());

    int low = 1;
    int high = stalls.back() - stalls[0];
    int result = 0;

    while(low <= high) {
        int mid = low + (high - low) / 2;

        if(canPlace(stalls, cows, mid)) {
            result = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return result;
}

// Binary Search on Sorted Array
int binarySearch(vector<int>& arr, int target) {
    int left = 0, right = arr.size() - 1;

    while(left <= right) {
        int mid = left + (right - left) / 2;

        if(arr[mid] == target) return mid;
        else if(arr[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}
)";
    }
    else if(result.best.name == "CYCLE_DETECTION") {
        cout << R"(
// Cycle Detection in Directed Graph - O(V + E)
bool hasCycle(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<int> visited(n, 0);

    function<bool(int)> dfs = [&](int u) {
        visited[u] = 1;
        for(int v : graph[u]) {
            if(visited[v] == 1) return true;
            if(visited[v] == 0 && dfs(v)) return true;
        }
        visited[u] = 2;
        return false;
    };

    for(int i = 0; i < n; i++) {
        if(visited[i] == 0 && dfs(i)) return true;
    }
    return false;
}
)";
    }
    else if(result.best.name == "MST") {
        cout << R"(
// Kruskal's MST - O(E log V)
struct DSU {
    vector<int> parent, rank;
    DSU(int n) {
        parent.resize(n);
        rank.resize(n);
        for(int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int x) {
        if(parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }
    bool unite(int x, int y) {
        int rx = find(x), ry = find(y);
        if(rx == ry) return false;
        if(rank[rx] < rank[ry]) parent[rx] = ry;
        else if(rank[rx] > rank[ry]) parent[ry] = rx;
        else { parent[ry] = rx; rank[rx]++; }
        return true;
    }
};

int kruskal(int n, vector<tuple<int,int,int>>& edges) {
    sort(edges.begin(), edges.end());
    DSU dsu(n);
    int totalWeight = 0, edgesUsed = 0;
    for(auto& [w, u, v] : edges) {
        if(dsu.unite(u, v)) {
            totalWeight += w;
            edgesUsed++;
            if(edgesUsed == n-1) break;
        }
    }
    return totalWeight;
}
)";
    }
    else if(result.best.name == "GAME_THEORY") {
        cout << R"(
// Game Theory - Nim Game
// XOR of all piles: if 0, first player loses
bool canWin(vector<int>& piles) {
    int xorsum = 0;
    for(int p : piles) xorsum ^= p;
    return xorsum != 0;
}
)";
    }
    else {
        cout << R"(
// General Template for: )" << result.best.name << R"(

// 1. Understand the problem constraints
// 2. Identify the optimal substructure
// 3. Choose appropriate data structures
// 4. Implement the core algorithm
// 5. Test with edge cases

// Consider using:
// - Dynamic Programming for overlapping subproblems
// - Greedy for locally optimal choices
// - Divide & Conquer for independent subproblems
// - Graph algorithms for network problems
)";
    }
    cout << "Confidence      : " << result.confidence << "%\n";
    cout << "-------------------------------------------------------------------------------\n";
    cout << "\n";
}

void runTests() {
    ifstream file("testcases.txt");
    if(!file) {
        cout << "testcases.txt not found! Creating default test cases...\n";
        ofstream out("testcases.txt");
        out << "Problem: Find longest increasing subsequence of array\n";
        out << "Constraints: n <= 100000\n";
        out << "Expected: DP_LIS\n\n";
        out << "Problem: Find shortest path in weighted graph\n";
        out << "Constraints: n <= 100000, weights >= 0\n";
        out << "Expected: DIJKSTRA\n\n";
        out << "Problem: Solve N queens problem\n";
        out << "Constraints: n <= 12\n";
        out << "Expected: NQUEEN\n\n";
        out << "Problem: Aggressive Cows - Place cows to maximize minimum distance\n";
        out << "Constraints: n <= 100000, cows <= n\n";
        out << "Expected: BINARY_SEARCH\n";
        out.close();
        file.open("testcases.txt");
    }

    AlgorithmAdvisor advisor;
    string line, problem = "", constraints = "", expected = "";
    int testNum = 1;

    cout << "\n";
    cout << "===============================================================================\n";
    cout << "                         RUNNING TEST SUITE                                    \n";
    cout << "===============================================================================\n";

    while(getline(file, line)) {
        if(line.find("Problem:") != string::npos) {
            problem = line.substr(line.find(":") + 1);
            while(!problem.empty() && problem[0] == ' ') problem.erase(0, 1);
        }
        else if(line.find("Constraints:") != string::npos) {
            constraints = line.substr(line.find(":") + 1);
            while(!constraints.empty() && constraints[0] == ' ') constraints.erase(0, 1);
        }
        else if(line.find("Expected:") != string::npos) {
            expected = line.substr(line.find(":") + 1);
            while(!expected.empty() && expected[0] == ' ') expected.erase(0, 1);

            cout << "\n===============================================================================\n";
            cout << "TEST CASE " << testNum++ << "\n";
            cout << "===============================================================================\n";

            AnalysisResult result = advisor.analyze(problem, constraints);

            cout << "\n[TEST RESULT]\n";
            cout << "-------------------------------------------------------------------------------\n";
            cout << "Expected: " << expected << "\n";
            cout << "Got:      " << result.best.name << "\n";

            if(expected == result.best.name) {
                cout << "Status:   PASSED\n";
            } else {
                cout << "Status:   FAILED\n";
                cout << "Note:     System's recommendation may be a better match\n";
            }
            cout << "-------------------------------------------------------------------------------\n";

            displayResult(result);

            problem = "";
            constraints = "";
            expected = "";
        }
    }

    cout << "\n===============================================================================\n";
    cout << "                    TEST SUITE COMPLETE (" << (testNum-1) << " tests)              \n";
    cout << "===============================================================================\n";
}

// ==================== MAIN ====================

int main() {
    AlgorithmAdvisor advisor;

    cout << "\n";
    cout << "===============================================================================\n";
    cout << "                    WELCOME TO ALGORITHM ADVISOR v2.0                         \n";
    cout << "===============================================================================\n";
    cout << "                                                                               \n";
    cout << "  This tool analyzes problem statements and recommends the best algorithm     \n";
    cout << "  based on patterns and constraints. No AI is used - everything is rule-based.\n";
    cout << "                                                                               \n";
    cout << "  Features:                                                                   \n";
    cout << "  * Constraint-based complexity analysis                                      \n";
    cout << "  * Multiple candidate algorithms with scoring                                \n";
    cout << "  * Reasoning trace for transparency                                          \n";
    cout << "  * Code templates for recommended algorithms                                 \n";
    cout << "  * Binary Search detection (Aggressive Cows, etc.)                          \n";
    cout << "===============================================================================\n";

    while(true) {
        cout << "\n";
        cout << "===============================================================================\n";
        cout << "                              MAIN MENU                                       \n";
        cout << "===============================================================================\n";
        cout << "  1. Analyze from input.txt                                                   \n";
        cout << "  2. Run test suite (testcases.txt)                                           \n";
        cout << "  3. Exit                                                                     \n";
        cout << "===============================================================================\n";
        cout << "Choice: ";

        int ch;
        if(!(cin >> ch)) {
            cout << "Invalid input. Exiting...\n";
            return 0;
        }

        if(ch == 2) {
            runTests();
            continue;
        }

        if(ch == 3) {
            cout << "\n===============================================================================\n";
            cout << "                    Thank you for using Algorithm Advisor!                     \n";
            cout << "===============================================================================\n";
            break;
        }

        if(ch == 1) {
            ifstream input("input.txt");
            if(!input) {
                cout << "\ninput.txt not found!\n";
                cout << "Creating sample input.txt...\n";
                ofstream out("input.txt");
                out << "Problem: Given a weighted graph, find shortest path from source to all nodes\n";
                out << "Constraints: 1 <= n <= 100000, edge weights >= 0\n";
                out.close();
                input.open("input.txt");
            }

            string line, problem = "", constraints = "";
            while(getline(input, line)) {
                if(line.find("Problem:") != string::npos) {
                    problem = line.substr(line.find(":") + 1);
                    while(!problem.empty() && problem[0] == ' ') problem.erase(0, 1);
                }
                else if(line.find("Constraints:") != string::npos) {
                    constraints = line.substr(line.find(":") + 1);
                    while(!constraints.empty() && constraints[0] == ' ') constraints.erase(0, 1);
                }
            }

            if(problem.empty()) {
                cout << "\nInvalid input.txt format!\n";
                cout << "Expected format:\n";
                cout << "Problem: <your problem description>\n";
                cout << "Constraints: <constraints>\n";
                continue;
            }

            AnalysisResult result = advisor.analyze(problem, constraints);
            displayResult(result);

            ofstream out("outputs.txt", ios::app);
            out << "========================================\n";
            out << "Problem: " << problem << "\n";
            out << "Constraints: " << constraints << "\n";
            out << "Recommended: " << result.best.name << "\n";
            out << "Score: " << result.best.score << "\n";
            out << "========================================\n";
            out.close();

            cout << "\nResult saved to outputs.txt\n";
        }
    }

    return 0;
}
