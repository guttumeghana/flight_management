//DSPD2 ASSIGNMENT1 USING SINGLY LINKED LIST
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

//QUESTION 1:TO DEFINE STRUCTURES OF TIME,FLIGHTPLAN AND BUCKET

// Structure to represent time (ETA, departure time)
typedef struct {
    int hours;
    int minutes;
} TIME;

// Structure to represent a flight plan
typedef struct FlightPlan {
    int flightID;
    TIME departureTime;
    TIME ETA;
    struct FlightPlan* next;
} FlightPlan;

// Structure to represent a bucket for the digital dashboard
typedef struct Bucket {
    int bucketID;
    TIME startTime;
    TIME endTime;
    FlightPlan* flightPlans;  // Linked list of flightplans in this bucket
    struct Bucket* next;
} Bucket;

//QUESTION 2:
//INSERTION OF A FLIGHT PLAN 
//Here it is given that the flight plan is added to active bucket only
//Given that these helper functions are available
int timedeff(TIME A, TIME B);
int maxtime(TIME A, TIME B);

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

// Function to insert a new flight plan into the digital dashboard
Bucket* insertFlightPlan(Bucket* dashboard, FlightPlan* newFlight) {
    Bucket* current = dashboard;
    Bucket* prev = NULL;

    // Find the correct bucket based on ETA
    while (current != NULL && maxtime(current->endTime, newFlight->ETA) < 0) {
        prev = current;
        current = current->next;
    }

    if (current != NULL && maxtime(current->startTime, newFlight->ETA) <= 0) {
        // The new flight's ETA is within the current bucket's interval

        // Find the correct position in the flightPlans linked list based on departure time
        FlightPlan* prevFlight = NULL;
        FlightPlan* currentFlight = current->flightPlans;
        while (currentFlight != NULL && timediff(currentFlight->departureTime, newFlight->departureTime) < 0) {
            prevFlight = currentFlight;
            currentFlight = currentFlight->next;
        }

        // Insert the new flight plan into the sorted list
        if (prevFlight == NULL) {
            // Insert at the beginning
            newFlight->next = current->flightPlans;
            current->flightPlans = newFlight;
        } else {
            // Insert in the middle or end
            prevFlight->next = newFlight;
            newFlight->next = currentFlight;
        }
    }

    else {
        printf("Error: Active bucket not found for the new flight plan.\n");
        // Handle error, maybe return or do something else
    }
      
   return dashboard;
}

//QUESTION 3:
//TO CANCEL A FLIGHT PLAN

void cancelFlightPlan(Bucket* dashboard, int flightID) {
    Bucket* current = dashboard;

    // Traverse the list of buckets
    while (current != NULL) {
        // Traverse the list of flight plans in this bucket
        FlightPlan* prevFlight = NULL;
        FlightPlan* currentFlight = current->flightPlans;

        while (currentFlight != NULL) {
            if (currentFlight->flightID == flightID) {
                // Found the flight plan to cancel
                if (prevFlight == NULL) {
                    // Flight plan to cancel is the first in the list
                    current->flightPlans = currentFlight->next;
                } else {
                    // Flight plan to cancel is in the middle or end
                    prevFlight->next = currentFlight->next;
                }
                
                // Free the memory of the cancelled flight plan
                free(currentFlight);
                
                printf("Flight plan with ID %d has been cancelled.\n", flightID);
                return; // Exit function after cancelling the flight plan
            }

            prevFlight = currentFlight;
            currentFlight = currentFlight->next;
        }

        current = current->next; // Move to the next bucket
    }
    printf("Flight plan with ID %d not found.\n", flightID);
}

//QUESTION 4:
//TO SHOW STATUS OF A PARTICULAR FLIGHT PLAN

