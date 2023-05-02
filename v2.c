#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_CHILDREN 4
#define MIN_CHILDREN 2

typedef struct point_t{
    double x;
    double y;
} Point;

//typedef struct point_t* Point;

typedef struct rect_t{
    double x_max;
    double x_min;
    double y_max;
    double y_min;
} Rectangle;

typedef struct node_t{
    int is_leaf;
    int num_children;
    struct node_t* children[MAX_CHILDREN];
    Rectangle* MBR[MAX_CHILDREN];
    Point* points[MAX_CHILDREN];
} Node;

//typedef struct node_t * Node;

Node * build_RTree(Node * nodes, int num_nodes);
Node * build_RTree_wrapper(Point * points, int num_points);
Node * create_leaf_node(Point * points, int num_points);
Rectangle * get_MBR_node(Node * node);
Rectangle * get_MBR_point(Point * point);
void STR_node(Node * nodes, int num_nodes);
void STR_points(Point * points, int num_points);
int compare_x(const void* a, const void*b);
int compare_y(const void* a, const void*b);
int compare_x_node(const void* a, const void* b);
int compare_y_node(const void* a, const void* b);



Node * build_RTree(Node * nodes, int num_nodes){
    STR_node(nodes, num_nodes);
    int num_of_parents = (int)ceil((double)num_nodes/MAX_CHILDREN);

    Node * parents = malloc(sizeof(Node) * num_of_parents);
    Node * parent;

    int starting_index;
    int ending_index;
    int test1,test2;

    for(int i = 0; i<num_of_parents; i++){
        starting_index = i*MAX_CHILDREN;
        ending_index = starting_index + MAX_CHILDREN;
        
        if (ending_index>num_nodes){
            ending_index = num_nodes;
        }

        if(ending_index == starting_index){
            break;
        }

        parent = (Node*)malloc(sizeof(Node));
        parent->num_children = ending_index - starting_index;
        parent->is_leaf = 0;
        test1 = parent->num_children;

        for(int j = 0; j<parent->num_children; j++){
            test2 = nodes[starting_index+j].num_children;
            parent->children[j] = &nodes[starting_index+j];
            parent->MBR[j] = get_MBR_node(&nodes[starting_index+j]);
            //printf("%lf\t %lf\t %lf\t %lf\n", parent->MBR[j]->x_max,parent->MBR[j]->x_min,parent->MBR[j]->y_max,parent->MBR[j]->y_min);
        }

        parents[i] = *parent;
    }

    if(num_of_parents == 1){
        return parents;
    }
    else{
        return build_RTree(parents, num_of_parents);
    }
}

Node * build_RTree_wrapper(Point * points, int num_points){
    
    STR_points(points, num_points);
    
    if(num_points <= MAX_CHILDREN){
        return create_leaf_node(points, num_points);
    }

    int num_of_nodes = (int)ceil((double)num_points/MAX_CHILDREN);
    
    Node * nodes = malloc(sizeof(Node)* num_of_nodes);
    Node * node;

    int starting_index;
    int ending_index;
    //int leaf;

    for(int i = 0; i<num_of_nodes; i++){
        starting_index = i*MAX_CHILDREN;
        ending_index = starting_index + MAX_CHILDREN;
        
        if (ending_index>num_points){
            ending_index = num_points;
        }

        node = create_leaf_node(&points[starting_index], ending_index-starting_index);
        //printf("%d %d\n", starting_index, ending_index);
        //node = malloc(sizeof(Node));
        //node->num_children = ending_index - starting_index;
        //node->is_leaf = 1;
        //leaf = node->is_leaf;

        //for(int j = 0; j<node->num_children; j++){
        //    node->points[j] = &points[starting_index+j];
         //   node->MBR[j] = get_MBR_point(&points[starting_index+j]);
            //printf("%lf\t %lf\t %lf\t %lf\n", node->MBR[j]->x_max,node->MBR[j]->x_min,node->MBR[j]->y_max,node->MBR[j]->y_min);
        //}

        nodes[i] = *node;
    }

    Node * roots = build_RTree(nodes, num_of_nodes);
    Node * root = &roots[0];
    return root;
}

Node * create_leaf_node(Point * points, int num_points){
    Node * node = malloc(sizeof(Node));
    node->num_children = num_points;
    node->is_leaf = 1;
    for(int i = 0; i<num_points; i++){
        node->points[i] = &points[i];
        node->MBR[i] = get_MBR_point(&points[i]);
    }
    return node;
}

