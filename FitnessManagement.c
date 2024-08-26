#include<stdio.h> 
#include<stdlib.h>
#include<string.h> 
#define SIZE 5
#define NAME_SIZE 20
// An AVL tree node 
typedef struct individual 
{ 
	int mem_id;
    char Name[NAME_SIZE];
    int age;
    int ind_goal;
    int stepcount[7];
	struct individual *left; 
	struct individual *right; 
	int height; 
} ind; 

typedef struct gr
{
    int gr_id;
    char gr_name[NAME_SIZE];
    int memberIDs[SIZE];
    ind *arr_mem[SIZE];
    int gr_goal;
    struct gr *left;
    struct gr *right;
    int height;
} group;

typedef struct leaderboard
{
    int gr_id;
    char gr_name[NAME_SIZE];
    int gr_stepcount;
	int height; 
    struct leaderboard *left;
    struct leaderboard *right;
} leader;

void update_individual_file(ind *root,FILE *fptr1)
{
    if (root != NULL)
    {
        update_individual_file(root->left,fptr1);
        fprintf(fptr1,"%d\t%10s\t%2d\t%10d\t",root->mem_id,root->Name,root->age,root->ind_goal);
        for (int i = 0; i < 7; i++)
        {
            fprintf(fptr1,"%5d ",root->stepcount[i]);
        }
        fprintf(fptr1,"\n");
        update_individual_file(root->right,fptr1);
    }
}

void update_group_file(group *root,FILE *fptr2)
{
    if (root != NULL)
    {
        update_group_file(root->left,fptr2);
        fprintf(fptr2,"%d\t%10s\t%10d\t",root->gr_id,root->gr_name,root->gr_goal);
        for (int i = 0; i < 5; i++)
        {
            fprintf(fptr2,"%5d ",root->memberIDs[i]);
        }
        fprintf(fptr2,"\n");
        update_group_file(root->right,fptr2);
    }
}

// A utility function to get the height of the tree 
int height(struct individual *N) 
{ 
	if (N == NULL) 
		return 0; 
	return N->height; 
} 

// A utility function to get maximum of two integers 
int max(int a, int b) 
{ 
	return (a > b)? a : b; 
} 

/* Helper function that allocates a new node with the given mem_id and 
	NULL left and right pointers. */
struct individual* newNode(int id,char *Name, int age, int goal, int *stepcount) 
{ 
	struct individual* node = (struct individual*) 
						malloc(sizeof(struct individual)); 
    node->mem_id = id;
    strcpy(node->Name, Name);
    node->age = age;
    node->ind_goal = goal;
    memcpy(node->stepcount, stepcount, 7 * sizeof(int));
	node->left = NULL; 
	node->right = NULL; 
	node->height = 1; // new node is initially added at leaf 
	return(node); 
} 

// A utility function to right rotate subtree rooted with y  
struct individual *rightRotate(struct individual *y) 
{ 
	struct individual *x = y->left; 
	struct individual *T2 = x->right; 

	// Perform rotation 
	x->right = y; 
	y->left = T2; 

	// Update heights 
	y->height = max(height(y->left), 
					height(y->right)) + 1; 
	x->height = max(height(x->left), 
					height(x->right)) + 1; 

	// Return new root 
	return x; 
} 

// A utility function to left rotate subtree rooted with x  
struct individual *leftRotate(struct individual *x) 
{ 
	struct individual *y = x->right; 
	struct individual *T2 = y->left; 

	// Perform rotation 
	y->left = x; 
	x->right = T2; 

	// Update heights 
	x->height = max(height(x->left), 
					height(x->right)) + 1; 
	y->height = max(height(y->left), 
					height(y->right)) + 1; 

	// Return new root 
	return y; 
} 

// Get Balance factor of node N 
int getBalance(struct individual *N) 
{ 
	if (N == NULL) 
		return 0; 
	return height(N->left) - height(N->right); 
} 

// Recursive function to insert a mem_id in the subtree rooted 
// with node and returns the new root of the subtree. 
struct individual* Add_Person(struct individual* node, int mem_id,char *Name, int age, int ind_goal, int *stepcount) 
{ 
	/* 1. Perform the normal BST insertion */
	if (node == NULL) 
		return(newNode(mem_id,Name, age, ind_goal, stepcount)); 

	if (mem_id < node->mem_id) 
		node->left = Add_Person(node->left, mem_id,Name, age, ind_goal, stepcount); 
	else if (mem_id > node->mem_id) 
		node->right = Add_Person(node->right, mem_id,Name, age, ind_goal, stepcount); 
	else // Equal mem_ids are not allowed in BST 
		return node; 