void showFlightStatus(Bucket* dashboard, int flightID ,TIME currentTime) {
    Bucket* current = dashboard;

    // Traverse the list of buckets
    while (current != NULL) {
        // Traverse the list of flight plans in this bucket
        FlightPlan* currentFlight = current->flightPlans;

        while (currentFlight != NULL) {
            if (currentFlight->flightID == flightID) {
                // Found the flight plan
                printf("Flight Plan ID: %d\n", currentFlight->flightID);
                printf("Departure Time: %d:%02d\n", currentFlight->departureTime.hours, currentFlight->departureTime.minutes);
                printf("ETA: %d:%02d\n", currentFlight->ETA.hours, currentFlight->ETA.minutes);
                printf("Bucket ID: %d\n", current->bucketID);
                
                //We can additional details as needed...

                int currentTotalMinutes = currentTime.hours * 60 + currentTime.minutes;
                int departureTotalMinutes = currentFlight->departureTime.hours * 60 + currentFlight->departureTime.minutes;
                int arrivalTotalMinutes = currentFlight->ETA.hours * 60 + currentFlight->ETA.minutes;
                
                // Determine current status
                if (currentTotalMinutes < departureTotalMinutes) {
                    printf("Current Status: Flight has not departed yet.\n");
                } else if (currentTotalMinutes >= arrivalTotalMinutes) {
                    printf("Current Status: Flight has already arrived.\n");
                } else {
                    printf("Current Status: Flight is in progress.\n");
                }
                            
                return; // Exit function after displaying flight plan details
            }
            currentFlight = currentFlight->next;
        }

        current = current->next; // Move to the next bucket
    }
    printf("Flight plan with ID %d not found.\n", flightID);
}

//QUESTION 5:
// TO SHOW THE FLIGHT PLANS IN 1 HOUR TIME PEROID FROM ANY GIVEN TIME
void showFlightPlansWithinHour(Bucket* dashboard, TIME currentTime) {
    // Calculate the start time and end time for the 1-hour period
    TIME startTime;
    TIME endTime;
    startTime.hours = currentTime.hours;
    startTime.minutes = currentTime.minutes;
    endTime.hours = startTime.hours + 1;
    endTime.minutes = startTime.minutes;

    if (endTime.hours >= 24) {
        endTime.hours -= 24;
    }

    printf("Flight Plans within the next hour (from %d:%02d to %d:%02d):\n", 
           startTime.hours, startTime.minutes, endTime.hours, endTime.minutes);

    // Traverse the list of buckets
    Bucket* current = dashboard;

    while (current != NULL) {
        // Check if the bucket's time interval overlaps with the next hour
        if ((timediff(current->startTime, endTime) <= 0) && (timediff(current->endTime, startTime) >= 0)) {
            printf("Bucket ID: %d\n", current->bucketID);

            // Traverse the flight plans in this bucket
            FlightPlan* currentFlight = current->flightPlans;

            while (currentFlight != NULL) {
                // Check if the flight plan is within the next hour
                if ((timediff(currentFlight->ETA, startTime) >= 0) && (timediff(currentFlight->ETA, endTime) <= 0)) {
                    printf("Flight Plan ID: %d\n", currentFlight->flightID);
                    printf("Departure Time: %d:%02d\n", currentFlight->departureTime.hours, currentFlight->departureTime.minutes);
                    printf("ETA: %d:%02d\n", currentFlight->ETA.hours, currentFlight->ETA.minutes);
                    // Additional details as needed...
                }
                currentFlight = currentFlight->next;
            }
        }
        current = current->next; // Move to the next bucket
    }
}


void printBucketsByHourInterval(Bucket* dashboard, TIME currentTime) {
    // Define an interval of 1 hour
    TIME interval;
    interval.hours = 1;
    interval.minutes = 0;

    // Calculate the end time for the first 1-hour interval
    TIME endTime;
    endTime.hours = currentTime.hours + interval.hours;  // we initialised before because we can only same datatype values
    endTime.minutes = currentTime.minutes;

    // Handle the case where endTime crosses 24 hours
    if (endTime.hours >= 24) {
        endTime.hours -= 24;
    }

    // Loop through each 1-hour interval
    while (1) {
        printf("Bucket IDs and Flight Plans for interval %02d:%02d - %02d:%02d:\n",
               currentTime.hours, currentTime.minutes, endTime.hours, endTime.minutes);

        // Traverse the list of buckets in the original dashboard
        Bucket* current = dashboard;
        int foundFlight = 0; // Flag to track if any flights are found within this interval

        while (current != NULL) {
            // Check if the bucket's time interval falls within the current 1-hour interval
            if ((timediff(current->startTime, endTime) <= 0) && (timediff(current->endTime, currentTime) >= 0)) {
                //printf("Bucket ID: %d\n", current->bucketID);
                printf("Bucket ID: %d\n", current->bucketID);

                // Print Flight Plans for this Bucket
                FlightPlan* currentFlight = current->flightPlans;
                while (currentFlight != NULL) {
                    // Check if the flight plan's ETA falls within the 1-hour interval
                    if ((timediff(currentFlight->ETA, currentTime) >= 0) && (timediff(currentFlight->ETA, endTime) <= 0)) {
                        printf("  Flight ID: %d\n", currentFlight->flightID);
                        printf("  Departure Time: %02d:%02d\n", currentFlight->departureTime.hours, currentFlight->departureTime.minutes);
                        printf("  ETA: %02d:%02d\n", currentFlight->ETA.hours, currentFlight->ETA.minutes);
                        foundFlight = 1;
                    }
                    currentFlight = currentFlight->next;   
                }
                printf("\n"); // Add a newline between each bucket
            }
            current = current->next;
        }

        // Print message if no flights were found for this interval
        if (!foundFlight) {
            printf("No flights in this interval.\n\n");
        }

        // Move to the next 1-hour interval
        currentTime = endTime;
        endTime.hours += interval.hours;
        if (endTime.hours >= 24) {
            endTime.hours -= 24;
        }

        // Check if we have covered the entire day
        if (timediff(endTime, interval) <= 0) {
            break;  // Exit the loop if we have covered the entire day
        }
    }
}

