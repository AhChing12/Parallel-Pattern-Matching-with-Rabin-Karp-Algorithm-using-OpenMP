/* Following program is a C implementation of Rabin Karp
Algorithm given in the CLRS book */
#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <string.h>
#include "time.h"

// d is the number of characters in the input alphabet
#define d 256

char txt[100000]; //array
char pat[10000]; //array

/* pat -> pattern
	txt -> text
	prime -> A prime number
*/

void readText() {
	FILE* f;
	int c;
	int insertIndex = 0;

	//open file
	f = fopen("text/text.txt", "r");//open for read
	if (f == NULL) { //open file failed
		perror("Error in opening file");
		return -1;
	}

	//read all character from file
	c = fgetc(f); ///get char 1 by 1
	while (!feof(f))
	{
		//printf("%c", c);
		txt[insertIndex] = c;
		insertIndex++;
		c = fgetc(f); ///get char 1 by 1
	}
	fclose(f);
}

void readPattern() {
	FILE* f;
	int c;
	int insertIndex = 0;

	//open file
	f = fopen("pattern/pattern.txt", "r");//open for read
	if (f == NULL) { //open file failed
		perror("Error in opening file");
		return -1;
	}


	//read all character from file
	c = fgetc(f); ///get char 1 by 1
	while (!feof(f))
	{
		//printf("%c", c);
		pat[insertIndex] = c;
		insertIndex++;
		c = fgetc(f); ///get char 1 by 1
	}
	fclose(f);
}

void search(char pat[], char txt[], int prime)
{
	int patLength = strlen(pat);
	int txtLength = strlen(txt);
	int i, j; //i records the first index of text window 
	int patHash = 0; // hash value for pattern
	int txtHash = 0; // hash value for txt
	int h = 1;
	clock_t start_t, end_t, total_t;

	// The value of h would be "pow(d, patLength-1)%prime"   
	for (i = 0; i < patLength - 1; i++)
		h = (h * d) % prime;

	// Calculate the hash value of pattern and first
	// window of text
	// one by one hash the character of pattern and main text
	for (i = 0; i < patLength; i++) {
		patHash = (d * patHash + pat[i]) % prime;
		txtHash = (d * txtHash + txt[i]) % prime;
	}

	start_t = clock();
	// Slide the pattern over text one by one
	for (i = 0; i <= txtLength - patLength; i++) { //i<=txtLength-patLength is number of sliding

		// Check the hash values of current window of text
		// and pattern. If the hash values match then only
		// check for characters on by one
		if (patHash == txtHash) {
			//printf("SAME HASH position:%d. ", i);
			/* Check for characters one by one */
			for (j = 0; j < patLength; j++) {
				if (txt[i + j] != pat[j]) //increment j if both character same. Else, stop looping
					break;
			}

			// if patHash == txtHash and pat[0...patLength-1] = txt[i, i+1, ...i+patLength-1]
			if (j == patLength)
				printf("Pattern found at index %d \n", i);//first index of text window where pattern found
		}

		// Calculate hash value for next window of text: Remove
		// leading digit, add trailing digit
		if (i < txtLength - patLength) { //if slided pattern haven't reach end of main text
			//printf("d:%d txtHash:%d txt[i]:%d h:%d txt[i+patLength]:%d prime:%d", d, txtHash, txt[i], h, txt[i + patLength], prime);
			txtHash = (d * (txtHash - txt[i] * h) + txt[i + patLength]) % prime;

			// We might get negative value of txtHash, converting it
			// to positive
			if (txtHash < 0)
				txtHash = (txtHash + prime);
		}
		printf("i:%d hash: %d\n", i, txtHash);
	}
	end_t = clock();
	printf("Work took %f seconds\n", (double)(end_t - start_t) / CLOCKS_PER_SEC);
}

/* Driver program to test above function */
int main()
{
	readText();
	readPattern();
	//char txt[] = "there is a bee under the tree";
	//char pat[] = "the";
	int prime = 101; // A prime number
	search(pat, txt, prime);
	return 0;
}

