#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio_ext.h>
#include <ctype.h>

#include "cgraph/cgraph.h"
#include "libfdr/jrb.h"

cgraph_t g;
cgraph_ivec_t edges;
cgraph_rvec_t weights;

JRB tree;

int curId = 0;
int demands[100];

int getVid(int location){
    JRB node = jrb_find_int(tree, location);
    if(!node) return -1;
    return node->val.i;
}

int getLocation(int vid){
    JRB ptr;
    jrb_traverse(ptr, tree){
        if(ptr->val.i == vid){
            return ptr->key.i;
        }
    }
    return -1;
}

void readLocation(int *n){
    FILE *fptr;
    if((fptr = fopen("location.txt", "r")) == NULL){
        printf("Khong mo duoc file\n");
        return;
    }
    fscanf(fptr, "%d\n", n);
    printf("%d\n", *n);
    // demands = (int*)malloc((*n)*sizeof(int));
    // Them dinh 0 cua noi xuat phat
    jrb_insert_int(tree, 0, (Jval){.i = curId});
    curId++;
    for(int i=0; i<*n; i++){
        int location, demand;
        fscanf(fptr, "%d %d\n", &location, &demand);
        printf("%d %d\n", location, demand);
        jrb_insert_int(tree, location, (Jval){.i = curId});
        demands[curId] = demand;
        curId++;
    }
    g = cgraph_create(edges, 0, CGRAPH_UNDIRECTED);
    cgraph_add_vertices(g, curId);
    fclose(fptr);
}

void readDistance(){
    FILE *fptr;
    if((fptr = fopen("distance.txt", "r")) == NULL){
        printf("Khong mo duoc file\n");
        return;
    }

    while(!feof(fptr)){
        int location1, location2, time;
        fscanf(fptr, "%d %d %d\n", &location1, &location2, &time);
        printf("%d %d %d\n", location1, location2, time);
        cgraph_ivec_push_back(&edges, getVid(location1));
        cgraph_ivec_push_back(&edges, getVid(location2));
        cgraph_rvec_push_back(&weights, time);
    }
    g = cgraph_create(edges, 0, CGRAPH_UNDIRECTED);
    fclose(fptr);
}

void sortByDemand(int n){
    int *demandsCopy = (int*)malloc(n*sizeof(int));
    int *ids = (int*)malloc(n*sizeof(int));
    for(int i=0; i<n; i++){
        demandsCopy[i] = demands[i];
        ids[i] = i;
    }
    for(int i=1; i<n; i++){
        for(int j=i+1; j<n; j++){
            if(demandsCopy[j] > demandsCopy[i]){
                int temp = demandsCopy[i];
                demandsCopy[i] = demandsCopy[j];
                demandsCopy[j] = temp;
                temp = ids[i];
                ids[i] = ids[j];
                ids[j] = temp;
            }
        }
    }

    for(int i=1; i<n; i++){
        int location = getLocation(ids[i]);
        printf("%d %d\n", location, demandsCopy[i]);
    }
    free(ids);
    free(demandsCopy);
}

void listNeighbors(){
    printf("Enter location: ");
    int location;
    scanf("%d", &location);
    if(getVid(location) == -1){
        printf("Location unexist!\n");
        return;
    }
    cgraph_ivec_t father = cgraph_ivec_create();
    cgraph_ivec_t dist = cgraph_ivec_create();
    cgraph_simple_bfs(g, getVid(location), CGRAPH_ALL, false, &father, &dist);
    printf("Neighbors list:");
    for(int i=0; i<cgraph_ivec_size(dist); i++){
        if(dist[i] > 0){
            // printf("%d: %d\n", i, dist[i]);
            printf(" %d", getLocation(i));
        }
    }
    printf("\n");
    cgraph_ivec_free(&father);
    cgraph_ivec_free(&dist);
}

void checkConnected(){
    printf("Enter location: ");
    int location;
    scanf("%d", &location);
    if(getVid(location) == -1){
        printf("Location unexist!\n");
        return;
    }
    cgraph_ivec_t father = cgraph_ivec_create();
    cgraph_ivec_t dist = cgraph_ivec_create();
    cgraph_simple_bfs(g, 0, CGRAPH_ALL, false, &father, &dist);
    if(dist[getVid(location)] < 0){
        printf("NO\n");
    }else{
        printf("YES\n");
    }
    cgraph_ivec_free(&father);
    cgraph_ivec_free(&dist);
}

int checkFeasibility(int location, int printMode){
    
    cgraph_ivec_t vpath = cgraph_ivec_create();
    cgraph_ivec_t epath = cgraph_ivec_create();
    cgraph_get_shortest_path_dijkstra(g, &vpath, &epath, 0, getVid(location)
    , weights, CGRAPH_ALL);
    double cost=0;
    for(int i=0; i<cgraph_ivec_size(epath); i++){
        cost += weights[epath[i]];
    }
    if(printMode)
    printf("Shortest time: %.0lf\n", cost);
    if(cost < 120){
        if(printMode)
        printf("YES\n");
        return 1;
    }else{
        if(printMode)
        printf("NO\n");
        return 0;
    }

    cgraph_ivec_free(&vpath);
    cgraph_ivec_free(&epath);
}

void checkFeasible(){
    JRB ptr;
    jrb_traverse(ptr, tree){
        if(ptr->key.i == 0) continue;
        if(checkFeasibility(ptr->key.i, 0) == 0){
            printf("NO\n");
            return;
        }
    }
    printf("YES\n");
}

int main(){
    tree = make_jrb();
    edges = cgraph_ivec_create();
    weights = cgraph_rvec_create();
    int choice, n, location;
    do{
        printf("=============MENU=============\n");
        printf("1. Input data\n");
        printf("2. Sort by demand\n");
        printf("3. List neighbors\n");
        printf("4. Check connected\n");
        printf("5. Check feasibility for location\n");
        printf("6. Check feasible solution\n");
        printf("7. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            readLocation(&n);
            readDistance();
            break;
        case 2:
            sortByDemand(n);
            break;
        case 3:
            listNeighbors();
            break;
        case 4:
            checkConnected();
            break;
        case 5:
            printf("Enter location: ");
            scanf("%d", &location);
            if(getVid(location) == -1){
            printf("Location unexist!\n");
                break;
            }
            checkFeasibility(location, 1);
            break;
        case 6:
            checkFeasible();
            break;
        case 7:
            printf("Bye...\n");
            
            break;
        default:
            printf("Khong hop le, nhap lai.\n");
            break;
        }
    }while(choice!=7);
    jrb_free_tree(tree);
    cgraph_ivec_free(&edges);
    cgraph_rvec_free(&weights);
}