	/* 2. Update height of this ancestor node */
	node->height = 1 + max(height(node->left), 
						height(node->right)); 

	/* 3. Get the balance factor of this ancestor 
		node to check whether this node became 
		unbalanced */
	int balance = getBalance(node); 

	// If this node becomes unbalanced, then 
	// there are 4 cases 

	// Left Left Case 
	if (balance > 1 && mem_id < node->left->mem_id) 
		return rightRotate(node); 

	// Right Right Case 
	if (balance < -1 && mem_id > node->right->mem_id) 
		return leftRotate(node); 

	// Left Right Case 
	if (balance > 1 && mem_id > node->left->mem_id) 
	{ 
		node->left = leftRotate(node->left); 
		return rightRotate(node); 
	} 

	// Right Left Case 
	if (balance < -1 && mem_id < node->right->mem_id) 
	{ 
		node->right = rightRotate(node->right); 
		return leftRotate(node); 
	} 

	/* return the (unchanged) node pointer */
	return node; 
} 

int gheight(group *N) 
{ 
	if (N == NULL) 
		return 0; 
	return N->height; 
} 

// A utility function to get maximum of two integers 
int gmax(int a, int b) 
{ 
	return (a > b)? a : b; 
} 

/* Helper function that allocates a new node with the given mem_id and 
	NULL left and right pointers. */
group* newgroupNode(int id,char *Name, int goal) 
{ 
	group* node = (group*)malloc(sizeof(group)); 
    node->gr_id = id;
    strcpy(node->gr_name, Name);
    node->gr_goal = goal;
    for (int i = 0; i < 5; i++)
    {
        node->arr_mem[i]= NULL;
        node->memberIDs[i]=0;      
    }
	node->left = NULL; 
	node->right = NULL; 
	node->height = 1; // new node is initially added at leaf 
	return(node); 
} 

// A utility function to right rotate subtree rooted with y  
group *grouprightRotate(group *y) 
{ 
	group *x = y->left; 
	group *T2 = x->right; 

	// Perform rotation 
	x->right = y; 
	y->left = T2; 

	// Update heights 
	y->height = gmax(gheight(y->left), 
					gheight(y->right)) + 1; 
	x->height = gmax(gheight(x->left), 
					gheight(x->right)) + 1; 

	// Return new root 
	return x; 
} 

// A utility function to left rotate subtree rooted with x 
group *groupleftRotate(group *x) 
{ 
	group *y = x->right; 
	group *T2 = y->left; 

	// Perform rotation 
	y->left = x; 
	x->right = T2; 

	// Update heights 
	x->height = gmax(gheight(x->left), 
					gheight(x->right)) + 1; 
	y->height = gmax(gheight(y->left), 
					gheight(y->right)) + 1; 

	// Return new root 
	return y; 
} 

// Get Balance factor of node N 
int getgroupBalance(group *N) 
{ 
	if (N == NULL) 
		return 0; 
	return gheight(N->left) - gheight(N->right); 
} 

// Recursive function to insert a mem_id in the subtree rooted 
// with node and returns the new root of the subtree. 
group* Add_group(group* node, int gr_id,char *Name, int goal) 
{ 
	/* 1. Perform the normal BST insertion */
	if (node == NULL) 
		return(newgroupNode(gr_id,Name,goal)); 

	if (gr_id < node->gr_id) 
		node->left = Add_group(node->left, gr_id,Name,goal); 
	else if (gr_id > node->gr_id) 
		node->right = Add_group(node->right, gr_id,Name,goal); 
	else // Equal mem_ids are not allowed in BST 
		return node; 

	/* 2. Update height of this ancestor node */
	node->height = 1 + gmax(gheight(node->left), 
						gheight(node->right)); 

	/* 3. Get the balance factor of this ancestor 
		node to check whether this node became 
		unbalanced */
	int balance = getgroupBalance(node); 

	// If this node becomes unbalanced, then 
	// there are 4 cases 

	// Left Left Case 
	if (balance > 1 && gr_id < node->left->gr_id) 
		return grouprightRotate(node); 

	// Right Right Case 
	if (balance < -1 && gr_id > node->right->gr_id) 
		return groupleftRotate(node); 

	// Left Right Case 
	if (balance > 1 && gr_id > node->left->gr_id) 
	{ 
		node->left = groupleftRotate(node->left); 
		return grouprightRotate(node); 
	} 

	// Right Left Case 
	if (balance < -1 && gr_id < node->right->gr_id) 
	{ 
		node->right = grouprightRotate(node->right); 
		return groupleftRotate(node); 
	} 

	/* return the (unchanged) node pointer */
	return node; 
} 

