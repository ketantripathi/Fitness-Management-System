#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#define SIZE 5
#define NAME_SIZE 20
#define ORDER 4  // Order of the B+ tree

// Individual structure
typedef struct individual {
    int mem_id;
    char Name[NAME_SIZE];
    int age;
    int ind_goal;
    int stepcount[7];
} ind;

// Group structure
typedef struct gr {
    int gr_id;
    char gr_name[NAME_SIZE];
    int memberIDs[SIZE];
    struct individual *arr_mem[SIZE];
    int gr_goal;
} group;

// Leaderboard structure
typedef struct leaderboard {
    int gr_id;
    char gr_name[NAME_SIZE];
    int gr_stepcount;
} leader;

// B+ Tree Node for Individuals
typedef struct BPlusTreeNode_ind {
    void **pointers;
    int *keys;
    struct BPlusTreeNode_ind *parent;
    bool is_leaf;
    int num_keys;
    struct BPlusTreeNode_ind *next; // For linking leaves
} BPlusTreeNode_ind;

// B+ Tree Node for Groups
typedef struct BPlusTreeNode_gr {
    void **pointers;
    int *keys;
    struct BPlusTreeNode_gr *parent;
    bool is_leaf;
    int num_keys;
    struct BPlusTreeNode_gr *next; // For linking leaves
} BPlusTreeNode_gr;

// B+ Tree Node for Leaderboard
typedef struct BPlusTreeNode_leader {
    void **pointers;
    int *keys;
    struct BPlusTreeNode_leader *parent;
    bool is_leaf;
    int num_keys;
    struct BPlusTreeNode_leader *next; // For linking leaves
} BPlusTreeNode_leader;

// B+ Tree structure for Individuals
typedef struct {
    BPlusTreeNode_ind *root;
} BPlusTree_ind;

// B+ Tree structure for Groups
typedef struct {
    BPlusTreeNode_gr *root;
} BPlusTree_gr;

// B+ Tree structure for Leaderboard
typedef struct {
    BPlusTreeNode_leader *root;
} BPlusTree_leader;

// Function prototypes
BPlusTreeNode_ind *insert_individual(BPlusTree_ind *tree, int key, ind *record);
ind *find_individual(BPlusTree_ind *tree, int key);
BPlusTreeNode_gr *insert_group(BPlusTree_gr *tree, int key, group *record);
group *find_group(BPlusTree_gr *tree, int key);
BPlusTreeNode_leader *insert_leader(BPlusTree_leader *tree, int key, leader *record);
void update_individual_file(BPlusTreeNode_ind *root, FILE *fptr1);
void update_group_file(BPlusTreeNode_gr *root, FILE *fptr2);
void display_indiv_info(BPlusTreeNode_ind *root);
void display_group_info(BPlusTreeNode_gr *root);
void Generate_leader_board(BPlusTree_gr *groot, BPlusTree_ind *itree);
void get_top_3(BPlusTreeNode_ind *root, int *top3_individuals, int *top3);
void print_top3(int *top3_individuals, int *top3, BPlusTree_ind *tree);
void Check_individual_rewards(int id, BPlusTree_ind *tree);
void Suggest_goal_update(BPlusTree_ind *tree);
void Check_group_achievement(int group_id, BPlusTree_gr *tree);
void Merge_groups(int id1, int id2, BPlusTree_gr *tree);
void Delete_Individual(int mem_id, BPlusTree_ind *itree, BPlusTree_gr *gtree);
void Delete_group(int gr_id, BPlusTree_gr *tree);

// B+ Tree implementation functions
BPlusTreeNode_ind *create_node_ind(bool is_leaf) {
    BPlusTreeNode_ind *new_node = (BPlusTreeNode_ind *)malloc(sizeof(BPlusTreeNode_ind));
    new_node->keys = (int *)malloc((ORDER - 1) * sizeof(int));
    new_node->pointers = (void **)malloc(ORDER * sizeof(void *));
    new_node->is_leaf = is_leaf;
    new_node->num_keys = 0;
    new_node->parent = NULL;
    new_node->next = NULL;
    return new_node;
}

