# GraphTheoryFP
| Name           | NRP        |
| ---            | ---        |
| Alif Muflih Jauhary | 5025241003 |
| Makna Alam Pratama | 5025241077 |
| Rayen Yeriel Mangiwa | 5025241262 |

## Raw Code

```cpp
#include <bits/stdc++.h>
#include <iomanip>
using namespace std;

const double INF = 1e9; 

// --- STRUCTURE TO HOLD DATA ---
struct Edge {
    int to;
    double cost;      
    double delay;     
    double bandwidth; 
};

struct graph {
    int vertexCount;
    
    vector<vector<Edge>> adjList;
    map<string, int> nameToIndex;
    vector<string> indexToName;
    vector<long> nodeBandwidths;
    vector<bool> nodeStatus; 

    // --- GLOBAL WEIGHTS ---
    double alpha = 0.5; 
    double beta = 0.3;  
    double gamma = 0.2; 

    void init(int v) {
        vertexCount = v;
        indexToName.resize(v);
        nodeBandwidths.resize(v);
        nodeStatus.resize(v, true); 
        adjList.clear();
        adjList.resize(v);
    }

    void set_node_data(int index, string name, long bandwidth) {
        nameToIndex[name] = index;
        indexToName[index] = name;
        nodeBandwidths[index] = bandwidth;
    }

    void update_weights(double a, double b, double c) {
        alpha = a;
        beta = b;
        gamma = c;
        cout << "\n[SUCCESS] Updated Priorities:" << endl;
        cout << "Alpha (Cost): " << alpha << endl;
        cout << "Beta (Delay): " << beta << endl;
        cout << "Gamma (BW):   " << gamma << endl;
    }

    void add_new_router(string name, long bandwidth) {
        if (nameToIndex.find(name) != nameToIndex.end()) {
            cout << "[ERROR] Router name '" << name << "' already exists!" << endl;
            return;
        }
        int newIndex = vertexCount;
        vertexCount++; 
        nameToIndex[name] = newIndex;
        indexToName.push_back(name);
        nodeBandwidths.push_back(bandwidth);
        nodeStatus.push_back(true); 
        adjList.resize(vertexCount);
        cout << "[SUCCESS] Added Router '" << name << "' (Index: " << newIndex << ")" << endl;
    }

    void add_connection_symmetric(string u_name, string v_name, double cost, double delay) {
        if (nameToIndex.find(u_name) == nameToIndex.end() || 
            nameToIndex.find(v_name) == nameToIndex.end()) {
            cout << "[ERROR] Router not found!" << endl;
            return;
        }

        int u = nameToIndex[u_name];
        int v = nameToIndex[v_name];
        double link_bw = min((double)nodeBandwidths[u], (double)nodeBandwidths[v]);

        for(auto& edge : adjList[u]) {
            if(edge.to == v) {
                cout << "[INFO] Connection exists. Updating values." << endl;
                edge.cost = cost;
                edge.delay = delay;
                edge.bandwidth = link_bw;
                for(auto& rev_edge : adjList[v]) {
                    if(rev_edge.to == u) {
                        rev_edge.cost = cost;
                        rev_edge.delay = delay;
                        rev_edge.bandwidth = link_bw;
                    }
                }
                return;
            }
        }

        adjList[u].push_back({v, cost, delay, link_bw});
        adjList[v].push_back({u, cost, delay, link_bw});
        cout << "[SUCCESS] Connected: " << u_name << " <--> " << v_name 
             << " [Cost:" << cost << ", Delay:" << delay << ", BW:" << link_bw << "]" << endl;
    }

    void toggle_router(string name, bool status) {
        if (nameToIndex.find(name) == nameToIndex.end()) return;
        nodeStatus[nameToIndex[name]] = status;
        if(status) cout << "[EVENT] Router " << name << " REBOOTED." << endl;
        else       cout << "[EVENT] Router " << name << " CRASHED." << endl;
    }

    // ==========================================
    // METHOD 1: MODIFIED DIJKSTRA
    // ==========================================
    void dijkstra(string startName) {
        if (nameToIndex.find(startName) == nameToIndex.end()) return;
        int start = nameToIndex[startName];
        if (!nodeStatus[start]) { cout << "\n[CRITICAL] Source is DOWN.\n"; return; }

        vector<double> dist(vertexCount, INF);
        vector<int> parent(vertexCount, -1);
        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

        pq.push({0, start});
        dist[start] = 0;

        while (!pq.empty()) {
            double d = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (d > dist[u]) continue;

            for (auto& edge : adjList[u]) {
                int v = edge.to;      
                if (!nodeStatus[v]) continue; 

                // THE FORMULA
                double effective_weight = (alpha * edge.cost) + 
                                          (beta * edge.delay) + 
                                          (gamma * (1.0 / edge.bandwidth) * 1000000.0);

                if (dist[u] + effective_weight < dist[v]) {
                    dist[v] = dist[u] + effective_weight;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }
        print_table("MODIFIED DIJKSTRA", start, dist, parent);
    }

    // ==========================================
    // METHOD 2: BELLMAN-FORD 
    // ==========================================
    void bellman_ford(string startName) {
        if (nameToIndex.find(startName) == nameToIndex.end()) return;
        int start = nameToIndex[startName];
        if (!nodeStatus[start]) { return; }

        vector<double> dist(vertexCount, INF);
        vector<int> parent(vertexCount, -1);
        dist[start] = 0;

        // Iterate V-1 times
        for (int i = 0; i < vertexCount - 1; i++) {
            bool changed = false;
            for (int u = 0; u < vertexCount; u++) {
                if (dist[u] == INF) continue;
                for (auto& edge : adjList[u]) {
                    int v = edge.to;
                    if (!nodeStatus[v]) continue;

                    double weight = (alpha * edge.cost) + (beta * edge.delay) + 
                                    (gamma * (1.0 / edge.bandwidth) * 1000000.0);

                    if (dist[u] + weight < dist[v]) {
                        dist[v] = dist[u] + weight;
                        parent[v] = u;
                        changed = true;
                    }
                }
            }
            if (!changed) break;
        }
        print_table("BELLMAN-FORD (Safe/Slow)", start, dist, parent);
    }

    // ==========================================
    // METHOD 3: BFS
    // ==========================================
    void bfs(string startName) {
        if (nameToIndex.find(startName) == nameToIndex.end()) return;
        int start = nameToIndex[startName];
        if (!nodeStatus[start]) { return; }

        vector<double> hops(vertexCount, INF);
        vector<int> parent(vertexCount, -1);
        queue<int> q;

        q.push(start);
        hops[start] = 0;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (auto& edge : adjList[u]) {
                int v = edge.to;
                if (!nodeStatus[v]) continue;

                if (hops[v] == INF) {
                    hops[v] = hops[u] + 1;
                    parent[v] = u;
                    q.push(v);
                }
            }
        }
        print_table("BFS (Shortest Hops Only)", start, hops, parent, true);
    }

    void print_table(string title, int start, vector<double>& dist, vector<int>& parent, bool isBFS = false) {
        cout << "\n" << string(90, '=') << endl;
        cout << "           ALGORITHM: " << title << endl;
        cout << string(90, '=') << endl;
        if (isBFS) cout << left << setw(15) << "Dest" << "| " << setw(15) << "Hops Count" << "| " << "Path" << endl;
        else       cout << left << setw(15) << "Dest" << "| " << setw(15) << "Total Cost" << "| " << "Path" << endl;
        cout << string(90, '-') << endl;

        for (int i = 0; i < vertexCount; i++) {
            if (i == start) continue;
            cout << left << setw(15) << indexToName[i] << "| ";

            if (!nodeStatus[i]) {
                cout << setw(15) << "DOWN" << "| HOST DOWN" << endl; continue;
            }
            if (dist[i] == INF) {
                cout << setw(15) << "Unreach" << "| No Path" << endl; continue;
            }

            cout << setw(15) << fixed << setprecision(2) << dist[i] << "| ";
            
            // Print Path
            int curr = i;
            vector<string> pathStack;
            while (curr != -1) {
                pathStack.push_back(indexToName[curr]);
                curr = parent[curr];
            }
            for (int k = pathStack.size() - 1; k >= 0; k--) {
                cout << pathStack[k];
                if (k > 0) cout << " -> ";
            }
            cout << endl;
        }
        cout << string(90, '=') << endl;
    }
};

int main() {
    graph g;
    int n;
    
    cout << "How many initial nodes: ";
    if (!(cin >> n)) return 0;
    
    g.init(n);

    cout << "Enter Node Name and Bandwidth (e.g., 'A 1000'):" << endl;
    for (int i = 0; i < n; i++) {
        string name;
        long bw;
        cout << "Node " << i + 1 << ": "; cin >> name >> bw;
        g.set_node_data(i, name, bw);
    }

    cout << "\nDefine Connections (Format: [Neighbor] [Cost] [Delay])" << endl;
    cout << "Example: If A connects to B with Cost 50 and Delay 10, type: B 50 10" << endl;
    
    for (int i = 0; i < n; i++) {
        string u_name = g.indexToName[i];
        cout << "Node " << u_name << " connected to (Type 'DONE' to finish):" << endl;
        string v_name;
        while (cin >> v_name) {
            if (v_name == "DONE") break;
            double cost, delay;
            if (!(cin >> cost >> delay)) break; 
            g.add_connection_symmetric(u_name, v_name, cost, delay);
        }
    }

    cout << "\n" << string(50, '-') << endl;
    cout << ">>> CONFIGURATION: Set Routing Priorities <<<" << endl;
    double start_a, start_b, start_c;
    cout << "Alpha (Cost): "; cin >> start_a;
    cout << "Beta (Delay): "; cin >> start_b;
    cout << "Gamma (BW):   "; cin >> start_c;
    g.update_weights(start_a, start_b, start_c);
    cout << string(50, '-') << endl;

    string startNode;
    cout << "\nSet Source Router: "; cin >> startNode;

    while(true) {
        
        g.dijkstra(startNode); 

        cout << "\n[MENU]" << endl;
        cout << "1: Kill Router        2: Revive Router" << endl;
        cout << "3: Change Source      4: Add Connection" << endl;
        cout << "5: ADD NEW ROUTER     6: Exit" << endl;
        cout << "7: CHANGE WEIGHTS     8: COMPARE ALL ALGORITHMS" << endl;
        cout << "Select: ";
        int choice;
        if (!(cin >> choice)) break;

        if (choice == 6) break;
        
        string target, target2;
        if (choice == 1) {
            cout << "Target to KILL: "; cin >> target;
            g.toggle_router(target, false);
        }
        else if (choice == 2) {
            cout << "Target to REVIVE: "; cin >> target;
            g.toggle_router(target, true);
        }
        else if (choice == 3) {
            cout << "New Source: "; cin >> target;
            if (g.nameToIndex.find(target) != g.nameToIndex.end()) startNode = target;
            else cout << "[ERROR] Node does not exist." << endl;
        }
        else if (choice == 4) {
            double c, d;
            cout << "Connect Node: "; cin >> target;
            cout << "With Node: ";    cin >> target2;
            cout << "Cost: ";         cin >> c;
            cout << "Delay: ";        cin >> d;
            g.add_connection_symmetric(target, target2, c, d);
        }
        else if (choice == 5) {
            string name;
            long bw;
            cout << "New Router Name: "; cin >> name;
            cout << "Bandwidth: ";       cin >> bw;
            g.add_new_router(name, bw);
        }
        else if (choice == 7) {
            double a, b, c;
            cout << "\nEnter new weights:" << endl;
            cout << "Alpha (Cost): "; cin >> a;
            cout << "Beta (Delay): "; cin >> b;
            cout << "Gamma (BW):   "; cin >> c;
            g.update_weights(a, b, c);
        }
        else if (choice == 8) {
            cout << "\n\n>>> RUNNING ALGORITHM COMPARISON <<<" << endl;
            g.dijkstra(startNode);
            g.bellman_ford(startNode);
            g.bfs(startNode);
            
            cout << "\n[ANALYSIS]" << endl;
            cout << "- Dijkstra:    Fastest, Best for Weighted Graphs." << endl;
            cout << "- Bellman:     Slow, Good for Negative Weights (useless here)." << endl;
            cout << "- BFS:         Shortest HOPS only (Ignores Cost/Bandwidth)." << endl;
            cout << "Press any key/letter to continue..." << endl;
            string dummy; cin >> dummy;
        }
    }
    return 0;
}

```
## Basic Explanation Regarding The Code
This is an updated version from our last (mostly failed attempt) network router simulator.
It lets you Build a network of routers
Each connection has:
Cost
Delay (how slow it is)
Bandwidth (how fast/big it is)

