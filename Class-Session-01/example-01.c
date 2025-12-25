//finding average and second largest number

#include <stdio.h>
#include <math.h>

int main() {
	int num = 0, count = 0;
	float sum = 0;
	float avg = 0; 
	double largest = -INFINITY;
	double second_largest = -INFINITY;

	while (1) {
		printf("Enter a number (Enter 0 to QUIT): ");
		scanf_s("%d", &num);
		
		if (num == 0) {
			break;
		}
		
		sum = sum + num;
		count++;

		if (num > largest) {
			second_largest = largest;
			largest = num;
		}

		//اگر عددم از دومین عدد بزرگتر، بزرگتر بود ولی از بزرگترین کوچکتر باشه؛ یعنی یه دومین بزرگتر جدید دارم
		else if (num > second_largest && num < largest) {
			second_largest = num;
		}
	}

	avg = sum / count;

	printf("Avrage of numbers is: %.2f\n", avg);

	if (count < 2 || second_largest == -INFINITY) {
		printf("Fewer than two valid numbers entered or second largest does not exist.\n");
	}
	else {
		printf("Second largest number is: %.0lf\n", second_largest);
	}

	return 0;
}
