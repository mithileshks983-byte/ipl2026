#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECORDS 100
#define FILENAME "employees_ascii.txt"

typedef struct {
    int id;
    char name[50];
    float salary;
} Employee;

// Function Prototypes
void saveToASCII(const char* filename, Employee empArray[], int n);
int buildOffsetIndex(const char* filename, long offsetArray[], int max_size);
void displayRecordAtPosition(const char* filename, long position);

int main() {
    // 1. Create an array of structures with sample data
    Employee employees[] = {
        {101, "Alice Smith", 75000.50},
        {102, "Bob Jones", 42000.00},
        {103, "Charlie Brown", 58500.75},
        {104, "Diana Prince", 91000.25}
    };
    int num_employees = sizeof(employees) / sizeof(employees[0]);

    // 2. Store the array of structures in an ASCII file
    saveToASCII(FILENAME, employees, num_employees);
    printf("Saved %d records to %s in ASCII format.\n\n", num_employees, FILENAME);

    // 3. Create the array of seek positions
    long record_offsets[MAX_RECORDS];
    int record_count = buildOffsetIndex(FILENAME, record_offsets, MAX_RECORDS);

    printf("--- Index of Record Positions ---\n");
    for (int i = 0; i < record_count; i++) {
        printf("Record %d starts at byte offset: %ld\n", i + 1, record_offsets[i]);
    }
    printf("---------------------------------\n\n");

    // 4. Display a specific record given its position
    // Let's say we want to fetch the 3rd record (index 2)
    int target_index = 2; 
    
    printf("Fetching Record %d directly using fseek...\n", target_index + 1);
    displayRecordAtPosition(FILENAME, record_offsets[target_index]);

    return 0;
}

// Function to store array of structures in an ASCII text file
void saveToASCII(const char* filename, Employee empArray[], int n) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error creating text file");
        exit(1);
    }

    // Write each record on a new line, using '|' as a delimiter
    for (int i = 0; i < n; i++) {
        fprintf(fp, "%d|%s|%.2f\n", empArray[i].id, empArray[i].name, empArray[i].salary);
    }

    fclose(fp);
}

// Function to build an array of seek positions (offsets)
int buildOffsetIndex(const char* filename, long offsetArray[], int max_size) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening text file for indexing");
        exit(1);
    }

    int count = 0;
    char buffer[256];

    while (count < max_size) {
        // Record the current file pointer position BEFORE reading the line
        offsetArray[count] = ftell(fp);

        // Read the line. If fgets returns NULL, we've hit the end of the file
        if (fgets(buffer, sizeof(buffer), fp) == NULL) {
            break; 
        }
        
        count++;
    }

    fclose(fp);
    return count; // Return total number of records found
}

// Function to read and display a record by jumping to its byte position
void displayRecordAtPosition(const char* filename, long position) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening text file for reading");
        return;
    }

    // Jump directly to the recorded offset
    if (fseek(fp, position, SEEK_SET) != 0) {
        printf("Error: Could not seek to position %ld\n", position);
        fclose(fp);
        return;
    }

    char buffer[256];
    // Read exactly one line from that starting position
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        Employee emp;
        
        // Parse the delimiter-separated ASCII string back into the struct
        // %[^|] tells sscanf to read a string until it hits the '|' character
        if (sscanf(buffer, "%d|%[^|]|%f", &emp.id, emp.name, &emp.salary) == 3) {
            printf("ID: %d | Name: %s | Salary: %.2f\n", emp.id, emp.name, emp.salary);
        } else {
            printf("Error parsing record data.\n");
        }
    } else {
        printf("No data found at position %ld\n", position);
    }

    fclose(fp);
}