Rectangle * get_MBR_node(Node * node){
    int num_children = node->num_children;
    Rectangle * rect = node->MBR[0];
    //printf("%lf\t %lf\t %lf\t %lf\n", rect->x_max,rect->x_min,rect->y_max,rect->y_min);
    if (node->num_children==0){
        return rect;
    }

    double x_max = rect->x_max;
    double x_min = rect->x_min;
    double y_max = rect->y_max;
    double y_min = rect->y_min;

    for(int i = 1; i<node->num_children; i++){
        rect = node->MBR[i];
        if(rect->x_max > x_max){
            x_max = rect->x_max;
            //printf("%lf\n", rect->x_max);
        }
        if(rect->x_min < x_min){
            x_min = rect->x_min;
        }
        if(rect->y_max > y_max){
            y_max = rect->y_max;
        }
        if(rect->y_min < y_min){
            y_min = rect->y_min;
        }
    }

    rect = malloc(sizeof(Rectangle));
    rect->x_max = x_max;
    rect->x_min = x_min;
    rect->y_max = y_max;
    rect->y_min = y_min;

    return rect;
}

Rectangle * get_MBR_point(Point * point){
    Rectangle * rect = malloc(sizeof(Rectangle));
    
    rect->x_max = point->x;
    rect->x_min = point->x;
    rect->y_max = point->y;
    rect->y_min = point->y;
    //printf("%lf\t %lf\t %lf\t %lf\n", rect->x_max,rect->x_min,rect->y_max,rect->y_min);
    return rect;
}

void STR_node(Node * nodes, int num_nodes){
    qsort(nodes, (size_t)num_nodes, sizeof(Node), compare_x_node);//change compare_x to new function compare_x_node

    int num_tiles = ceil(sqrt(ceil((double)num_nodes/ MAX_CHILDREN)));
    int num_points_per_tile = num_tiles*MAX_CHILDREN;

    int starting_index;
    int ending_index;

    for(int i = 0; i<num_tiles; i++){
        starting_index = i*num_points_per_tile;
        ending_index = starting_index + num_points_per_tile;
        
        if(starting_index>num_nodes){
            break;
        }
        if (ending_index>num_nodes){
            ending_index = num_nodes;
        }

        qsort(&nodes[starting_index], (size_t)(ending_index-starting_index), sizeof(Node), compare_y_node);//change compare_y to new function compare_y_node

    }
}


void STR_points(Point * points, int num_points){
    qsort(points, num_points, sizeof(Point), compare_x);

    int num_tiles = ceil(sqrt(ceil((double)num_points/ MAX_CHILDREN)));
    int num_points_per_tile = num_tiles*MAX_CHILDREN;

    int starting_index;
    int ending_index;

    for(int i = 0; i<num_tiles; i++){
        starting_index = i*MAX_CHILDREN;
        ending_index = starting_index + MAX_CHILDREN;
        
        if (ending_index>num_points){
            ending_index = num_points;
        }

        qsort(&points[starting_index], ending_index-starting_index, sizeof(Point), compare_y);
    }
}

int compare_x(const void* a, const void* b) {
    Point * p1 = (Point*)a;
    Point * p2 = (Point*)b;

    if (p1->x < p2->x) {
        return -1;
    } else if (p1->x > p2->x) {
        return 1;
    } else {
        return 0;
    }
}

int compare_y(const void* a, const void* b) {
    Point * p1 = (Point*)a;
    Point * p2 = (Point*)b;

    if (p1->y < p2->y) {
        return -1;
    } else if (p1->y > p2->y) {
        return 1;
    } else {
        return 0;
    }
}

int compare_x_node(const void* a, const void* b) {
    Node * n1 = (Node*)a;
    Node * n2 = (Node*)b;

    Rectangle * rect1 = get_MBR_node(n1);
    Rectangle * rect2 = get_MBR_node(n2);

    double x1 = (rect1->x_max + rect1->x_min)/2;
    double x2 = (rect2->x_max + rect2->x_min)/2;

    if (x1 < x2) {
        return -1;
    } else if (x1 > x2) {
        return 1;
    } else {
        return 0;
    }
}