int isIndividualInAnyGroup(group *grouptree, ind *individual)
{
    group *currentGroup = grouptree;
    int found = 0;
    if (currentGroup != NULL)
    {
        for (int i = 0; i < 5; ++i)
        {
            if (currentGroup->arr_mem[i] == individual && !found)
            {
                found = 1;
            }
        }
        if (found==0)
        {
            found = isIndividualInAnyGroup(currentGroup->left,individual);
            if (found==0)
            {
                found = isIndividualInAnyGroup(currentGroup->right,individual);
            }
        }
    }
    return found;
}

void addIndividualToGroup(group *group1, ind *individual1, group *groot)
{
    if (group1 == NULL || individual1 == NULL)
    {
        printf("Group or individual does not exist.\n");
        return;
    }
    if (isIndividualInAnyGroup(groot, individual1))
    {
        printf("Individual is already in another group.\n");
        return;
    }

    for (int i = 0; i < SIZE; ++i)
    {
        if (group1->arr_mem[i] == NULL)
        {
            group1->memberIDs[i] = individual1->mem_id;
            group1->arr_mem[i] = individual1;
            return;
        }
    }
}

void display_indiv_info(ind *root)
{
    ind *curr = root;
    if(curr != NULL)
    {
        display_indiv_info(root->left);
        printf("\n %d \t %10s  %d \t %d\t ", curr->mem_id, curr->Name, curr->age, curr->ind_goal);
        for (int i = 0; i < 7; i++)
        {
            printf("%d ", curr->stepcount[i]);
        }
        printf("\n");
        display_indiv_info(root->right);
    }
    if (curr == NULL)
    {
        return;
    }
}

void display_group_info(group *root)
{
    group *curr = root;
    if(curr != NULL)
    {
        display_group_info(root->left);
        printf("\n %d \t %10s  %d\t ", curr->gr_id, curr->gr_name, curr->gr_goal);
        for (int i = 0; i < 5; i++)
        {
            printf("%d ", curr->memberIDs[i]);
        }
        printf("\n");
        display_group_info(root->right);
    }
    if (curr == NULL)
    {
        return;
    }
}

int getSize(group* root) {
    if (root == NULL)
        return 0;
    else
        return getSize(root->left) + 1 + getSize(root->right);
}

void treeToarray(group *root, leader *arr, int *index) {
    if (root != NULL) {
        // Traverse the left subtree
        treeToarray(root->left, arr, index);
        // Copy data into the array
        strcpy(arr[*index].gr_name, root->gr_name);
        arr[*index].gr_stepcount = 0;
        arr[*index].gr_id = root->gr_id;
        arr[*index].height = 1;
        // Calculate step count
        for (int i = 0; i < SIZE; i++) {
            if (root->memberIDs[i] != 0) {
                for (int j = 0; j < 7; j++) {
                    // printf("%d %d\n",root->gr_id,j);
                    arr[*index].gr_stepcount += root->arr_mem[i]->stepcount[j];
                }
            }
        }

        // Move to the next index in the array
        (*index)++;

        // Traverse the right subtree
        treeToarray(root->right, arr, index);
    }
}

// Function to create a new tree node
int getheight(leader *N) 
{ 
	if (N == NULL) 
		return 0; 
	return N->height; 
} 

// A utility function to get maximum of two integers 
int getmax(int a, int b) 
{ 
	return (a > b)? a : b; 
} 

/* Helper function that allocates a new node with the given mem_id and 
	NULL left and right pointers. */
leader* newleaderNode(int id,char *Name,int step) 
{ 
	leader* node = (leader*) 
						malloc(sizeof(leader)); 
    node->gr_id = id;
    strcpy(node->gr_name, Name);
    node->gr_stepcount= step;
	node->left = NULL; 
	node->right = NULL; 
	node->height = 1; // new node is initially added at leaf 
	return(node); 
} 

// A utility function to right rotate subtree rooted with y  
leader *getrightRotate(leader *y) 
{ 
	leader *x = y->left; 
	leader *T2 = x->right; 

	// Perform rotation 
	x->right = y; 
	y->left = T2; 

	// Update heights 
	y->height = getmax(getheight(y->left), 
					getheight(y->right)) + 1; 
	x->height = getmax(getheight(x->left), 
					getheight(x->right)) + 1; 

	// Return new root 
	return x; 
} 

// A utility function to left rotate subtree rooted with x  
leader *getleftRotate(leader *x) 
{ 
	leader *y = x->right; 
	leader *T2 = y->left; 

	// Perform rotation 
	y->left = x; 
	x->right = T2; 

	// Update heights 
	x->height = getmax(getheight(x->left), 
					getheight(x->right)) + 1; 
	y->height = getmax(getheight(y->left), 
					getheight(y->right)) + 1; 

	// Return new root 
	return y; 
} 

