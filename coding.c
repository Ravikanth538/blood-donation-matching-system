#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


// Donor Structure

typedef struct {
    char name[50];
    char bloodGroup[5];
    char city[30];
    char phone[15];
    int available; // 1 = Available, 0 = Not Available
} Donor;

#define FILE_NAME "donors.dat"


// Function Declarations

void addDonor();
void viewDonors();
void searchMatch();
int isCompatible(char *donorBG, char *patientBG);
void urgentAlert();


// Main Function

int main() {
    int choice;
    while (1) {
        printf("\n=========================================\n");
        printf("     BLOOD DONATION & MATCHING SYSTEM\n");
        printf("=========================================\n");
        printf("1. Add Donor\n");
        printf("2. View All Donors\n");
        printf("3. Search for Matching Donor\n");
        printf("4. Urgent Need Alert\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number.\n");
            while (getchar() != '\n'); // clear buffer
            continue;
        }
        getchar(); // clear newline

        switch (choice) {
            case 1: addDonor(); break;
            case 2: viewDonors(); break;
            case 3: searchMatch(); break;
            case 4: urgentAlert(); break;
            case 5: printf("Exiting... Thank you!\n"); exit(0);
            default: printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}


// Add Donor Function

void addDonor() {
    Donor d;
    FILE *fp = fopen(FILE_NAME, "ab+");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    printf("\nEnter Donor Name: ");
    fgets(d.name, sizeof(d.name), stdin);
    d.name[strcspn(d.name, "\n")] = '\0'; // remove newline

    printf("Enter Blood Group (A+, A-, B+, B-, AB+, AB-, O+, O-): ");
    scanf("%4s", d.bloodGroup);
    getchar(); // clear newline

    printf("Enter City: ");
    fgets(d.city, sizeof(d.city), stdin);
    d.city[strcspn(d.city, "\n")] = '\0';

    printf("Enter Phone Number: ");
    scanf("%14s", d.phone);

    printf("Is donor available? (1=Yes / 0=No): ");
    scanf("%d", &d.available);

    fwrite(&d, sizeof(Donor), 1, fp);
    fclose(fp);

    printf("\nDonor added successfully!\n");
}


// View Donors Function

void viewDonors() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        printf("No donor data found!\n");
        return;
    }

    Donor d;
    printf("\n%-20s %-10s %-15s %-15s %-10s\n", "Name", "BloodGrp", "City", "Phone", "Available");
    printf("---------------------------------------------------------------\n");
    while (fread(&d, sizeof(Donor), 1, fp)) {
        printf("%-20s %-10s %-15s %-15s %-10s\n",
               d.name, d.bloodGroup, d.city, d.phone, d.available ? "Yes" : "No");
    }
    fclose(fp);
}

//--------------------------
// Blood Compatibility Logic
//--------------------------
int isCompatible(char *donorBG, char *patientBG) {
    if (strcmp(patientBG, "O-") == 0) return strcmp(donorBG, "O-") == 0;
    if (strcmp(patientBG, "O+") == 0) return (strcmp(donorBG, "O+") == 0 || strcmp(donorBG, "O-") == 0);
    if (strcmp(patientBG, "A-") == 0) return (strcmp(donorBG, "A-") == 0 || strcmp(donorBG, "O-") == 0);
    if (strcmp(patientBG, "A+") == 0) return (strcmp(donorBG, "A+") == 0 || strcmp(donorBG, "A-") == 0 ||
                                              strcmp(donorBG, "O+") == 0 || strcmp(donorBG, "O-") == 0);
    if (strcmp(patientBG, "B-") == 0) return (strcmp(donorBG, "B-") == 0 || strcmp(donorBG, "O-") == 0);
    if (strcmp(patientBG, "B+") == 0) return (strcmp(donorBG, "B+") == 0 || strcmp(donorBG, "B-") == 0 ||
                                              strcmp(donorBG, "O+") == 0 || strcmp(donorBG, "O-") == 0);
    if (strcmp(patientBG, "AB-") == 0) return (strcmp(donorBG, "A-") == 0 || strcmp(donorBG, "B-") == 0 ||
                                               strcmp(donorBG, "AB-") == 0 || strcmp(donorBG, "O-") == 0);
    if (strcmp(patientBG, "AB+") == 0) return 1; // universal recipient
    return 0;
}

//--------------------------
// Search Matching Donor
//--------------------------
void searchMatch() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        printf("No donor data found!\n");
        return;
    }

    char patientBG[5], patientCity[30];
    int found = 0;
    printf("\nEnter Patient Blood Group: ");
    scanf("%4s", patientBG);
    getchar();

    printf("Enter Patient City: ");
    fgets(patientCity, sizeof(patientCity), stdin);
    patientCity[strcspn(patientCity, "\n")] = '\0';

    Donor d;
    printf("\nSearching for compatible donors...\n");

    while (fread(&d, sizeof(Donor), 1, fp)) {
        if (d.available && isCompatible(d.bloodGroup, patientBG) &&
            strcasecmp(d.city, patientCity) == 0) {
            printf("\nMatch Found!\n");
            printf("Name: %s\nBlood Group: %s\nCity: %s\nPhone: %s\n",
                   d.name, d.bloodGroup, d.city, d.phone);
            found = 1;
        }
    }

    if (!found)
        printf("\nNo matching donors found in %s.\n", patientCity);

    fclose(fp);
}

//--------------------------
// Urgent Need Alert
//--------------------------
void urgentAlert() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        printf("No donor records found!\n");
        return;
    }

    printf("\nUrgent Alert! Listing all available donors:\n");
    Donor d;
    int count = 0;
    while (fread(&d, sizeof(Donor), 1, fp)) {
        if (d.available) {
            printf("Name: %-15s | Blood: %-4s | City: %-10s | Phone: %-10s\n",
                   d.name, d.bloodGroup, d.city, d.phone);
            count++;
        }
    }

    if (count == 0)
        printf("\nNo active donors available at this moment.\n");

    fclose(fp);
}