//To print the buckets and their flight plans
void printBuckets(Bucket *dashboard) {
    if (dashboard == NULL) {
        printf("Dashboard is empty\n");
        return;
    }

    Bucket *currentBucket = dashboard;
    while (currentBucket != NULL) {
        printf("Bucket ID: %d\n", currentBucket->bucketID);
        printf("ETA Interval: %d:%02d - %d:%02d\n", currentBucket->startTime.hours, currentBucket->startTime.minutes,currentBucket->endTime.hours, currentBucket->endTime.minutes);
        printf("\n");

        FlightPlan *currentFlight = currentBucket->flightPlans;
        if (currentFlight == NULL) {
            printf("No flight plans in this bucket\n");
        } else {
            printf("Flight Plans:\n");
            while (currentFlight != NULL) {
                printf("Flight ID: %d\n", currentFlight->flightID);
                printf("Departure Time: %d:%02d\n", currentFlight->departureTime.hours, currentFlight->departureTime.minutes);
                printf("ETA: %d:%02d\n", currentFlight->ETA.hours, currentFlight->ETA.minutes);
                printf("\n");
                currentFlight = currentFlight->next;
            }
        }
        printf("\n");
        currentBucket = currentBucket->next;
    }
}


// Function to insert a bucket based on ETA interval.
// Used to print buckets in order of ETA interval.
Bucket* insertBucket(Bucket* dashboard, Bucket* newBucket) {
    if (dashboard == NULL || maxtime(newBucket->startTime, dashboard->startTime) < 0) {
        newBucket->next = dashboard;
        return newBucket;
    }

    Bucket* current = dashboard;
    while (current->next != NULL && maxtime(current->next->startTime, newBucket->startTime) < 0) {
        current = current->next;
    }

    newBucket->next = current->next;
    current->next = newBucket;
    return dashboard;
}


//MAIN FUNCTION