// Get Balance factor of node N 
int getleaderBalance(leader *N) 
{ 
	if (N == NULL) 
		return 0; 
	return getheight(N->left) - getheight(N->right); 
} 

// Recursive function to insert a mem_id in the subtree rooted 
// with node and returns the new root of the subtree. 
leader* Add_leader(leader* node, int id,char *Name,int stepcount) 
{ 
	/* 1. Perform the normal BST insertion */
	if (node == NULL) 
		return(newleaderNode(id,Name, stepcount)); 

	if (stepcount < node->gr_stepcount) 
		node->left = Add_leader(node->left, id,Name, stepcount); 
	else if (stepcount > node->gr_stepcount) 
		node->right = Add_leader(node->right, id,Name, stepcount); 
	else // Equal mem_ids are not allowed in BST 
		return node; 

	/* 2. Update height of this ancestor node */
	node->height = 1 + getmax(getheight(node->left), 
						getheight(node->right)); 

	/* 3. Get the balance factor of this ancestor 
		node to check whether this node became 
		unbalanced */
	int balance = getleaderBalance(node); 

	// If this node becomes unbalanced, then 
	// there are 4 cases 

	// Left Left Case 
	if (balance > 1 && stepcount < node->left->gr_stepcount) 
		return getrightRotate(node); 

	// Right Right Case 
	if (balance < -1 && stepcount > node->right->gr_stepcount) 
		return getleftRotate(node); 

	// Left Right Case 
	if (balance > 1 && stepcount > node->left->gr_stepcount) 
	{ 
		node->left = getleftRotate(node->left); 
		return getrightRotate(node); 
	} 

	// Right Left Case 
	if (balance < -1 && stepcount < node->right->gr_stepcount) 
	{ 
		node->right = getrightRotate(node->right); 
		return getleftRotate(node); 
	} 

	/* return the (unchanged) node pointer */
	return node; 
}

// Function to print inorder traversal of AVL tree
void inorderTraversal(leader* root) {
    static int rank =1;
    if (root != NULL) {
        inorderTraversal(root->right);
        printf("\n %d\t%10s \t%d", rank++,root->gr_name, root->gr_stepcount);
        inorderTraversal(root->left);
    }
}

// Function to deallocate memory allocated for AVL tree
void deallocateAVL(leader* root) {
    if (root != NULL) {
        deallocateAVL(root->left);
        deallocateAVL(root->right);
        free(root);
    }
}

void Generate_leader_board(group *groot)
{
    int size = getSize(groot);
    leader* array = (leader*)malloc(size * sizeof(leader));
    int index = 0;
    treeToarray(groot, array, &index);
    leader *lroot = NULL;
    for (int i = 0; i < size; i++)
    {
        lroot = Add_leader(lroot,array[i].gr_id,array[i].gr_name,array[i].gr_stepcount);
    }
    printf("Inorder traversal of AVL tree: ");
    inorderTraversal(lroot);
    // printf("%d\n",avlRoot->left->gr_stepcount);
    deallocateAVL(lroot);
}

//function that gives top 3 individual from individual tree
void get_top_3(ind *root, int *top3_individuals,int *top3)
{
    ind *current = root;
    if(current != NULL)
    {
        int total_steps = 0;
        int flag = 1; // Reset flag for each individual
        for (int i = 0; i < 7; i++)
        {
            if (current->stepcount[i] > current->ind_goal)
            {
                total_steps += current->stepcount[i];
            }
            else
            {
                flag = 0;
                break; // No need to continue summing if goal not met
            }
        }
        if (flag)
        {
            if (total_steps > top3[0])
            {
                top3_individuals[2] = top3_individuals[1];
                top3_individuals[1]  = top3_individuals[0];
                top3_individuals[0] = current->mem_id;
                top3[2] = top3[1];
                top3[1]  = top3[0];
                top3[0] = total_steps;
            }
            else if (total_steps > top3[1])
            {
                top3_individuals[2] = top3_individuals[1];
                top3_individuals[1]  = current->mem_id;
                top3[2] = top3[1];
                top3[1]  = total_steps;
            }
            else if (total_steps > top3[2])
            {
                top3_individuals[2]  = current->mem_id;
                top3[2]  = total_steps;
            }
        }
        get_top_3(root->left,top3_individuals,top3);
        get_top_3(root->right,top3_individuals,top3);
    }
}