We can decide what matters most:
"I want the cheapest path" → prioritize cost
"I want the fastest path" → prioritize low delay
"I want the highest speed path" → prioritize bandwidth

Users can also break routers (simulate crashes) and see what happens!
on top of that, they can add new routers anytime
It instantly shows you the best path from one router to all others
## How to use:
1. Start the program
   You will be asked how many nodes initially.
2. Enter the routers name and its bandwidth.
3. Connect the nodes; type "DONE" when finnished.
4. Pick the source
5. A Menu will pop up giving you options on what you want to do.
6. Pick whichever one you desire
## Flowchart

<img width="701" height="451" alt="FlowchartForGraphTheoryFP" src="https://github.com/user-attachments/assets/fefb1350-db94-49ee-8fa1-40d5a49a839b" />

## Sample test case
Input:

3

A 1000

B 500

C 1000

B 20 30

C 100 5

DONE

C 60 40

DONE


DONE

A

7

0.2

0.7

0.1

1

B

2

B

5

D 2000

4

A

D

10 15

D

C

5 8

6

Output:
Routing table based on the current inputs
example from the given input case:

```
3

A 1000

B 500

C 1000

B 20 30

C 100 5

DONE

C 60 40

DONE

DONE

A
```
Up to that point, it will give an output as shown below (not this exact one, but something similiar to this... point is, it will print out the routig table for each iterations/update you do)

<img width="2026" height="345" alt="image" src="https://github.com/user-attachments/assets/d5e99020-66bb-4e43-a824-d2c99393b283" />