BPlusTreeNode_gr *create_node_gr(bool is_leaf) {
    BPlusTreeNode_gr *new_node = (BPlusTreeNode_gr *)malloc(sizeof(BPlusTreeNode_gr));
    new_node->keys = (int *)malloc((ORDER - 1) * sizeof(int));
    new_node->pointers = (void **)malloc(ORDER * sizeof(void *));
    new_node->is_leaf = is_leaf;
    new_node->num_keys = 0;
    new_node->parent = NULL;
    new_node->next = NULL;
    return new_node;
}

BPlusTreeNode_leader *create_node_leader(bool is_leaf) {
    BPlusTreeNode_leader *new_node = (BPlusTreeNode_leader *)malloc(sizeof(BPlusTreeNode_leader));
    new_node->keys = (int *)malloc((ORDER - 1) * sizeof(int));
    new_node->pointers = (void **)malloc(ORDER * sizeof(void *));
    new_node->is_leaf = is_leaf;
    new_node->num_keys = 0;
    new_node->parent = NULL;
    new_node->next = NULL;
    return new_node;
}

// Find the leaf node where a key should be inserted (for individuals)
BPlusTreeNode_ind *find_leaf_ind(BPlusTreeNode_ind *root, int key) {
    BPlusTreeNode_ind *c = root;
    if (c == NULL) {
        return c;
    }
    while (!c->is_leaf) {
        int i = 0;
        while (i < c->num_keys) {
            if (key >= c->keys[i]) i++;
            else break;
        }
        c = (BPlusTreeNode_ind *)c->pointers[i];
    }
    return c;
}

// Find the leaf node where a key should be inserted (for groups)
BPlusTreeNode_gr *find_leaf_gr(BPlusTreeNode_gr *root, int key) {
    BPlusTreeNode_gr *c = root;
    if (c == NULL) {
        return c;
    }
    while (!c->is_leaf) {
        int i = 0;
        while (i < c->num_keys) {
            if (key >= c->keys[i]) i++;
            else break;
        }
        c = (BPlusTreeNode_gr *)c->pointers[i];
    }
    return c;
}

// Insert into B+ tree for individuals
BPlusTreeNode_ind *insert_individual(BPlusTree_ind *tree, int key, ind *record) {
    if (tree->root == NULL) {
        tree->root = create_node_ind(true);
        tree->root->keys[0] = key;
        tree->root->pointers[0] = record;
        tree->root->pointers[ORDER - 1] = NULL;
        tree->root->num_keys++;
        return tree->root;
    }

    BPlusTreeNode_ind *leaf = find_leaf_ind(tree->root, key);

    // Check if key already exists
    for (int i = 0; i < leaf->num_keys; i++) {
        if (leaf->keys[i] == key) {
            // Key exists, update the record
            leaf->pointers[i] = record;
            return tree->root;
        }
    }

    // If leaf has room, insert into leaf
    if (leaf->num_keys < ORDER - 1) {
        int i = 0;
        while (i < leaf->num_keys && leaf->keys[i] < key) {
            i++;
        }

        for (int j = leaf->num_keys; j > i; j--) {
            leaf->keys[j] = leaf->keys[j - 1];
            leaf->pointers[j] = leaf->pointers[j - 1];
        }

        leaf->keys[i] = key;
        leaf->pointers[i] = record;
        leaf->num_keys++;
        return tree->root;
    }

    // Leaf is full, need to split
    // Implementation of split and insert would go here
    // This is a simplified version - a full implementation would handle splitting nodes
    // and propagating changes up the tree
    
    printf("Need to implement node splitting for B+ tree\n");
    return tree->root;
}

// Find an individual in the B+ tree
ind *find_individual(BPlusTree_ind *tree, int key) {
    BPlusTreeNode_ind *leaf = find_leaf_ind(tree->root, key);
    if (leaf == NULL) return NULL;
    
    for (int i = 0; i < leaf->num_keys; i++) {
        if (leaf->keys[i] == key) {
            return (ind *)leaf->pointers[i];
        }
    }
    return NULL;
}