// Utility function to search a key in a BST
ind* search(ind* root, int mem_id)
{
    // Base Cases: root is null or key is present at root
    if (root == NULL || root->mem_id == mem_id)
        return root;
 
    // Key is greater than root's key
    if (root->mem_id < mem_id)
        return search(root->right, mem_id);
 
    // Key is smaller than root's key
    return search(root->left, mem_id);
}

void print_top3(int *top3_individuals,int *top3,ind *root)
{
     // Print top 3 individuals
    printf("Top 3 individuals:\n");
    for (int i = 0; i < 3; i++)
    {
        if (top3_individuals[i] != 0)
        {
            ind *curr = search(root,top3_individuals[i]);
            
            if (curr == NULL)
            {
                printf("ERROR");
            }
            else
            {
                printf("%d. ID: %d, Name: %s, Total Steps: %d\n", i + 1,
                       top3_individuals[i], curr->Name, top3[i]);
            }
        }
    }
}

void Check_individual_rewards(int id, ind *head)
{
    int *arr, *ptr;
    arr = (int *)malloc(sizeof(int) * 3);
    ptr = (int *)malloc(sizeof(int) * 3);
    get_top_3(head, arr,ptr);
    if (arr[0] == id)
    {
        printf("The person with person id:%d is on top 1 of the leaderboard\nReward:100 Points\n", id);
    }
    else if (arr[1] == id)
    {
        printf("The person with person id:%d is on top 2 of the leaderboard\nReward:75 Points\n", id);
    }
    else if (arr[2] == id)
    {
        printf("The person with person id:%d is on top 3 of the leaderboard\nReward:50 Points\n", id);
    }
    else
    {
        printf("The person with person id:%d is not in the top 3 of the leaderboard\n", id);
    }
    free(arr);
    free(ptr);
}

void Suggest_goal_update(ind *head)
{
    int *arr, *ptr, goal;
    arr = (int *)malloc(sizeof(int) * 3);
    ptr = (int *)malloc(sizeof(int) * 3);
    get_top_3(head, arr,ptr);
    for (int i = 0; i < 3; i++)
    {
        goal = ptr[i] / 7;
        printf("\nyou need to have minimum daily goal of %d for rank %d \n", goal + 1, i + 1);
    }
    free(arr);
    free(ptr);
}

void Check_group_achievement(int group_id, group *head)
{
    group *curr = head;
    int flag = 0;
    while (curr != NULL && !flag)
    {
        if (group_id == curr->gr_id)
        {
            flag = 1;
        }
        else if (group_id < curr->gr_id)
        {
            curr = curr->left;
        }
        else
        {
            curr = curr->right;
        }
    }
    if (flag)
    {
        int sum = 0;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < 7 && (curr->arr_mem[i] != NULL); j++)
            {
                sum = sum + curr->arr_mem[i]->stepcount[j];
            }
        }
        if (sum >= curr->gr_goal)
        {
            printf("Group id:%d \n The group with above group id have achieved the "
                   "group goal for this week\n Cumulative step count of group:%d \n "
                   "Group goal:%d",
                   curr->gr_id, sum, curr->gr_goal);
        }
        else
        {
            printf("Group id:%d \n The group with above group id have not achieved "
                   "the group goal for this week\n Cumulative step count of group:%d "
                   "\n Group goal:%d \n The group is short of %d no. of steps in the "
                   "current week",
                   curr->gr_id, sum, curr->gr_goal, curr->gr_goal - sum);
        }
    }
    else
    {
        printf("The group with group id:%d not found in the list", group_id);
    }
}

group *Delete_group(group *root, int gr_id) {
    if (root == NULL)
        return root;

    if (gr_id < root->gr_id)
        root->left = Delete_group(root->left, gr_id);
    else if (gr_id > root->gr_id)
        root->right = Delete_group(root->right, gr_id);
    else {
        if ((root->left == NULL) || (root->right == NULL)) {
            group *temp = root->left ? root->left : root->right;
            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else
                *root = *temp;
            // update_group_file(root);
            free(temp);
        } else {
            group *temp = root->right;
            while (temp->left != NULL)
                temp = temp->left;
            root->gr_id = temp->gr_id;
            root->right = Delete_group(root->right, temp->gr_id);
        }
    }
    if (root == NULL)
        return root;

    root->height = 1 + max(gheight(root->left), gheight(root->right));
    int balance = getgroupBalance(root);

    if (balance > 1 && getgroupBalance(root->left) >= 0)
        return grouprightRotate(root);

    if (balance > 1 && getgroupBalance(root->left) < 0) {
        root->left = groupleftRotate(root->left);
        return grouprightRotate(root);
    }
    if (balance < -1 && getgroupBalance(root->right) <= 0)
        return groupleftRotate(root);

    if (balance < -1 && getgroupBalance(root->right) > 0) {
        root->right = grouprightRotate(root->right);
        return groupleftRotate(root);
    }
    return root;
}