int compare_y_node(const void* a, const void* b) {
    Node * n1 = (Node*)a;
    Node * n2 = (Node*)b;

    Rectangle * rect1 = get_MBR_node(n1);
    Rectangle * rect2 = get_MBR_node(n2);

    if (rect1 == NULL && rect2 == NULL){
        return 0;
    }
    else if (rect2 == NULL){
        return 1;
    }
    else if (rect1 == NULL){
        return -1;
    }

    double y1 = (rect1->y_max + rect1->y_min)/2;
    double y2 = (rect2->y_max + rect2->y_min)/2;
    
    if (y1 < y2) {
        return -1;
    } else if (y1 > y2) {
        return 1;
    } else {
        return 0;
    }
}

void print_RTree(Node * node, int depth) {
    if (node == NULL) {
        return;
    }
    printf("Depth: %d \n", depth);
    if (node->is_leaf) {
        printf("Leaf Node\n");
        for (int i = 0; i < node->num_children; i++) {
            printf("Point: (%lf, %lf)\n", node->points[i]->x, node->points[i]->y);
        }
    } else {
        printf("Internal Node\n");
        for (int i = 0; i < node->num_children; i++) {
            print_RTree(node->children[i], depth + 1);
            printf("MBR: (%lf, %lf), (%lf, %lf)\n", node->MBR[i]->x_min, node->MBR[i]->y_min, node->MBR[i]->x_max, node->MBR[i]->y_max);
        }
        print_RTree(node->children[node->num_children - 1], depth + 1);
    }
}

void post_order_traversal(Node* node) {
    if (node == NULL) {
        return;
    }
    
    // Recursively traverse the children of the node
    for (int i = 0; i < node->num_children; i++) {
        post_order_traversal(node->children[i]);
    }
    
    // Output information about the node
    printf("Node information:\n");
    printf("Is leaf: %d\n", node->is_leaf);
    printf("Number of children: %d\n", node->num_children);
    for (int i = 0; i < node->num_children; i++) {
        printf("Child %d: MBR = (%lf, %lf, %lf, %lf)\n", i+1,
            node->MBR[i]->x_min, node->MBR[i]->x_max,
            node->MBR[i]->y_min, node->MBR[i]->y_max);
    }
    
    // Output information about the points that belong to the node
    if (node->is_leaf) {
        printf("Points:\n");
        for (int i = 0; i < node->num_children; i++) {
            printf("(%lf, %lf)\n", node->points[i]->x, node->points[i]->y);
        }
    }
}

void pre_order_traversal(Node* node, int* num) {
    if (node == NULL) {
        return;
    }

    
    // Output information about the node
    int numg;
    numg = *num;
    int num_children = node->num_children;
    printf("Node information:\n");
    printf("Is leaf: %d\n", node->is_leaf);
    printf("Number of children: %d\n", num_children);

    if (node->is_leaf) {
        *num += 1;
        printf("Points:\n");
        for (int i = 0; i < num_children; i++) {
            printf("(%lf, %lf)\n", node->points[i]->x, node->points[i]->y);
        }
    } else {
        for (int i = 0; i < node->num_children; i++) {
            printf("Child %d: MBR = (%lf, %lf, %lf, %lf)\n", i+1,
                node->MBR[i]->x_min, node->MBR[i]->x_max,
                node->MBR[i]->y_min, node->MBR[i]->y_max);
        }

        printf("Child Nodes: ");

    // Recursively traverse the children of the node
        for (int i = 0; i < num_children; i++) {
            pre_order_traversal(node->children[i], num);
        }
    }  
}



int main()
{
    // read from data.txt file
    FILE *fp;
    fp = fopen("data1.txt", "r");
    //allocating memory for the array of point
    Point * points = (Point *) malloc(sizeof(Point)*100000);
    //points[0].x = 2;
    //retrieving the points from the file and storing it in the array of points
    for (int i = 0; i < 100000; i++)
    {
        fscanf(fp, "%lf %lf", &points[i].x, &points[i].y);
    }

    //for (int i = 0; i < 21; i++)
    //{
    //   printf("%lf %lf", points[i].x, points[i].y);
    //}
    
    fclose(fp); 

    int numg = 0;
    int * num = &numg;
    int num_points = sizeof(points) / sizeof(Point);
    Node * root = build_RTree_wrapper(points, 67584);     
    pre_order_traversal(root, num);
    printf("%d", *num);
    return 0;
}