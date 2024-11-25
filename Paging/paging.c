#include <stdio.h>
#include <stdbool.h>

#define MAX_FRAMES 10
#define MAX_PAGES 50

// Function to check if a page is in memory
bool contains_page(int *memory, int size, int page) {
    // perform simple linear search
    for (int i = 0; i < size; ++i) {
        if (memory[i] == page) {
            return true;
        }
    }
    return false;
}

// Function to print memory state
void print_state(int step, int page, int *memory, int size, bool fault) {
    /// if first step, print header
    if (step <= 1) {
        printf("  Step\t|  Page\t|  Memory State\t\t%s|  Page Fault?\n"
               "------------------------------------------------------\n", size >= 5 ? "\t" : "" );
    }

    printf("  %d\t|  %d\t|  [", step, page);
    // print memory state
    for (int i = 0; i < size; ++i) {
        if (i > step - 1) {
            printf("-");
        }
        else {
            printf("%d", memory[i]);
        }
        if (i < size - 1) {
            printf(", ");
        }
    }
    printf("]\t\t%s|  %s", size <= 1 ? "\t" : size >= 8 ? "\t\t" : "", fault ? "Yes\n" : "No\n");
}

// FIFO Page Replacement Algorithm
void fifo(int *pages, int pageCount, int frameSize) {
    int pageFaults = 0;
    int list[frameSize];
    int indexToRemove = 0;

    for (int i = 0; i < pageCount; ++i) {
        bool fault = !contains_page(list, i < frameSize ? i : frameSize, pages[i]);
        if (fault) {
            // if there is still empty space inside the list
            if (i < frameSize) {
                list[i] = pages[i];
            }
            // perform FIFO operation, removing first one in
            else {
                list[indexToRemove] = pages[i];
                indexToRemove = (indexToRemove + 1) % frameSize;
            }
            ++pageFaults;
        }
        print_state(i+1, pages[i], list, frameSize, fault);
    }
    printf("\nTotal Page Faults: %d\n", pageFaults);
}

// LRU Page Replacement Algorithm
void lru(int *pages, int pageCount, int frameSize) {
    int pageFaults = 0;
    int list[frameSize];

    for (int i = 0; i < pageCount; ++i) {
        bool fault = !contains_page(list, i < frameSize ? i : frameSize, pages[i]);
        if (fault) {
            // if there is still empty space inside the list
            if (i < frameSize) {
                list[i] = pages[i];
            }
            else {
                // push all elements back an index, removing element at 0
                for (int j = 0; j < frameSize - 1; ++j) {
                    list[j] = list[j+1];
                }
                list[frameSize-1] = pages[i];
            }
            ++pageFaults;
        }
        // need to push most recently used element to end of array
        else {
            // get index of the page in the list
            int index = 0;
            while (pages[i] != list[index]) {
                ++index;
            }
            // push elements back an index, starting from the index of the requested page
            for (int j = index; j < frameSize - 1; ++j) {
                list[j] = list[j+1];
            }
            list[frameSize-1] = pages[i];
        }
        print_state(i+1, pages[i], list, frameSize, fault);
    }
    printf("\nTotal Page Faults: %d\n", pageFaults);
}

int main() {
    int pages[MAX_PAGES];
    int frameSize, pageCount = 0;

    // Input
    // frame size
    do {
        if (frameSize > MAX_FRAMES) {
            printf("Number of frames cannot be greater than %d\n", MAX_FRAMES);
        }

        printf("Enter the number of frames: ");
        scanf("%d", &frameSize);
    } while (frameSize > MAX_FRAMES);

    // pagecount
    do {
        if (pageCount > MAX_FRAMES) {
            printf("Number of pages cannot be greater than %d\n", MAX_PAGES);
        }

        printf("Enter the number of pages: ");
        scanf("%d", &pageCount);
    } while (pageCount > MAX_PAGES);

    // pages
    printf("Enter the page reference sequence:\n");
    for (int i = 0; i < pageCount; i++) {
        printf("  Page %d: ", i+1);
        scanf("%d", &pages[i]);
    }

    // Run FIFO
    printf("\nFIFO:\n");
    fifo(pages, pageCount, frameSize);

    // Run LRU
    printf("\nLRU:\n");
    lru(pages, pageCount, frameSize);
}