// Insert into B+ tree for groups
BPlusTreeNode_gr *insert_group(BPlusTree_gr *tree, int key, group *record) {
    if (tree->root == NULL) {
        tree->root = create_node_gr(true);
        tree->root->keys[0] = key;
        tree->root->pointers[0] = record;
        tree->root->pointers[ORDER - 1] = NULL;
        tree->root->num_keys++;
        return tree->root;
    }

    BPlusTreeNode_gr *leaf = find_leaf_gr(tree->root, key);

    // Check if key already exists
    for (int i = 0; i < leaf->num_keys; i++) {
        if (leaf->keys[i] == key) {
            // Key exists, update the record
            leaf->pointers[i] = record;
            return tree->root;
        }
    }

    // If leaf has room, insert into leaf
    if (leaf->num_keys < ORDER - 1) {
        int i = 0;
        while (i < leaf->num_keys && leaf->keys[i] < key) {
            i++;
        }

        for (int j = leaf->num_keys; j > i; j--) {
            leaf->keys[j] = leaf->keys[j - 1];
            leaf->pointers[j] = leaf->pointers[j - 1];
        }

        leaf->keys[i] = key;
        leaf->pointers[i] = record;
        leaf->num_keys++;
        return tree->root;
    }

    // Leaf is full, need to split
    printf("Need to implement node splitting for B+ tree\n");
    return tree->root;
}

// Find a group in the B+ tree
group *find_group(BPlusTree_gr *tree, int key) {
    BPlusTreeNode_gr *leaf = find_leaf_gr(tree->root, key);
    if (leaf == NULL) return NULL;
    
    for (int i = 0; i < leaf->num_keys; i++) {
        if (leaf->keys[i] == key) {
            return (group *)leaf->pointers[i];
        }
    }
    return NULL;
}

// Insert into leaderboard B+ tree
BPlusTreeNode_leader *insert_leader(BPlusTree_leader *tree, int key, leader *record) {
    if (tree->root == NULL) {
        tree->root = create_node_leader(true);
        tree->root->keys[0] = key;
        tree->root->pointers[0] = record;
        tree->root->pointers[ORDER - 1] = NULL;
        tree->root->num_keys++;
        return tree->root;
    }

    // Similar implementation as above for groups and individuals
    // Would need to find the appropriate leaf node and insert
    // This is a simplified version
    
    printf("Leaderboard insertion not fully implemented\n");
    return tree->root;
}

// Update individual file (traverse B+ tree leaves)
void update_individual_file(BPlusTreeNode_ind *root, FILE *fptr1) {
    if (root == NULL) return;
    
    if (root->is_leaf) {
        for (int i = 0; i < root->num_keys; i++) {
            ind *person = (ind *)root->pointers[i];
            fprintf(fptr1, "%d\t%10s\t%2d\t%10d\t", person->mem_id, person->Name, person->age, person->ind_goal);
            for (int j = 0; j < 7; j++) {
                fprintf(fptr1, "%5d ", person->stepcount[j]);
            }
            fprintf(fptr1, "\n");
        }
        
        // Move to next leaf node
        if (root->next != NULL) {
            update_individual_file(root->next, fptr1);
        }
    } else {
        // For non-leaf nodes, recursively visit all children
        for (int i = 0; i <= root->num_keys; i++) {
            update_individual_file((BPlusTreeNode_ind *)root->pointers[i], fptr1);
        }
    }
}

// Update group file (traverse B+ tree leaves)
void update_group_file(BPlusTreeNode_gr *root, FILE *fptr2) {
    if (root == NULL) return;
    
    if (root->is_leaf) {
        for (int i = 0; i < root->num_keys; i++) {
            group *grp = (group *)root->pointers[i];
            fprintf(fptr2, "%d\t%10s\t%10d\t", grp->gr_id, grp->gr_name, grp->gr_goal);
            for (int j = 0; j < 5; j++) {
                fprintf(fptr2, "%5d ", grp->memberIDs[j]);
            }
            fprintf(fptr2, "\n");
        }
        
        // Move to next leaf node
        if (root->next != NULL) {
            update_group_file(root->next, fptr2);
        }
    } else {
        // For non-leaf nodes, recursively visit all children
        for (int i = 0; i <= root->num_keys; i++) {
            update_group_file((BPlusTreeNode_gr *)root->pointers[i], fptr2);
        }
    }
}

