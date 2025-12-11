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

const double INF = 1e9; // Use double for precise calculations

// --- 1. STRUCTURE TO HOLD DATA ---
struct Edge {
    int to;
    double cost;      // Financial Cost
    double delay;     // Latency (ms)
    double bandwidth; // Bandwidth (Mbps)
};

struct graph {
    int vertexCount;
    
    // Adjacency List stores neighbors and edge properties
    vector<vector<Edge>> adjList;
    
    map<string, int> nameToIndex;
    vector<string> indexToName;
    vector<long> nodeBandwidths;
    vector<bool> nodeStatus; 

    // --- NEW: Global Weights (Stored here so they are remembered!) ---
    double alpha = 0.5; // Default Cost Priority (50%)
    double beta = 0.3;  // Default Delay Priority (30%)
    double gamma = 0.2; // Default Bandwidth Priority (20%)

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

    // --- NEW FEATURE: Update Weights Dynamically ---
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

    // --- CONNECTION FUNCTION ---
    void add_connection_symmetric(string u_name, string v_name, double cost, double delay) {
        if (nameToIndex.find(u_name) == nameToIndex.end() || 
            nameToIndex.find(v_name) == nameToIndex.end()) {
            cout << "[ERROR] Router not found!" << endl;
            return;
        }

        int u = nameToIndex[u_name];
        int v = nameToIndex[v_name];

        // Bottleneck Logic: The link is only as fast as the slowest router
        double link_bw = min((double)nodeBandwidths[u], (double)nodeBandwidths[v]);

        // Check if connection already exists (Update it if it does)
        for(auto& edge : adjList[u]) {
            if(edge.to == v) {
                cout << "[INFO] Connection exists. Updating values." << endl;
                edge.cost = cost;
                edge.delay = delay;
                edge.bandwidth = link_bw;
                // Update the reverse path too
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

        // Add new connection (Symmetric = Two-way)
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

    // --- DIJKSTRA ALGORITHM ---
    void dijkstra(string startName) {
        if (nameToIndex.find(startName) == nameToIndex.end()) return;
        
        int start = nameToIndex[startName];
        if (!nodeStatus[start]) {
            cout << "\n[CRITICAL] Source router " << startName << " is DOWN." << endl;
            return;
        }

        vector<double> dist(vertexCount, INF);
        vector<int> parent(vertexCount, -1);
        
        // Priority Queue: Always puts the smallest 'dist' at the top
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
                if (!nodeStatus[v]) continue; // Skip dead routers

                // --- THE SCALED FORMULA ---
                // We use the member variables 'alpha', 'beta', 'gamma' here!
                // (1.0 / bandwidth) * 1,000,000 scales the small decimal up so it matters.
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

        // --- PRINTING THE ROUTING TABLE ---
        cout << "\n" << string(95, '=') << endl;
        cout << "                ROUTING TABLE (Alpha=" << alpha << ", Beta=" << beta << ", Gamma=" << gamma << ")" << endl;
        cout << string(95, '=') << endl;
        cout << left << setw(15) << "Dest" << "| " << setw(15) << "Eff. Cost" << "| " << setw(15) << "Next Hop" << "| " << "Full Path" << endl;
        cout << string(95, '-') << endl;

        for (int i = 0; i < vertexCount; i++) {
            if (i == start) continue; 

            cout << left << setw(15) << indexToName[i] << "| ";

            if (!nodeStatus[i]) {
                cout << setw(15) << "INF" << "| " << setw(15) << "-" << "| HOST DOWN" << endl;
                continue;
            }

            if (dist[i] == INF) {
                cout << setw(15) << "Unreachable" << "| " << setw(15) << "-" << "| No Path" << endl;
            } else {
                cout << setw(15) << fixed << setprecision(2) << dist[i] << "| ";
                
                // Reconstruct Path Logic (Backtracking from Target -> Source)
                int curr = i;
                vector<string> pathStack;
                while (curr != -1) {
                    pathStack.push_back(indexToName[curr]);
                    curr = parent[curr];
                }

                // Next Hop is the second to last element
                if (pathStack.size() >= 2) 
                    cout << setw(15) << pathStack[pathStack.size() - 2] << "| ";
                else 
                    cout << setw(15) << "-" << "| ";

                // Print Full Path
                for (int k = pathStack.size() - 1; k >= 0; k--) {
                    cout << pathStack[k];
                    if (k > 0) cout << " -> ";
                }
                cout << endl;
            }
        }
        cout << string(95, '=') << endl;
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
        cout << "Node " << i + 1 << ": ";
        cin >> name >> bw;
        g.set_node_data(i, name, bw);
    }

    // --- INPUT CONNECTIONS ---
    cout << "\nDefine Connections (Format: [Neighbor] [Cost] [Delay])" << endl;
    cout << "Example: If A connects to B with Cost 50 and Delay 10, type: B 50 10" << endl;
    
    for (int i = 0; i < n; i++) {
        string u_name = g.indexToName[i];
        cout << "Node " << u_name << " connected to (Type 'DONE' to finish):" << endl;
        
        string v_name;
        while (cin >> v_name) {
            if (v_name == "DONE") break;
            
            double cost, delay;
            // We must read Cost and Delay for the formula to work!
            if (!(cin >> cost >> delay)) break; 
            
            g.add_connection_symmetric(u_name, v_name, cost, delay);
        }
    }

    string startNode;
    cout << "\nSet Source Router: ";
    cin >> startNode;

    while(true) {
        g.dijkstra(startNode); // Runs immediately so you see results

        cout << "\n[MENU]" << endl;
        cout << "1: Kill Router        2: Revive Router" << endl;
        cout << "3: Change Source      4: Add Connection" << endl;
        cout << "5: ADD NEW ROUTER     6: Exit" << endl;
        cout << "7: CHANGE WEIGHTS (Alpha, Beta, Gamma)" << endl;
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
            // Check if node exists before setting
            if (g.nameToIndex.find(target) != g.nameToIndex.end()) {
                startNode = target;
            } else {
                cout << "[ERROR] Node does not exist." << endl;
            }
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
            cout << "\nEnter new weights (Sum should be 1.0 ideally):" << endl;
            cout << "Alpha (Cost Priority): "; cin >> a;
            cout << "Beta (Delay Priority): "; cin >> b;
            cout << "Gamma (BW Priority):   "; cin >> c;
            g.update_weights(a, b, c);
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

<img width="756" height="489" alt="image" src="https://github.com/user-attachments/assets/309210d4-367e-48d9-b70d-be5ba53d7673" />

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

0.1

0.1

0.8

A

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

0.1
0.1
0.8

A
```
Up to that point, it will give an output as shown below (not this exact one, but something similiar to this... point is, it will print out the routig table for each iterations/update you do)

<img width="853" height="200" alt="image" src="https://github.com/user-attachments/assets/f966260e-5417-411e-ae5a-244840ff73f1" />
