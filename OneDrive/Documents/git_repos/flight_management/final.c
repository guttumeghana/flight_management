//DSPD ASSIGNMENT-2 USING AVL TREES
//SEARCH FUNCTION BASED ON FLIGHT ID

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure for Time
typedef struct {
    int hours;
    int minutes;
} TIME;

// Structure for Flight Plan
typedef struct {
    int flightID;
    TIME departureTime;
    TIME ETA;
} FlightPlan;

// Structure for AVL Tree Node of Flight Plan
typedef struct FlightAVLNode {
    FlightPlan data;
    struct FlightAVLNode* left;
    struct FlightAVLNode* right;
    int height;
} FlightAVLNode;

// Structure for AVL Tree of Flight Plan
typedef struct {
    FlightAVLNode* root;
} FlightAVLTree;

// Structure for AVL Tree Node of Bucket
typedef struct BucketAVLNode {
    int bucketID;
    TIME startTime;
    TIME endTime;
    FlightAVLTree flightPlans;  // AVL tree of flight plans in this bucket
    struct BucketAVLNode* left;
    struct BucketAVLNode* right;
    int height;
} BucketAVLNode;

// Structure for AVL Tree of Buckets
typedef struct {
    BucketAVLNode* root;
} BucketAVLTree;

//GIVEN DEFINED FUNCTIONS

// Definition of the timediff function
int timediff(TIME A, TIME B) {
    // Calculate the difference between two times in minutes
    return (A.hours - B.hours) * 60 + (A.minutes - B.minutes);
}

// Definition of the maxtime function
int maxtime(TIME A, TIME B) {
    if (A.hours == B.hours) {
        if (A.minutes == B.minutes) {
            return 0;  // A and B are equal
        } else if (A.minutes < B.minutes) {
            return -1;  // A precedes B
        } else {
            return 1;  // A succeeds B
        }
    } else if (A.hours < B.hours) {
        return -1;  // A precedes B
    } else {
        return 1;  // A succeeds B
    }
}

// Get the maximum of two integers
int max(int a, int b) {
    return (a > b) ? a : b;
}

//BASIC FUNCTIONS OF AVL TREES

// Get the height of a Flight AVL node
int getFlightHeight(FlightAVLNode* node) {
    if (node == NULL)
        return 0;
    return node->height;
}

// Get the height of a Bucket AVL node
int getBucketHeight(BucketAVLNode* node) {
    if (node == NULL)
        return 0;
    return node->height;
}

//AVL TREE FUNCTIONS FOR FLIGHTS