// Display individual information (in-order traversal)
void display_indiv_info(BPlusTreeNode_ind *root) {
    if (root == NULL) return;
    
    if (root->is_leaf) {
        for (int i = 0; i < root->num_keys; i++) {
            ind *person = (ind *)root->pointers[i];
            printf("\n %d \t %10s  %d \t %d\t ", person->mem_id, person->Name, person->age, person->ind_goal);
            for (int j = 0; j < 7; j++) {
                printf("%d ", person->stepcount[j]);
            }
            printf("\n");
        }
        
        // Move to next leaf node
        if (root->next != NULL) {
            display_indiv_info(root->next);
        }
    } else {
        // For non-leaf nodes, recursively visit all children
        for (int i = 0; i <= root->num_keys; i++) {
            display_indiv_info((BPlusTreeNode_ind *)root->pointers[i]);
        }
    }
}

// Display group information (in-order traversal)
void display_group_info(BPlusTreeNode_gr *root) {
    if (root == NULL) return;
    
    if (root->is_leaf) {
        for (int i = 0; i < root->num_keys; i++) {
            group *grp = (group *)root->pointers[i];
            printf("\n %d \t %10s  %d\t ", grp->gr_id, grp->gr_name, grp->gr_goal);
            for (int j = 0; j < 5; j++) {
                printf("%d ", grp->memberIDs[j]);
            }
            printf("\n");
        }
        
        // Move to next leaf node
        if (root->next != NULL) {
            display_group_info(root->next);
        }
    } else {
        // For non-leaf nodes, recursively visit all children
        for (int i = 0; i <= root->num_keys; i++) {
            display_group_info((BPlusTreeNode_gr *)root->pointers[i]);
        }
    }
}

// Check if an individual is in any group
bool isIndividualInAnyGroup(BPlusTree_gr *grouptree, ind *individual) {
    // This would need to traverse all groups and check their members
    // Simplified implementation - would need proper B+ tree traversal
    printf("isIndividualInAnyGroup not fully implemented\n");
    return false;
}

// Add individual to group
void addIndividualToGroup(group *group1, ind *individual1, BPlusTree_gr *gtree) {
    if (group1 == NULL || individual1 == NULL) {
        printf("Group or individual does not exist.\n");
        return;
    }
    
    if (isIndividualInAnyGroup(gtree, individual1)) {
        printf("Individual is already in another group.\n");
        return;
    }

    for (int i = 0; i < SIZE; ++i) {
        if (group1->arr_mem[i] == NULL) {
            group1->memberIDs[i] = individual1->mem_id;
            group1->arr_mem[i] = individual1;
            return;
        }
    }
}

// Generate leaderboard
void Generate_leader_board(BPlusTree_gr *groot, BPlusTree_ind *itree) {
    // This would need to traverse all groups, calculate their step counts,
    // and then insert into a leaderboard B+ tree sorted by step count
    printf("Generate_leader_board not fully implemented\n");
}

// Get top 3 individuals
void get_top_3(BPlusTreeNode_ind *root, int *top3_individuals, int *top3) {
    if (root == NULL) return;
    
    if (root->is_leaf) {
        for (int i = 0; i < root->num_keys; i++) {
            ind *person = (ind *)root->pointers[i];
            int total_steps = 0;
            int flag = 1;
            
            for (int j = 0; j < 7; j++) {
                if (person->stepcount[j] > person->ind_goal) {
                    total_steps += person->stepcount[j];
                } else {
                    flag = 0;
                    break;
                }
            }
            
            if (flag) {
                if (total_steps > top3[0]) {
                    top3_individuals[2] = top3_individuals[1];
                    top3_individuals[1] = top3_individuals[0];
                    top3_individuals[0] = person->mem_id;
                    top3[2] = top3[1];
                    top3[1] = top3[0];
                    top3[0] = total_steps;
                } else if (total_steps > top3[1]) {
                    top3_individuals[2] = top3_individuals[1];
                    top3_individuals[1] = person->mem_id;
                    top3[2] = top3[1];
                    top3[1] = total_steps;
                } else if (total_steps > top3[2]) {
                    top3_individuals[2] = person->mem_id;
                    top3[2] = total_steps;
                }
            }
        }
        
        if (root->next != NULL) {
            get_top_3(root->next, top3_individuals, top3);
        }
    } else {
        for (int i = 0; i <= root->num_keys; i++) {
            get_top_3((BPlusTreeNode_ind *)root->pointers[i], top3_individuals, top3);
        }
    }
}