group* Merge_groups(int id1,int id2,group *root)
{
    group *ptr1, *ptr2;
    ptr1 = ptr2 =root;
    while (ptr1 != NULL && ptr1->gr_id != id1)
    {
        if (ptr1->gr_id < id1)
        {
            ptr1 = ptr1->right;
        }
        else
        {
            ptr1 = ptr1->left;
        }
    }
    while (ptr2 != NULL && ptr2->gr_id != id2)
    {
        if (ptr2->gr_id < id2)
        {
            ptr2 = ptr2->right;
        }
        else
        {
            ptr2 = ptr2->left;
        }
    }
    if (ptr1 == NULL || ptr2 == NULL)
    {
        printf("One or both of the groups not found.\n");
    }
    else
    {
        int c=0;
        for (int i = 0; i < 5; i++)
        {
            if (ptr1->memberIDs[i] != 0 || ptr2->memberIDs[i] != 0)
            {
                c++;
            }         
        }
        if (c>5)
        {
            printf("merge unsuccessful, total members exceed group size...!");
        }
        else
        {
            ptr1->gr_goal = ptr1->gr_goal + ptr2->gr_goal;
            int x=0;
            for (int j = 0; j < 5; j++)
            {
                if (ptr1->memberIDs[j] == 0)
                {
                    while (ptr2->memberIDs[x] == 0)
                    {
                        x++;
                    }
                    ptr1->memberIDs[j] = ptr2->memberIDs[x];
                    ptr1->arr_mem[j] = ptr2->arr_mem[x];
                    x++;
                }
            }
            root = Delete_group(root,ptr2->gr_id);
            printf("%d is merged into %d successfully",id2,id1);
        }  
    }
    return root;
}

/* Given a non-empty binary search tree, return the
   node with minimum key value found in that tree.
   Note that the entire tree does not need to be
   searched. */
ind *minValueNode(ind *node)
{
    ind *current = node;

    /* loop down to find the leftmost leaf */
    while (current->left != NULL)
        current = current->left;

    return current;
}

// int Delete_ind_from_Group(group *groot,int mem_id)
// {
//     int x=0;
//     if (groot!=NULL)
//     {
//         for (int i = 0; i < SIZE && !x; i++)
//         {
//             if (groot->memberIDs[i]==mem_id)
//             {
//                 groot->memberIDs[i]=0;
//                 groot->arr_mem[i]=NULL;
//                 x=1;
//             }
//         }
//         x = Delete_ind_from_group(groot->left,mem_id);
//         if (!x)
//         {
//             x = Delete_ind_from_group(groot->right,mem_id);
//         }
//     }
//     return x;
// }

int Delete_ind_from_group(group* groot,int mem_id)
{
    static int found=0;
    int all_empty=1;
    if (groot==NULL)
    {
        return found;
    }
    else
    {
        for (int i = 0; i < 7 ; i++)
        {
            if(groot->memberIDs[i]==mem_id && !found)
            {
                found=1;
                groot->memberIDs[i]=0;
                groot->arr_mem[i]=NULL;
            }
            // if (found && i+1<7)
            // {
            //     groot->memberIDs[i]=groot->memberIDs[i+1];
            //     groot->arr_mem[i]=groot->arr_mem[i+1];

            // }
            // else if (found && i==6)
            // {
            //     groot->memberIDs[i]=0;
            //     groot->arr_mem[i]=NULL;
            // }
            
            
        }
        
        for (int i = 0; i < 7; i++)
        {
            if (groot->memberIDs[i]!=0)
            {
                all_empty=0;
            }
            
        }
        

        if (!found)
        {
            found=Delete_ind_from_group(groot->left,mem_id);
            if (!found)
            {
                found=Delete_ind_from_group(groot->right,mem_id);
            }
            
        }
        
        if (all_empty)
        {
            groot=Delete_group(groot,groot->gr_id);
        }
        
        
        return found;
    }
    
}

