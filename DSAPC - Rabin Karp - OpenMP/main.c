#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <string.h>
#include "omp.h"
#include "time.h"

#define NUM_THREADS 8

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

void search(int prime)
{
	int patLength = strlen(pat);
	int txtLength = strlen(txt);
	int i, j; //i records the first index of text window 
	int patHash = 0; // hash value of pattern
	int h = 1;
	int range[NUM_THREADS][2];
	int match[10000];
	int matchIndex = 0;
	clock_t start_t, end_t, total_t;


	int splitSize = txtLength / NUM_THREADS; //split text evenly, initial quantity to process by each thread
	int extra = txtLength - (splitSize * NUM_THREADS); //remainder


	//distribute the extra by adding 1 to the front of range[][]
	//e.g. from 2 2 2 2 -> 3 3 3 2
	//to ensure distributed into equal parts
	for (int start = 0, end = splitSize, i = 0;
		start < txtLength;
		start = end, end = start + splitSize) {

		if (extra > 0) {//have extra
			end++;//add 1 more to process

			//update start & end
			range[i][0] = start;
			range[i][1] = end;

			extra--;
		}
		else {//assign remaining to have default splitSize
			range[i][0] = start;
			range[i][1] = end;
		}
		i++;
	}

	//check the range assigned to each thread
	//for (int i = 0; i < NUM_THREADS; i++) {
	//	printf("range[%i][0]: %i \n", i, range[i][0]);
	//	printf("range[%i][1]: %i \n", i, range[i][1]);
	//}

	//The value of h would be "pow(d, patLength-1)%prime"
	//h is used to remove 1st value in text window later
	for (i = 0; i < patLength - 1; i++) {
		h = (h * d) % prime; //h=88(the)
	}

	//calculate pattern hashValue
	//done outside #pragma omp parallel because pattern's hash
	//will always be the same
	for (int k = 0; k < patLength; k++) {
		patHash = (d * patHash + pat[k]) % prime;
	}
	double start_time = omp_get_wtime();
	start_t = clock();
#pragma omp parallel
	{
		int id = omp_get_thread_num();
		int txtHash = 0; // hash value of text window
		int first = range[id][0]; //first index in subtext
		int subMatchIndex = 0;
		int subMatch[1000];

		//each thread process their own range of indexes
		//range[id][0] = starting index (inclusive), range[id][1] = ending index(exclusive)
		for (int i = range[id][0]; i < range[id][1]; i++)
		{
			//first comparison
			if (i == first)
			{
				// Calculate the hash value of first window of text
				// one by one hash the character of main text
				for (int k = i; k < patLength + i; k++) {
					txtHash = (d * txtHash + txt[k]) % prime;
				}
			}
			//else if slided pattern haven't reach end of main text
			else if (i - 1 < txtLength - patLength) {
				// Calculate hash value for next window of text: Remove
				// leading digit, add trailing digit
				txtHash = (d * (txtHash - txt[i - 1] * h) + txt[i - 1 + patLength]) % prime;


				// We might get negative value of txtHash, converting it
				// to positive
				if (txtHash < 0) {
					txtHash = (txtHash + prime);
				}
			}
			

			
			// Check the hash values of current window of text and pattern. 
			if (patHash == txtHash) {
				int j;

				/* Check for characters one by one */
				for (j = 0; j < patLength; j++) {
					if (txt[i + j] != pat[j]) //increment j if both character same. Else, stop looping
						break;
				}
				

				if (j == patLength) {//if all characters same
					subMatch[subMatchIndex++] = i;
					printf("Pattern found at index %d--------- \n", i);//first index of text window where pattern found
				}
			}
		}
#pragma omp critical
		{
			//add occurence index into match[] array
			for (int i = 0; i < subMatchIndex; i++) {
				match[matchIndex++] = subMatch[i];
			}
		}
	}
	end_t = clock();
	double end_time = omp_get_wtime();

	printf("Match at index: ");
	for (int i = 0; i < matchIndex; i++) {
		printf("%d, ", match[i]);
	}
	printf("Work took %f seconds\n", end_time - start_time);//this is more precise, more precision
	printf("Work took %f seconds by time()\n", (double)(end_t - start_t) / CLOCKS_PER_SEC);
}

/* Driver program to test above function */
int main()
{
	readText();
	readPattern();
	char input;

	omp_set_num_threads(NUM_THREADS);
	//char txt[] = "there is a bee under the tree.";
	//char pat[] = "the";
	int prime = 101; // A prime number

	search(prime);

	return 0;
}