// Print top 3 individuals
void print_top3(int *top3_individuals, int *top3, BPlusTree_ind *tree) {
    printf("Top 3 individuals:\n");
    for (int i = 0; i < 3; i++) {
        if (top3_individuals[i] != 0) {
            ind *curr = find_individual(tree, top3_individuals[i]);
            if (curr == NULL) {
                printf("ERROR");
            } else {
                printf("%d. ID: %d, Name: %s, Total Steps: %d\n", i + 1,
                       top3_individuals[i], curr->Name, top3[i]);
            }
        }
    }
}

// Check individual rewards
void Check_individual_rewards(int id, BPlusTree_ind *tree) {
    int *arr = (int *)malloc(sizeof(int) * 3);
    int *ptr = (int *)malloc(sizeof(int) * 3);
    memset(ptr, 0, 3 * sizeof(int));
    memset(arr, 0, 3 * sizeof(int));
    
    get_top_3(tree->root, arr, ptr);
    
    if (arr[0] == id) {
        printf("The person with person id:%d is on top 1 of the leaderboard\nReward:100 Points\n", id);
    } else if (arr[1] == id) {
        printf("The person with person id:%d is on top 2 of the leaderboard\nReward:75 Points\n", id);
    } else if (arr[2] == id) {
        printf("The person with person id:%d is on top 3 of the leaderboard\nReward:50 Points\n", id);
    } else {
        printf("The person with person id:%d is not in the top 3 of the leaderboard\n", id);
    }
    
    free(arr);
    free(ptr);
}

// Suggest goal updates
void Suggest_goal_update(BPlusTree_ind *tree) {
    int *arr = (int *)malloc(sizeof(int) * 3);
    int *ptr = (int *)malloc(sizeof(int) * 3);
    memset(ptr, 0, 3 * sizeof(int));
    memset(arr, 0, 3 * sizeof(int));
    
    get_top_3(tree->root, arr, ptr);
    
    for (int i = 0; i < 3; i++) {
        int goal = ptr[i] / 7;
        printf("\nyou need to have minimum daily goal of %d for rank %d \n", goal + 1, i + 1);
    }
    
    free(arr);
    free(ptr);
}

// Check group achievement
void Check_group_achievement(int group_id, BPlusTree_gr *tree) {
    group *curr = find_group(tree, group_id);
    
    if (curr != NULL) {
        int sum = 0;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < 7 && (curr->arr_mem[i] != NULL); j++) {
                sum = sum + curr->arr_mem[i]->stepcount[j];
            }
        }
        
        if (sum >= curr->gr_goal) {
            printf("Group id:%d \n The group with above group id have achieved the "
                   "group goal for this week\n Cumulative step count of group:%d \n "
                   "Group goal:%d",
                   curr->gr_id, sum, curr->gr_goal);
        } else {
            printf("Group id:%d \n The group with above group id have not achieved "
                   "the group goal for this week\n Cumulative step count of group:%d "
                   "\n Group goal:%d \n The group is short of %d no. of steps in the "
                   "current week",
                   curr->gr_id, sum, curr->gr_goal, curr->gr_goal - sum);
        }
    } else {
        printf("The group with group id:%d not found in the list", group_id);
    }
}

// Delete individual
void Delete_Individual(int mem_id, BPlusTree_ind *itree, BPlusTree_gr *gtree) {
    // Implementation of B+ tree deletion would go here
    printf("Delete_Individual not fully implemented\n");
}

// Delete group
void Delete_group(int gr_id, BPlusTree_gr *tree) {
    // Implementation of B+ tree deletion would go here
    printf("Delete_group not fully implemented\n");
}

// Merge groups
void Merge_groups(int id1, int id2, BPlusTree_gr *tree) {
    group *ptr1 = find_group(tree, id1);
    group *ptr2 = find_group(tree, id2);
    
    if (ptr1 == NULL || ptr2 == NULL) {
        printf("One or both of the groups not found.\n");
    } else {
        int c = 0;
        for (int i = 0; i < 5; i++) {
            if (ptr1->memberIDs[i] != 0 || ptr2->memberIDs[i] != 0) {
                c++;
            }         
        }
        
        if (c > 5) {
            printf("merge unsuccessful, total members exceed group size...!");
        } else {
            ptr1->gr_goal = ptr1->gr_goal + ptr2->gr_goal;
            int x = 0;
            for (int j = 0; j < 5; j++) {
                if (ptr1->memberIDs[j] == 0) {
                    while (ptr2->memberIDs[x] == 0) {
                        x++;
                    }
                    ptr1->memberIDs[j] = ptr2->memberIDs[x];
                    ptr1->arr_mem[j] = ptr2->arr_mem[x];
                    x++;
                }
            }
            
            Delete_group(ptr2->gr_id, tree);
            printf("%d is merged into %d successfully", id2, id1);
        }  
    }
}

