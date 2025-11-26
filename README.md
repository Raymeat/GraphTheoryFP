# GraphTheoryFP
| Name           | NRP        |
| ---            | ---        |
| Alif Muflih Jauhary | 5025241003 |
| Makna Alam Pratama | 5025241077 |
| Rayen Yeriel Mangiwa | 5025241262 |

## Raw Code

```C

#include <stdio.h>

#include <stdlib.h>

#include <float.h>

#include <stdbool.h>

#include <string.h>



#define N 6

const char *labels[N] = {"ITS", "Singapore", "Japan", "Dubai", "London", "Eropa"};

const double INF = 1e9;



// === METRIC DATA ===

double cost[N][N] = {

    {INF, 5.2, INF, 8.0, INF, INF},   // ITS

    {5.2, INF, 2.5, INF, 6.0, INF},   // Singapore

    {INF, 2.5, INF, INF, INF, 7.5},   // Japan

    {8.0, INF, INF, INF, 4.5, INF},   // Dubai

    {INF, 6.0, INF, 4.5, INF, 3.0},   // London

    {INF, INF, 7.5, INF, 3.0, INF}    // Eropa

};



double delay[N][N] = {

    {INF, 1.2, INF, 3.0, INF, INF},

    {1.2, INF, 0.8, INF, 1.5, INF},

    {INF, 0.8, INF, INF, INF, 2.5},

    {3.0, INF, INF, INF, 1.1, INF},

    {INF, 1.5, INF, 1.1, INF, 0.9},

    {INF, INF, 2.5, INF, 0.9, INF}

};



double bandwidth[N][N] = {

    {0, 80, 0, 60, 0, 0},

    {80, 0, 100, 0, 70, 0},

    {0, 100, 0, 0, 0, 90},

    {60, 0, 0, 0, 85, 0},

    {0, 70, 0, 85, 0, 95},

    {0, 0, 90, 0, 95, 0}

};



// === BOBOT METRIK ===

double alpha = 0.5; // cost

double beta  = 0.3; // delay

double gamma = 0.2; // bandwidth (dibalik)



int parent[N];

double minCost[N];

bool visited[N];



int getNodeIndex(const char *name) {

    for (int i = 0; i < N; i++)

        if (strcasecmp(labels[i], name) == 0)

            return i;

    return -1;

}



// Hitung effective cost berdasarkan 3 parameter

double effectiveCost(int u, int v) {

    if (cost[u][v] == INF) return INF;

    // proteksi bandwidth 0 (no-edge) agar tidak div/0

    double bwInv = (bandwidth[u][v] > 0) ? (1.0 / bandwidth[u][v]) : DBL_MAX;

    return alpha * cost[u][v] + beta * delay[u][v] + gamma * bwInv;

}



int findMinNode() {

    double min = INF;

    int idx = -1;

    for (int i = 0; i < N; i++) {

        if (!visited[i] && minCost[i] < min) {

            min = minCost[i];

            idx = i;

        }

    }

    return idx;

}



void Dijkstra(int start, int target) {

    for (int i = 0; i < N; i++) {

        minCost[i] = INF;

        parent[i] = -1;

        visited[i] = false;

    }



    minCost[start] = 0;



    while (1) {

        int u = findMinNode();

        if (u == -1) break;

        visited[u] = true;

        if (u == target) break;



        for (int v = 0; v < N; v++) {

            double ec = effectiveCost(u, v);

            if (!visited[v] && ec < minCost[v]) {

                parent[v] = u;

                minCost[v] = ec;

            }

        }

    }

}



void printPath(int target) {

    if (parent[target] == -1) {

        printf("%s", labels[target]);

        return;

    }

    printPath(parent[target]);

    printf(" -> %s", labels[target]);

}



int main() {

    char startName[50], targetName[50];



    printf("Daftar router yang tersedia:\n");

    for (int i = 0; i < N; i++) printf("- %s\n", labels[i]);

    printf("\nMasukkan node awal: ");

    scanf("%49s", startName);

    printf("Masukkan node tujuan: ");

    scanf("%49s", targetName);



    int start = getNodeIndex(startName);

    int target = getNodeIndex(targetName);

    if (start == -1 || target == -1) {

        printf("Nama node tidak valid. Pastikan sesuai daftar di atas.\n");

        return 1;

    }



    Dijkstra(start, target);

    printPath(target);

    printf("\n");



    if (parent[target] == -1) {

        printf("Tidak ada jalur yang menghubungkan %s ke %s.\n", labels[start], labels[target]);

        return 0;

    }



    double totalCost = 0, totalDelay = 0;

    double bottleneckBandwidth = DBL_MAX; // kita cari min sepanjang path

    int cur = target;

    while (parent[cur] != -1) {

        int p = parent[cur];

        totalCost += cost[p][cur];

        totalDelay += delay[p][cur];

        if (bandwidth[p][cur] > 0 && bandwidth[p][cur] < bottleneckBandwidth) {

            bottleneckBandwidth = bandwidth[p][cur];

        }

        cur = p;

    }

    if (bottleneckBandwidth == DBL_MAX) bottleneckBandwidth = 0; // safety fallback

    

    printf("Total biaya         : %.2f\n", totalCost);

    printf("Total delay         : %.2f ms\n", totalDelay);

    printf("Bottleneck bandwidth: %.2f Mbps\n", bottleneckBandwidth);

    printf("Nilai efektif       : %.4f\n", minCost[target]);



    return 0;

}

```
## Basic Explanation Regarding The Code
This C program finds the **best path between two routers** by implementing **Dijkstra’s algorithm**, where the “best” edge is determined by a **weighted combination of three metrics**:

* **Cost** → weight = 0.5
* **Delay** → weight = 0.3
* **Bandwidth** (inverted, because bigger is better) → weight = 0.2

So instead of minimizing only cost, the algorithm minimizes:

```
effectiveCost = 0.5*cost + 0.3*delay + 0.2*(1/bandwidth)
```
**What the code does**

### 1. **Loads the network graph**

* The graph has 6 nodes: ITS, Singapore, Japan, Dubai, London, Eropa.
* Each pair of nodes has cost, delay, and bandwidth.
* INF means no connection.

**Steps**
### **(A) Read user input**

User inputs:

* Start node (e.g., “ITS”)
* Target node (e.g., “Eropa”)

The program maps these names to indices using `getNodeIndex()`.

### **(B) Run Dijkstra's algorithm**

The function `Dijkstra(start, target)` does:

1. Initialize all nodes

   * `minCost[i] = INF`
   * `parent[i] = -1`
   * `visited[i] = false`
2. Set start node cost → `minCost[start] = 0`
3. Repeatedly pick:

   * the unvisited node with the smallest `minCost`
4. Relax edges from this node using `effectiveCost(u, v)`
5. Stop early when the **target** node is added.

### **(C) Reconstruct the path**

`printPath(target)` prints the final route by following `parent[]`.

Example:

```
ITS -> Singapore -> London -> Eropa
```

## **(D) Compute metrics along the chosen path**

By walking backward from target to start:

* Sum **cost**
* Sum **delay**
* Track **minimum bandwidth** (bottleneck)
* Print final “effective” value from `minCost[target]`


### **FINAL OUTPUT**

The program shows:

* Path taken
* Total cost
* Total delay
* Bottleneck bandwidth
* Total effective score (the multi-metric combined cost)
