#include <stdio.h>

int main() {
    int max_len = 0, current_len = 1, prev = 0, num = 0;

    // Ask the user to enter the first number
    printf("Enter an number: ");
    scanf_s("%d", &num);  // Read the first number from user
    prev = num;

    while (num != -1) {

        if (num > prev) {
            current_len++;

            if (current_len > max_len)
                max_len = current_len;
        }
        else {
            //the increasing sequence has broken
            if (current_len > max_len)
                max_len = current_len;

            current_len = 1;
        }

        prev = num;

        printf("Enter an number (Enter -1 to EXIT): ");
        scanf_s("%d", &num);
    }

    // After the loop ends (user entered -1), print the result
    printf("The length of the longest strictly increasing subsequence is: %d\n", max_len);

    return 0;  // End the program successfully
}