int main() {
    int NO_of_individuals = 20;
    int NO_of_groups = 5;
    FILE *fptr = fopen("input.txt", "r");
    FILE *gptr = fopen("group.txt", "r");
    
    BPlusTree_ind itree = {NULL};
    BPlusTree_gr gtree = {NULL};
    
    int mem_id, age, ind_goal;
    char Name[NAME_SIZE];
    int stepcount[7];
    
    // Load individuals
    for (int i = 0; i < NO_of_individuals; i++) {
        fscanf(fptr, "%d", &mem_id);
        fflush(stdin);
        fscanf(fptr, "%s", Name);
        fflush(stdin);
        fscanf(fptr, "%d", &age);
        fflush(stdin);
        fscanf(fptr, "%d", &ind_goal);
        fflush(stdin);

        for (int j = 0; j < 7; j++) {
            fscanf(fptr, "%d", &stepcount[j]);
        }
        
        ind *new_ind = (ind *)malloc(sizeof(ind));
        new_ind->mem_id = mem_id;
        strcpy(new_ind->Name, Name);
        new_ind->age = age;
        new_ind->ind_goal = ind_goal;
        memcpy(new_ind->stepcount, stepcount, 7 * sizeof(int));
        
        insert_individual(&itree, mem_id, new_ind);
        printf("%d is added\n", mem_id);
    }

    int group_id;
    int group_goal;
    int NO_of_members;
    char group_name[NAME_SIZE];
    int members_Id[5];
    
    // Load groups
    for (int i = 0; i < NO_of_groups; i++) {
        fscanf(gptr, "%d", &group_id);
        fflush(stdin);
        fscanf(gptr, "%s", group_name);
        fflush(stdin);
        fscanf(gptr, "%d", &group_goal);
        fflush(stdin);
        
        group *new_group = (group *)malloc(sizeof(group));
        new_group->gr_id = group_id;
        strcpy(new_group->gr_name, group_name);
        new_group->gr_goal = group_goal;
        for (int j = 0; j < SIZE; j++) {
            new_group->memberIDs[j] = 0;
            new_group->arr_mem[j] = NULL;
        }
        
        insert_group(&gtree, group_id, new_group);
        
        fscanf(gptr, "%d", &NO_of_members);
        fflush(stdin);
        for (int j = 0; j < NO_of_members; j++) {
            fscanf(gptr, "%d", &members_Id[j]);
            ind *curr = find_individual(&itree, members_Id[j]);
            
            if (curr == NULL) {
                printf("ERROR:-The individual with member id-%d not found in the list\n", members_Id[j]);
            } else {
                addIndividualToGroup(new_group, curr, &gtree);
            }
        }
        printf("%d is created and added successfully \n", group_id);
    }
    
    fclose(fptr);
    fclose(gptr);
    
    int choice = 0;
    while (choice != 13) {
        printf("\n\n\n\n\nCHOOSE ANY OPTION:-\n");
        printf("1- Add a person\n2- Create a group\n3- Get top 3 winners\n4- Check group achievements\n");
        printf("5- Generate Leaderboard of groups\n6- check individual rewards\n7- delete individual\n");
        printf("8- Delete a group\n9- suggest goal updates for any person\n10- merge two groups\n");
        printf("11- display group info\n12-display all individual info\n13-EXIT\n");
        printf("Enter:");
        scanf("%d", &choice);
        
        if (choice == 1) {
            printf("Enter ID : \n");
            scanf("%d", &mem_id);
            printf("Enter Name : \n");
            scanf("%s", Name);
            printf("Enter age : \n");
            scanf("%d", &age);
            printf("Enter individual goal : \n");
            scanf("%d", &ind_goal);
            printf("Enter Stepcount of 7 Days : \n");
            for (int j = 0; j < 7; j++) {
                scanf("%d", &stepcount[j]);
            }
            
            ind *new_ind = (ind *)malloc(sizeof(ind));
            new_ind->mem_id = mem_id;
            strcpy(new_ind->Name, Name);
            new_ind->age = age;
            new_ind->ind_goal = ind_goal;
            memcpy(new_ind->stepcount, stepcount, 7 * sizeof(int));
            
            insert_individual(&itree, mem_id, new_ind);
            
            FILE *fptr1 = fopen("input.txt", "a");
            fprintf(fptr1, "\n");
            fprintf(fptr1, "%d %s %d %d", mem_id, Name, age, ind_goal);
            for (int i = 0; i < 7; i++) {
                fprintf(fptr1, " %d", stepcount[i]);
            }
            fclose(fptr1);
        }
        else if (choice == 2) {
            printf("Enter group ID : \n");
            scanf("%d", &group_id);
            printf("Enter group Name : \n");
            scanf("%s", group_name);
            printf("Enter weekly group goal : \n");
            scanf("%d", &group_goal);
            printf("Enter no of members : \n");
            scanf("%d", &NO_of_members);
            printf("Enter id of members : \n");
            for (int j = 0; j < NO_of_members; j++) {
                scanf("%d", &members_Id[j]);
            }
            
            group *new_group = (group *)malloc(sizeof(group));
            new_group->gr_id = group_id;
            strcpy(new_group->gr_name, group_name);
            new_group->gr_goal = group_goal;
            for (int j = 0; j < SIZE; j++) {
                new_group->memberIDs[j] = 0;
                new_group->arr_mem[j] = NULL;
            }
            
            insert_group(&gtree, group_id, new_group);
            
            for (int j = 0; j < NO_of_members; j++) {
                ind *curr = find_individual(&itree, members_Id[j]);
                addIndividualToGroup(new_group, curr, &gtree);
            }
            
            FILE *fptr2 = fopen("group.txt", "a");
            fprintf(fptr2, "\n");
            fprintf(fptr2, "%d %s %d %d", group_id, group_name, group_goal, NO_of_members);
            for (int i = 0; i < NO_of_members; i++) {
                fprintf(fptr2, " %d", members_Id[i]);
            }
            fclose(fptr2);
        }
        else if (choice == 3) {
            int *arr = (int *)malloc(sizeof(int) * 3);
            int *ptr = (int *)malloc(sizeof(int) * 3);
            memset(ptr, 0, 3 * sizeof(int));
            memset(arr, 0, 3 * sizeof(int));
            
            get_top_3(itree.root, arr, ptr);
            print_top3(arr, ptr, &itree);
            
            free(arr);
            free(ptr);
        }
        else if (choice == 4) {
            int id;
            printf("enter group id no.:");
            scanf("%d", &id);
            Check_group_achievement(id, &gtree);
        }
        else if (choice == 5) {
            Generate_leader_board(&gtree, &itree);
        }
        else if (choice == 6) {
            int id;
            printf("enter id no.:");
            scanf("%d", &id);
            Check_individual_rewards(id, &itree);
        }
        else if (choice == 7) {
            int id;
            printf("enter id no.:");
            scanf("%d", &id);
            Delete_Individual(id, &itree, &gtree);
            
            FILE *fptr3 = fopen("group.txt", "w");
            update_group_file(gtree.root, fptr3);
            fclose(fptr3);
            
            FILE *fptr4 = fopen("input.txt", "w");
            update_individual_file(itree.root, fptr4);
            fclose(fptr4);
        }
        else if (choice == 8) {
            int id;
            printf("enter group id no.:");
            scanf("%d", &id);
            
            Delete_group(id, &gtree);
            
            FILE *fptr5 = fopen("group.txt", "w");
            update_group_file(gtree.root, fptr5);
            fclose(fptr5);
        }
        else if (choice == 9) {
            Suggest_goal_update(&itree);
        }
        else if (choice == 10) {
            int id1, id2;
            printf("enter group id Nos.:");
            scanf("%d", &id1);
            scanf("%d", &id2);
            Merge_groups(id1, id2, &gtree);
        }
        else if (choice == 11) {
            display_group_info(gtree.root);
        }
        else if (choice == 12) {
            display_indiv_info(itree.root);
        }
        else if (choice == 13) {
            printf("EXITING...");
        }
        else {
            printf("wrong input, try again...!");
        }
    }
    
    return 0;
}