// Recursive function to delete a node with given key
// from subtree with given root. It returns root of
// the modified subtree.
ind* Delete_Individual(ind* root,group *groot, int mem_id)
{
    static int found=0;
    // STEP 1: PERFORM STANDARD BST DELETE
    
    if (root == NULL)
        return root;
 
    // If the key to be deleted is smaller than the
    // root's key, then it lies in left subtree
    if ( mem_id < root->mem_id )
        root->left = Delete_Individual(root->left,groot, mem_id);
 
    // If the key to be deleted is greater than the
    // root's key, then it lies in right subtree
    else if( mem_id > root->mem_id )
        root->right = Delete_Individual(root->right,groot, mem_id);
 
    // if key is same as root's key, then This is
    // the node to be deleted
    else
    {
        // node with only one child or no child
        if( (root->left == NULL) || (root->right == NULL) )
        {
            ind *temp = root->left ? root->left :
                                             root->right;
 
            // No child case
            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else // One child case
             *root = *temp; // Copy the contents of
                            // the non-empty child
            free(temp);
        }
        else
        {
            // node with two children: Get the inorder
            // successor (smallest in the right subtree)
            ind* temp = minValueNode(root->right);
 
            // Copy the inorder successor's data to this node
            root->mem_id = temp->mem_id;
 
            // Delete the inorder successor
            root->right = Delete_Individual(root->right,groot, temp->mem_id);
        }
    }
 
    // If the tree had only one node then return
    if (root == NULL)
      return root;
 
    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    root->height = 1 + max(height(root->left),
                           height(root->right));
 
    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to
    // check whether this node became unbalanced)
    int balance = getBalance(root);
 
    // If this node becomes unbalanced, then there are 4 cases
 
    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);
 
    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0)
    {
        root->left =  leftRotate(root->left);
        return rightRotate(root);
    }
 
    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);
 
    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    found+=Delete_ind_from_group(groot,mem_id);
    if (found==1)
    {
        printf("\nIndividual deleted individual tree and from group as well\n");
    }
    return root;
}


