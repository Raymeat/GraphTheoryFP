
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
