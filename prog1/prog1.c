#include <stdio.h>
#include <stdlib.h>

// Define the structure for our record
typedef struct {
    int id;
    char name[50];
    float salary;
} Employee;

// Function Prototypes
void createRecords(const char* filename, int n);
void displayMthRecord(const char* filename, int m);
void deleteRecord(const char* filename, int target_id);
void displayAll(const char* filename);

int main() {
    const char* filename = "employees.dat";
    int n, m, delete_id;

    // 1. Store n records
    printf("How many records do you want to create? ");
    scanf("%d", &n);
    createRecords(filename, n);

    printf("\n--- All Records --- \n");
    displayAll(filename);

    // 2. Get the m-th record
    printf("\nEnter the record number (m) you want to view: ");
    scanf("%d", &m);
    displayMthRecord(filename, m);

    // 3. Delete a record
    printf("\nEnter the ID of the employee you want to delete: ");
    scanf("%d", &delete_id);
    deleteRecord(filename, delete_id);

    printf("\n--- Records after deletion --- \n");
    displayAll(filename);

    return 0;
}

// Function to store n records in the binary file
void createRecords(const char* filename, int n) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        perror("Error opening file for writing");
        return;
    }

    Employee emp;
    for (int i = 0; i < n; i++) {
        printf("\nEnter details for Employee %d:\n", i + 1);
        printf("ID: ");
        scanf("%d", &emp.id);
        printf("Name: ");
        scanf(" %[^\n]s", emp.name); // Read string with spaces
        printf("Salary: ");
        scanf("%f", &emp.salary);

        fwrite(&emp, sizeof(Employee), 1, fp);
    }

    fclose(fp);
    printf("\n%d records successfully saved to %s.\n", n, filename);
}

// Function to fetch and display the m-th record using fseek
void displayMthRecord(const char* filename, int m) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error opening file for reading");
        return;
    }

    Employee emp;
    
    // Calculate the exact byte offset: (m - 1) * size of one record
    // We use SEEK_SET to start calculating from the very beginning of the file.
    long offset = (m - 1) * sizeof(Employee);
    
    // Move the file pointer to the calculated offset
    if (fseek(fp, offset, SEEK_SET) != 0) {
        printf("Error: Could not locate record %d.\n", m);
        fclose(fp);
        return;
    }

    // Attempt to read the record
    if (fread(&emp, sizeof(Employee), 1, fp) == 1) {
        printf("\n--- Record %d Details ---\n", m);
        printf("ID: %d | Name: %s | Salary: %.2f\n", emp.id, emp.name, emp.salary);
    } else {
        printf("\nError: Record %d does not exist (file too short).\n", m);
    }

    fclose(fp);
}

// Function to delete a record by ID using the Temp File strategy
void deleteRecord(const char* filename, int target_id) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        perror("Error opening file for reading");
        return;
    }

    // Create a temporary file to copy over the data we want to keep
    const char* temp_filename = "temp.dat";
    FILE *temp = fopen(temp_filename, "wb");
    if (temp == NULL) {
        perror("Error creating temporary file");
        fclose(fp);
        return;
    }

    Employee emp;
    int found = 0;

    // Read the original file sequentially
    while (fread(&emp, sizeof(Employee), 1, fp) == 1) {
        // If it's NOT the record we want to delete, write it to the temp file
        if (emp.id != target_id) {
            fwrite(&emp, sizeof(Employee), 1, temp);
        } else {
            found = 1; // Mark that we found and "skipped" the target record
        }
    }

    // Close both files to release OS locks
    fclose(fp);
    fclose(temp);

    // If the record was found, replace the old file with the new temp file
    if (found) {
        remove(filename);              // Delete original file
        rename(temp_filename, filename); // Rename temp to original
        printf("Record with ID %d was successfully deleted.\n", target_id);
    } else {
        remove(temp_filename);         // Just discard the temp file
        printf("Record with ID %d not found.\n", target_id);
    }
}

// Helper function to print the whole file so we can see the changes
void displayAll(const char* filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("No records found.\n");
        return;
    }

    Employee emp;
    int count = 1;
    while (fread(&emp, sizeof(Employee), 1, fp) == 1) {
        printf("%d. ID: %d | Name: %s | Salary: %.2f\n", count++, emp.id, emp.name, emp.salary);
    }
    fclose(fp);
}