int main()
{
    int NO_of_individuals =20;
    int NO_of_groups=5;
    FILE *fptr = fopen("input.txt", "r");
    FILE *gptr = fopen("group.txt", "r");
    struct individual* head;
    head = NULL;
    group *ghead;
    ghead = NULL;
    // leader *lead;
    int mem_id, age, ind_goal;
    char Name[NAME_SIZE];
    int stepcount[7];
    for (int i = 0; i < NO_of_individuals; i++)
    {
        fscanf(fptr, "%d", &mem_id);
        fflush(stdin);
        fscanf(fptr, "%s", Name);
        fflush(stdin);
        fscanf(fptr, "%d", &age);
        fflush(stdin);
        fscanf(fptr, "%d", &ind_goal);
        fflush(stdin);

        for (int j = 0; j < 7; j++)
        {
            fscanf(fptr, "%d", &stepcount[j]);
        }
        head = Add_Person(head,mem_id, Name, age, ind_goal, stepcount);
        printf("%d is added\n", mem_id);
    }

    int group_id;
    int group_goal;
    int NO_of_members;
    char group_name[NAME_SIZE];
    int members_Id[5];
    for (int i = 0; i < NO_of_groups; i++)
    {
        fscanf(gptr, "%d", &group_id);
        fflush(stdin);
        fscanf(gptr, "%s", group_name);
        fflush(stdin);
        fscanf(gptr, "%d", &group_goal);
        fflush(stdin);
        ghead = Add_group(ghead,group_id,group_name,group_goal);
        fscanf(gptr, "%d", &NO_of_members);
        fflush(stdin);
        for (int i = 0; i < NO_of_members; i++)
        {
            fscanf(gptr, "%d", &members_Id[i]);
            struct individual *curr = head;
            while (curr != NULL && curr->mem_id != members_Id[i])
            {
                if (curr->mem_id < members_Id[i])
                {
                    curr= curr->right;
                }
                else
                {
                    curr = curr->left;
                }
            }
            if (curr == NULL)
            {
                printf("ERROR:-The individual with member id-%d not found in the list\n", members_Id[i]);
            }
            else
            {
                group *current = ghead;
                while (current != NULL && current->gr_id != group_id)
                {
                    if (current->gr_id < group_id)
                {
                    current= current->right;
                }
                else
                {
                    current = current->left;
                }
                }
                if (current == NULL)
                {
                    printf("ERROR:-The group with group id-%d not found in the list\n", group_id);
                }
                else
                {
                    addIndividualToGroup(current, curr, ghead);
                }
            }
        }
        printf("%d is created and added successfully \n", group_id);
    }
    fclose(fptr);
    fclose(gptr);
    int choice = 0;
    while (choice != 13)
    {
        printf("\n\n\n\n\nCHOOSE ANY OPTION:-\n");
        printf("1- Add a person\n2- Create a group\n3- Get top 3 winners\n4- Check group achievements\n5- Generate Leaderboard of groups\n6- check individual rewards\n7- delete individual\n8- Delete a group\n9- suggest goal updates for any person\n10- merge two groups\n11- display group info\n12-display all individual info\n13-EXIT\n");
        printf("Enter:");
        scanf("%d", &choice);
        if (choice == 1)
        {
            printf("Enter ID : \n");
            scanf("%d", &mem_id);
            printf("Enter Name : \n");
            scanf("%s", Name);
            printf("Enter age : \n");
            scanf("%d", &age);
            printf("Enter individual goal : \n");
            scanf("%d", &ind_goal);
            printf("Enter Stepcount of 7 Days : \n");
            for (int j = 0; j < 7; j++)
            {
                scanf("%d", &stepcount[j]);
            }
            head = Add_Person(head,mem_id, Name, age, ind_goal, stepcount);
            FILE *fptr1 = fopen("input.txt", "a");
            fprintf(fptr1, "\n");
            fprintf(fptr1, "%d %s %d %d", mem_id, Name, age, ind_goal);
            for (int i = 0; i < 7; i++)
            {
                fprintf(fptr1, " %d", stepcount[i]);
            }

            fclose(fptr1);
        }
        else if (choice == 2)
        {
            printf("Enter group ID : \n");
            scanf("%d", &group_id);
            printf("Enter group Name : \n");
            scanf("%s", group_name);
            printf("Enter weekly group goal : \n");
            scanf("%d", &group_goal);
            printf("Enter no of members : \n");
            scanf("%d", &NO_of_members);
            printf("Enter id of members : \n");
            for (int j = 0; j < NO_of_members; j++)
            {
                scanf("%d", &members_Id[j]);
            }
            ghead = Add_group(ghead,group_id,group_name,group_goal);
            group *current = ghead;
            while (current != NULL && current->gr_id != group_id)
            {
                if (current->gr_id < group_id)
                {
                    current = current->right;
                }
                else
                {
                    current = current->left;
                }
            }
            ind *curr = head;
            for (int i = 0; i < NO_of_members; i++)
            {
                while (curr != NULL && curr->mem_id != members_Id[i])
                {
                    if (curr->mem_id < members_Id[i])
                    {
                        curr = curr->right;
                    }
                    else
                    {
                        curr = curr->left;
                    }
                }
                addIndividualToGroup(current, curr, ghead);
            }
            
            FILE *fptr2 = fopen("group.txt", "a");
            fprintf(fptr2, "\n");
            fprintf(fptr2, "%d %s %d %d", group_id,group_name,group_goal,NO_of_members);
            for (int i = 0; i < NO_of_members; i++)
            {
                fprintf(fptr2, " %d", members_Id[i]);
            }

            fclose(fptr2);
        }
        else if (choice == 3)
        {
            int *arr,*ptr;
            arr = (int *)malloc(sizeof(int) * 3);
            ptr = (int *)malloc(sizeof(int) * 3);
            memset(ptr,0,3*sizeof(int));
            memset(arr,0,3*sizeof(int));
            get_top_3(head, arr,ptr);
            print_top3(arr,ptr,head);
            free(arr);
            free(ptr);
        }
        else if (choice == 4)
        {
            int id;
            printf("enter group id no.:");
            scanf("%d", &id);
            Check_group_achievement(id, ghead);
        }
        else if (choice == 5)
        {
            Generate_leader_board(ghead);
        }
        else if (choice == 6)
        {
            int id;
            printf("enter id no.:");
            scanf("%d", &id);
            Check_individual_rewards(id, head);
        }
        else if (choice == 7)
        {
            int id;
            printf("enter id no.:");
            scanf("%d", &id);
            head = Delete_Individual(head,ghead,id);
            FILE *fptr3 = fopen("group.txt","w");
            update_group_file(ghead,fptr3);
            fclose(fptr3);
            FILE *fptr4 = fopen("input.txt","w");
            update_individual_file(head,fptr4);
            fclose(fptr4);
        }
        else if (choice == 8)
        {
            int id;
            printf("enter group id no.:");
            scanf("%d", &id);
            FILE *fptr5 = fopen("group.txt","w");
            ghead = Delete_group(ghead,id);
            update_group_file(ghead,fptr5);
            fclose(fptr5);
        }
        else if (choice == 9)
        {
            Suggest_goal_update(head);
        }
        else if (choice == 10)
        {
            int id1, id2;
            printf("enter group id Nos.:");
            scanf("%d", &id1);
            scanf("%d", &id2);
            ghead = Merge_groups(id1, id2, ghead);
        }
        else if (choice == 11)
        {
            display_group_info(ghead);
        }
        else if (choice == 12)
        {
            display_indiv_info(head);
        }
        else if (choice == 13)
        {
            printf("EXITING...");
        }
        else
        {
            printf("wrong input, try again...!");
        }
    }
    return 0;
}