// Create a new AVL node for Flight Plan
FlightAVLNode* newFlightAVLNode(FlightPlan data) {
    FlightAVLNode* node = (FlightAVLNode*)malloc(sizeof(FlightAVLNode));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

// Get balance factor of a Flight AVL node
int getFlightBalance(FlightAVLNode* node) {
    if (node == NULL)
        return 0;
    return getFlightHeight(node->left) - getFlightHeight(node->right);
}

// AVL TREE FUNCTIONS FOR BUCKETS

// Create a new AVL node for Bucket
BucketAVLNode* newBucketAVLNode(int bucketID, TIME startTime, TIME endTime) {
    BucketAVLNode* node = (BucketAVLNode*)malloc(sizeof(BucketAVLNode));
    node->bucketID = bucketID;
    node->startTime = startTime;
    node->endTime = endTime;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    node->flightPlans.root = NULL;  // Initialize flight plans AVL tree to NULL
    return node;
}

// Get balance factor of a Bucket AVL node
int getBucketBalance(BucketAVLNode* node) {
    if (node == NULL)
        return 0;
    return getBucketHeight(node->left) - getBucketHeight(node->right);
}

//ROTATE FUNCTIONS FOR FLIGHT PLANS

// Right rotate subtree rooted with y for Flight Plan AVL
FlightAVLNode* rightRotateFlight(FlightAVLNode* y) {
    FlightAVLNode* x = y->left;
    FlightAVLNode* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(getFlightHeight(y->left), getFlightHeight(y->right)) + 1;
    x->height = max(getFlightHeight(x->left), getFlightHeight(x->right)) + 1;

    // Return new root
    return x;
}

// Left rotate subtree rooted with x for Flight Plan AVL
FlightAVLNode* leftRotateFlight(FlightAVLNode* x) {
    FlightAVLNode* y = x->right;
    FlightAVLNode* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(getFlightHeight(x->left), getFlightHeight(x->right)) + 1;
    y->height = max(getFlightHeight(y->left), getFlightHeight(y->right)) + 1;

    // Return new root
    return y;
}

//ROTATE FUNCTIONS FOR BUCKETS 

// Right rotate subtree rooted with y for Bucket AVL
BucketAVLNode* rightRotateBucket(BucketAVLNode* y) {
    BucketAVLNode* x = y->left;
    BucketAVLNode* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(getBucketHeight(y->left), getBucketHeight(y->right)) + 1;
    x->height = max(getBucketHeight(x->left), getBucketHeight(x->right)) + 1;

    // Return new root
    return x;
}

// Left rotate subtree rooted with x for Bucket AVL
BucketAVLNode* leftRotateBucket(BucketAVLNode* x) {
    BucketAVLNode* y = x->right;
    BucketAVLNode* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(getBucketHeight(x->left), getBucketHeight(x->right)) + 1;
    y->height = max(getBucketHeight(y->left), getBucketHeight(y->right)) + 1;

    // Return new root
    return y;
}

//QUESTION 2:
//TO INSERT A NEW FLIGHT PLAN INTO AN ACTIVE BUCKET

// Insert a new flight plan into Flight AVL tree
FlightAVLNode* insertFlightIntoTree(FlightAVLNode* node, FlightPlan data) {
    // Perform normal BST insertion
    if (node == NULL)
        return newFlightAVLNode(data);

    if (data.departureTime.hours < node->data.departureTime.hours ||
        (data.departureTime.hours == node->data.departureTime.hours &&
         data.departureTime.minutes < node->data.departureTime.minutes)) {
        node->left = insertFlightIntoTree(node->left, data);
    } else if (data.departureTime.hours > node->data.departureTime.hours ||
               (data.departureTime.hours == node->data.departureTime.hours &&
                data.departureTime.minutes > node->data.departureTime.minutes)) {
        node->right = insertFlightIntoTree(node->right, data);
    } else {
        // Duplicate keys are not allowed
        printf("Error: Duplicate flight ID %d. Cannot insert.\n", data.flightID);
        return node;
    }

    // Update height of this ancestor node
    node->height = 1 + max(getFlightHeight(node->left), getFlightHeight(node->right));

    // Get the balance factor
    int balance = getFlightBalance(node);

    // If the node becomes unbalanced, there are 4 cases

    // Left Left Case
    if (balance > 1 && data.departureTime.hours < node->left->data.departureTime.hours)//heavy by left
        return rightRotateFlight(node);

    // Right Right Case
    if (balance < -1 && data.departureTime.hours > node->right->data.departureTime.hours)//heavy by right
        return leftRotateFlight(node);

    // Left Right Case
    if (balance > 1 && data.departureTime.hours > node->left->data.departureTime.hours) {
        node->left = leftRotateFlight(node->left);
        return rightRotateFlight(node);
    }

    // Right Left Case
    if (balance < -1 && data.departureTime.hours < node->right->data.departureTime.hours) {
        node->right = rightRotateFlight(node->right);
        return leftRotateFlight(node);
    }

    // Return the unchanged node pointer
    return node;
}

// Insert a new bucket into Bucket AVL tree
BucketAVLNode* insertBucketIntoTree(BucketAVLNode* node, BucketAVLNode* newBucket) {
    // Perform normal BST insertion
    if (node == NULL)
        return newBucket;

    if (maxtime(newBucket->startTime, node->startTime) == -1) {
        node->left = insertBucketIntoTree(node->left, newBucket);
    } else {
        node->right = insertBucketIntoTree(node->right, newBucket);
    }

    // Update height of this ancestor node
    node->height = 1 + max(getBucketHeight(node->left), getBucketHeight(node->right));

    // Get the balance factor
    int balance = getBucketBalance(node);

    // If the node becomes unbalanced, there are 4 cases

    // Left Left Case
    if (balance > 1 && maxtime(newBucket->startTime, node->left->startTime) == -1)
        return rightRotateBucket(node);

    // Right Right Case
    if (balance < -1 && maxtime(newBucket->startTime, node->right->startTime) == 1)
        return leftRotateBucket(node);

    // Left Right Case
    if (balance > 1 && maxtime(newBucket->startTime, node->left->startTime) == 1) {
        node->left = leftRotateBucket(node->left);
        return rightRotateBucket(node);
    }

    // Right Left Case
    if (balance < -1 && maxtime(newBucket->startTime, node->right->startTime) == -1) {
        node->right = rightRotateBucket(node->right);
        return leftRotateBucket(node);
    }

    // Return the unchanged node pointer
    return node;
}

// Function to insert a new flight plan into the digital dashboard
BucketAVLNode* insertFlightIntoDashboard(BucketAVLNode* root, FlightPlan newFlight) {
    // Find the bucket where this flight belongs
    if (root == NULL) {
        printf("Error: No active buckets exist.\n");
        return NULL;
    }

    BucketAVLNode* current = root;

    while (current != NULL) {
        if (maxtime(newFlight.ETA, current->startTime) != -1 &&
            maxtime(newFlight.ETA, current->endTime) != 1) {
            // This flight belongs to this bucket
            current->flightPlans.root = insertFlightIntoTree(current->flightPlans.root, newFlight);
            return root;
        } else if (maxtime(newFlight.ETA, current->startTime) == -1) {
            // Go to the left subtree
            current = current->left;
        } else {
            // Go to the right subtree
            current = current->right;
        }
    }

    // If no appropriate bucket found
    printf("Error: No matching bucket found for the flight.\n");
    return root;
}

//QUESTION 3:
//TO CANCEL A FLIGHT PLAN

// Function to search for a flight plan by ID
FlightAVLNode* searchFlightByID(FlightAVLNode* root, int flightID) {
    if (root == NULL || root->data.flightID == flightID)
        return root;

    if (flightID < root->data.flightID)
        return searchFlightByID(root->left, flightID);

    return searchFlightByID(root->right, flightID);
}

// Function to delete a node with a given key and return the new root of the AVL tree
FlightAVLNode* deleteFlightByID(FlightAVLNode* root, int flightID) {
    if (root == NULL)
        return root;

    // If the key to be deleted is smaller than the root's key, then it lies in the left subtree
    if (flightID < root->data.flightID)
        root->left = deleteFlightByID(root->left, flightID);

    // If the key to be deleted is greater than the root's key, then it lies in the right subtree
    else if (flightID > root->data.flightID)
        root->right = deleteFlightByID(root->right, flightID);

    // If key is the same as root's key, then this is the node to be deleted
    else {
        // Node with only one child or no child
        if (root->left == NULL || root->right == NULL) {
            FlightAVLNode* temp = root->left ? root->left : root->right;//temp holds the child which is present

            // No child case
            if (temp == NULL) {
                temp = root;
                root = NULL;
            } else  // One child case
                *root = *temp;  // Copy the contents of the non-empty child

            free(temp);
        } else {
            // Node with two children: Get the inorder successor (smallest in the right subtree)
            FlightAVLNode* temp = root->right;
            while (temp->left != NULL)
                temp = temp->left;

            // Copy the inorder successor's data to this node
            root->data = temp->data;

            // Delete the inorder successor
            root->right = deleteFlightByID(root->right, temp->data.flightID);
        }
    }

    // If the tree had only one node, then return
    if (root == NULL)
        return root;

    // Update the height of the current node
    root->height = 1 + max(getFlightHeight(root->left), getFlightHeight(root->right));

    // Get the balance factor of this node to check if it became unbalanced
    int balance = getFlightBalance(root);

    // If this node becomes unbalanced, then there are 4 cases

    // Left Left Case
    if (balance > 1 && getFlightBalance(root->left) >= 0)
        return rightRotateFlight(root);

    // Left Right Case
    if (balance > 1 && getFlightBalance(root->left) < 0) {
        root->left = leftRotateFlight(root->left);
        return rightRotateFlight(root);
    }

    // Right Right Case
    if (balance < -1 && getFlightBalance(root->right) <= 0)
        return leftRotateFlight(root);

    // Right Left Case
    if (balance < -1 && getFlightBalance(root->right) > 0) {
        root->right = rightRotateFlight(root->right);
        return leftRotateFlight(root);
    }

    return root;
}

// Function to cancel a flight plan by ID in the digital dashboard
BucketAVLNode* cancelFlightPlanByID(BucketAVLNode* root, int flightID) {
    if (root == NULL) {
        printf("Error: No active buckets exist.\n");
        return NULL;
    }

    BucketAVLNode* currentBucket = root;
    while (currentBucket != NULL) {
        // Search for the flight ID in the flight plans AVL tree of this bucket
        FlightAVLNode* foundFlight = searchFlightByID(currentBucket->flightPlans.root, flightID);
        if (foundFlight != NULL) {
            // Flight found, delete it from this bucket
            currentBucket->flightPlans.root = deleteFlightByID(currentBucket->flightPlans.root, flightID);
            printf("Flight with ID %d cancelled.\n", flightID);
            return root;
        } else {
            // Flight not found in this bucket, move to the appropriate subtree
            if (flightID < currentBucket->flightPlans.root->data.flightID) {
                currentBucket = currentBucket->left;
            } else {
                currentBucket = currentBucket->right;
            }
        }
    }

    // Flight not found in any bucket
    printf("Error: Flight with ID %d not found in any bucket.\n", flightID);
    return root;
}

//QUESTION 4:
//TO SHOW STATUS OF A PARTICULAR FLIGHT PLAN

// Function definition for displayFlightDetails
void displayFlightDetails(FlightPlan flight,int bucketID) {
    printf("Bucket ID: %d\n", bucketID);
    printf("Flight ID: %d\n", flight.flightID);
    printf("Departure Time: %02d:%02d\n", flight.departureTime.hours, flight.departureTime.minutes);
    printf("ETA: %02d:%02d\n", flight.ETA.hours, flight.ETA.minutes);
}


// Function to show status of a particular flight plan by ID
void showFlightStatusByID(BucketAVLNode* root, int flightID) {
    if (root == NULL) {
        printf("Error: No active buckets exist.\n");
        return;
    }

    BucketAVLNode* currentBucket = root;
    while (currentBucket != NULL) {
        // Search for the flight ID in the flight plans AVL tree of this bucket
        FlightAVLNode* foundFlight = searchFlightByID(currentBucket->flightPlans.root, flightID);
        if (foundFlight != NULL) {
            // Flight found, display its details
            printf("Flight Status for ID %d:\n", flightID);
            displayFlightDetails(foundFlight->data,currentBucket->bucketID);
            return;
        } else {
            // Flight not found in this bucket, move to the appropriate subtree
            if (flightID < currentBucket->flightPlans.root->data.flightID) {
                currentBucket = currentBucket->left;
            } else {
                currentBucket = currentBucket->right;
            }
        }
    }

    // Flight not found in any bucket
    printf("Error: Flight with ID %d not found in any bucket.\n", flightID);
}

//QUESTION 5:
//TO SHOW THE FLIGHTS AVAILABLE WITHIN THE GIVEN INTERVAL OF TIME


// Function to perform range search in Flight AVL tree
void rangeSearchFlights(FlightAVLNode* root, TIME startTime, TIME endTime) {
    if (root == NULL) {
        return;
    }

    // If the current node's departure time falls in the range
    if (maxtime(root->data.departureTime, startTime) != -1 &&
        maxtime(root->data.departureTime, endTime) != 1) {
        // Display the flight plan
        printf("Flight ID: %d, Departure Time: %d:%02d, ETA: %d:%02d\n",
               root->data.flightID, root->data.departureTime.hours, root->data.departureTime.minutes,
               root->data.ETA.hours, root->data.ETA.minutes);
    }

    // If the left subtree might contain nodes in the range
    if (maxtime(root->data.departureTime, startTime) == -1) {
        rangeSearchFlights(root->right, startTime, endTime);
    }

    // If the right subtree might contain nodes in the range
    if (maxtime(root->data.departureTime, endTime) == 1) {
        rangeSearchFlights(root->left, startTime, endTime);
    }
}

// Function to perform range search in Bucket AVL tree
void rangeSearchBuckets(BucketAVLNode* root, TIME startTime, TIME endTime) {
    if (root == NULL) {
        return;
    }

    // If the current node's start time falls in the range
    if (maxtime(root->startTime, startTime) != -1 && maxtime(root->startTime, endTime) != 1) {
        // Display flight plans in this bucket
        printf("Bucket ID: %d, Start Time: %d:%02d, End Time: %d:%02d\n",
               root->bucketID, root->startTime.hours, root->startTime.minutes,
               root->endTime.hours, root->endTime.minutes);

        printf("Flight Plans in Bucket %d:\n", root->bucketID);
        rangeSearchFlights(root->flightPlans.root, startTime, endTime);
    }

    // If the left subtree might contain buckets in the range
    if (maxtime(root->startTime, startTime) == -1) {
        rangeSearchBuckets(root->right, startTime, endTime);
    }

    // If the right subtree might contain buckets in the range
    if (maxtime(root->startTime, endTime) == 1) {
        rangeSearchBuckets(root->left, startTime, endTime);
    }
}

// Function to perform range search in Flight AVL tree and display flights within the given interval
void showFlightsWithinInterval(BucketAVLNode* root, TIME startTime, TIME endTime) {
    if (root == NULL) {
        return;
    }

    // If the current node's start time falls in the range
    if (maxtime(root->startTime, startTime) != -1 && maxtime(root->startTime, endTime) != 1) {
        // Display flight plans in this bucket within the given interval
        printf("Bucket ID: %d, Start Time: %d:%02d, End Time: %d:%02d\n",
               root->bucketID, root->startTime.hours, root->startTime.minutes,
               root->endTime.hours, root->endTime.minutes);

        printf("Flight Plans within the given interval in Bucket %d:\n", root->bucketID);
        rangeSearchFlights(root->flightPlans.root, startTime, endTime);
    }

    // If the left subtree might contain buckets in the range
    if (maxtime(root->startTime, startTime) == -1) {
        showFlightsWithinInterval(root->right, startTime, endTime);
    }

    // If the right subtree might contain buckets in the range
    if (maxtime(root->startTime, endTime) == 1) {
        showFlightsWithinInterval(root->left, startTime, endTime);
    }
}

//TO GET TO KNOW THAT THE BUCKETS AND FLIGHTS ARE IN SORTED ORDER BY THEIR RESPECTIVE VALUES(IF CHOICE EQUALS 5)

// Function to display all Flight AVL Tree nodes (In-order traversal)
void displayAllFlights(FlightAVLNode* root) {
    if (root == NULL) {
        return;
    }
    displayAllFlights(root->left);

    printf("Flight ID: %d, Departure Time: %d:%02d, ETA: %d:%02d\n", 
           root->data.flightID, root->data.departureTime.hours, root->data.departureTime.minutes, 
           root->data.ETA.hours, root->data.ETA.minutes);

    displayAllFlights(root->right);
}

// Function to display all Bucket AVL Tree nodes (In-order traversal)
void displayAllBuckets(BucketAVLNode* root) {
    if (root == NULL) {
        return;
    }
    displayAllBuckets(root->left);
    
    printf("Bucket ID: %d, Start Time: %d:%02d, End Time: %d:%02d\n", 
           root->bucketID, root->startTime.hours, root->startTime.minutes, 
           root->endTime.hours, root->endTime.minutes);

    // Display flight plans in this bucket
    printf("Flight Plans in Bucket %d:\n", root->bucketID);
    displayAllFlights(root->flightPlans.root);

    displayAllBuckets(root->right);
}

/*
BucketAVLNode* findBucketByID(BucketAVLNode* root, int bucketID) {
    if (root == NULL || root->bucketID == bucketID) {
        return root;
    }

    if (bucketID < root->bucketID) {
        return findBucketByID(root->left, bucketID);
    } else {
        return findBucketByID(root->right, bucketID);
    }
}
*/

int main() {
    FILE* file = fopen("flights.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    BucketAVLNode* dashboard = NULL; // Initialize dashboard outside the loop

    char line[100];

    while (fgets(line, sizeof(line), file)) {
        int bucketID;
        TIME startTime, endTime;

        if (sscanf(line, "%d %d:%d %d:%d", &bucketID, &startTime.hours, &startTime.minutes,
                   &endTime.hours, &endTime.minutes) != 5) {
            fprintf(stderr, "Invalid bucket information: %s", line);
            continue;
        }

        BucketAVLNode* newBucket = newBucketAVLNode(bucketID, startTime, endTime);

        dashboard = insertBucketIntoTree(dashboard, newBucket);

        // Read flight plans for this bucket
        fgets(line, sizeof(line), file); // Consume the newline character left after reading the bucket information
        while (fgets(line, sizeof(line), file) && line[0] != '\n') {
            int flightID;
            TIME departureTime, ETA;
            if (sscanf(line, "%d %d:%d %d:%d", &flightID, &departureTime.hours, &departureTime.minutes,
                       &ETA.hours, &ETA.minutes) != 5) {
                fprintf(stderr, "Invalid flight information: %s", line);
                continue;
            }

            FlightPlan newFlight;
            newFlight.flightID = flightID;
            newFlight.departureTime = departureTime;
            newFlight.ETA = ETA;

            // Insert the new flight into the current bucket
            dashboard = insertFlightIntoDashboard(dashboard, newFlight);
        }
    }

    fclose(file);

    while (1) {
        int choice;
        printf("\n1. Insert a new flight plan\n");
        printf("2. Cancel a flight plan by ID\n");
        printf("3. Display status of a particular flight plan by ID\n");
        printf("4. Show flight plans within the given interval of time\n");
        printf("5. To print the buckets and their corresponding flights\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume the newline character

        if(choice==1)
        {
            // Insert a new flight plan
            FlightPlan newFlightPlan;
            printf("\nEnter details for new flight plan:\n");
            printf("Enter Flight ID: ");
            scanf("%d", &newFlightPlan.flightID);
            printf("Enter Departure Time (HH:MM): ");
            scanf("%d:%d", &newFlightPlan.departureTime.hours, &newFlightPlan.departureTime.minutes);
            printf("Enter ETA (HH:MM): ");
            scanf("%d:%d", &newFlightPlan.ETA.hours, &newFlightPlan.ETA.minutes);
            
            // Insert the new flight plan into the digital dashboard
            printf("Inserting new flight plan...\n");
            dashboard = insertFlightIntoDashboard(dashboard, newFlightPlan);
            
        }
        else if(choice==2)
        {
            // Cancel a flight plan by ID
            int cancelID;
            printf("\nEnter the Flight ID to cancel: ");
            scanf("%d", &cancelID);
            getchar(); // Consume the newline character

            // Call the cancelFlightPlanByID function
            dashboard = cancelFlightPlanByID(dashboard, cancelID);
           
        }
        else if(choice==3)
        {
            // Show status of a particular flight plan
            int searchID;
            printf("\nEnter the Flight ID to search: ");
            scanf("%d", &searchID);
            getchar(); // Consume the newline character

            // Call the showFlightStatusByID function
            showFlightStatusByID(dashboard, searchID);
        }
        else if(choice==4)
        {
            // Show flight plans between two given time intervals
            TIME startTime, endTime;
            printf("\nEnter the start time (HH:MM): ");
            scanf("%d:%d", &startTime.hours, &startTime.minutes);
            printf("Enter the end time (HH:MM): ");
            scanf("%d:%d", &endTime.hours, &endTime.minutes);

            // Perform range search in Bucket AVL tree
            printf("Flight plans between %02d:%02d and %02d:%02d:\n",
                   startTime.hours, startTime.minutes, endTime.hours, endTime.minutes);
            rangeSearchBuckets(dashboard, startTime, endTime);
            
        }
        else if(choice==5)
        {
            // Display all buckets and their flights
            printf("\nAll Buckets and their Flight Plans:\n");
            displayAllBuckets(dashboard);
        }
        else if(choice==6)
        {
            //To exit the program
            printf("Exiting...\n");
            break;
        }
        else
        {
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
