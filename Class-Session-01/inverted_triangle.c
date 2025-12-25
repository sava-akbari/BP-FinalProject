#include <stdio.h>

int main() {
	int n = 0;
	printf("Enter a number: ");
	scanf_s("%d", &n);

	for (int i = n; i > 0; i--) {
		for (int j = i; j > 0; j--) {
			printf("*");
		}
		printf("\n");
	}
	return 0;
}