int main() {
   FILE *file = fopen("flights.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    Bucket* dashboard = NULL;
    char line[100];// store each line read from the file.

    // Read current time
    TIME currentTime;//Means TIME structure named currentTime
    printf("Enter current time (HH:MM): ");
    scanf("%d:%d", &currentTime.hours, &currentTime.minutes);
    getchar(); // Consume the newline character and  ensures that the next fgets function doesn't read this newline character accidentally.

    while (fgets(line, sizeof(line), file)) { //reads each line from the file using fgets.
        //fgets reads a line and stores as string 
        int bucketID;
        TIME startTime, endTime;
        // Read bucket information from file
        if (sscanf(line, "%d %d:%d %d:%d", &bucketID, &startTime.hours, &startTime.minutes,
            &endTime.hours, &endTime.minutes) != 5) {
            //sscanf:reads from string 
            fprintf(stderr, "Invalid bucket information: %s", line);
            //use of stderr:Standard error stream,it is also used to output data, specifically error messages, warnings, or other diagnostic information.
            continue;//skips the rest of the loop and moves to the next iteration if the line is not correctly formatted.
        }

        Bucket* newBucket = (Bucket*)malloc(sizeof(Bucket));
        newBucket->bucketID = bucketID;
        newBucket->startTime = startTime;
        newBucket->endTime = endTime;
        newBucket->flightPlans = NULL;
        newBucket->next=NULL;

        dashboard = insertBucket(dashboard, newBucket);

        // Read flight plans for this bucket
        fgets(line, sizeof(line), file); // Consume the newline character left after reading the bucket information
        while (fgets(line, sizeof(line), file) && line[0] != '\n') {//reads till the end of the line and stores it in line
            int flightID;
            TIME departureTime, ETA;
            if (sscanf(line, "%d %d:%d %d:%d", &flightID, &departureTime.hours, &departureTime.minutes,
                &ETA.hours, &ETA.minutes) != 5) {//reads flight information from the information stored in the line
                fprintf(stderr, "Invalid flight information: %s", line);
                continue;//skips the rest of the loop and moves to the next iteration if the line is not correctly formatted.
            }

            // Create new flight plan
            FlightPlan* newFlight = (FlightPlan*)malloc(sizeof(FlightPlan));
            newFlight->flightID = flightID;
            newFlight->departureTime = departureTime;
            newFlight->ETA = ETA;
            newFlight->next = NULL;

            dashboard = insertFlightPlan(dashboard, newFlight);
        }
    }

    fclose(file);

    while (1) {
        int choice;
        printf("\n1. Insert a new flight plan\n");
        printf("2. Cancel a flight plan by ID\n");
        printf("3. Display status of a particular flight plan by ID\n");
        printf("4. Show flight plans within the next hour and to rearrange buckets in the intervals of 1 hour from the given time\n");
        printf("5. To print the buckets and their corresponding flights\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume the newline character

        if (choice == 1) {
            // Insert a new flight plan
            FlightPlan* newFlight = (FlightPlan*)malloc(sizeof(FlightPlan));
            printf("Enter Flight Plan ID: ");
            scanf("%d", &newFlight->flightID);
            getchar(); // Consume the newline character
            printf("Enter Departure Time (HH:MM): ");
            scanf("%d:%d", &newFlight->departureTime.hours, &newFlight->departureTime.minutes);
            getchar(); // Consume the newline character
            printf("Enter ETA (HH:MM): ");
            scanf("%d:%d", &newFlight->ETA.hours, &newFlight->ETA.minutes);
            getchar(); // Consume the newline character
            // Call the function to insert the new flight plan into the dashboard
            printf("Inserting new flight plan...\n");
            dashboard = insertFlightPlan(dashboard, newFlight);

        } else if (choice == 2) {
            // Cancel a flight plan by ID
            int flightID;
            printf("Enter Flight Plan ID to cancel: ");
            scanf("%d", &flightID);
            getchar(); // Consume the newline character
            cancelFlightPlan(dashboard, flightID);

        } else if (choice == 3) {
            // Display status of a particular flight plan by ID
            int flightID;
            printf("Enter Flight Plan ID to display: ");
            scanf("%d", &flightID);
            getchar(); // Consume the newline character
            showFlightStatus(dashboard, flightID, currentTime);

        } else if (choice == 4) { 
            // Show flight plans within the next hour
            showFlightPlansWithinHour(dashboard, currentTime);
            //To rearrange buckets in the intervals of 1 hour from the given time 
            printBucketsByHourInterval(dashboard, currentTime);
            
        } else if (choice == 5) {
            //To print the buckets and their corresponding flights
            printf("Initial Dashboard:\n");
            printBuckets(dashboard);

        } else if (choice ==6){
            // Exit the program
            printf("Exiting program...\n");
            break;

        }else {
            printf("Invalid choice. Please enter a number from 1 to 5.\n");
        }
    }

    // Free allocated memory for flight plans and buckets
    Bucket* currentBucket = dashboard;
    while (currentBucket != NULL) {
        FlightPlan* currentFlightPlan = currentBucket->flightPlans;
        while (currentFlightPlan != NULL) {
            FlightPlan* tempFlightPlan = currentFlightPlan;
            currentFlightPlan = currentFlightPlan->next;
            free(tempFlightPlan);
        }
        Bucket* tempBucket = currentBucket;
        currentBucket = currentBucket->next;
        free(tempBucket);
    }
    
    return 0;
}