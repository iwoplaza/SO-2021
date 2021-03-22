#include <stdio.h>
#include <storeutils.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

int count_even(SULineNode_t* root)
{
    int even_count = 0;
    SULineNode_t* curr = root;

    while (curr != NULL)
    {
        int num = atoi(curr->content);

        if (num % 2 == 0)
        {
            even_count++;
        }

        curr = curr->next;
    }

    return even_count;
}

SULineNode_t* filter(SULineNode_t* root, bool (*predicate)(int num))
{
    // Finding the root
    while (root != NULL)
    {
        int num = atoi(root->content);

        if (predicate(num))
        {
            break;
        }

        root = root->next;
    }

    if (root == NULL)
        return NULL;

    SULineNode_t* curr = root->next;
    root = su_create_line_node(root->content);
    SULineNode_t* head = root;

    while (curr != NULL)
    {
        int num = atoi(curr->content);

        if (predicate(num))
        {
            SULineNode_t* new_curr = su_create_line_node(curr->content);
            head->next = new_curr;
            head = new_curr;
        }

        curr = curr->next;
    }

    return root;
}

bool is_snd_digit_0_or_7(int number)
{
    int digit = (number / 10) % 10;
    return digit == 0 || digit == 7;
}

bool is_sqrt_of_integer(int number)
{
    // Base cases
    if (number == 0 || number == 1)
        return true;

    // Doing a binary search for floor(sqrt(x))
    int start = 1, end = number, ans;
    while (start <= end)
    {
        int mid = (start + end) / 2;

        // If number is a perfect square
        if (mid * mid == number)
            return mid;

        // Since we need floor, we update answer when mid*mid is
        // smaller than x, and move closer to sqrt(x)
        if (mid * mid < number)
        {
            start = mid + 1;
            ans = mid;
        }
        else // If mid*mid is greater than x
            end = mid-1;
    }

    return ans * ans == number;
}

int main()
{
    clock_t start = clock();

    char buffer[256];
    SULineNode_t* root = su_read_file("dane.txt");

    sprintf(buffer, "Liczb parzystych jest %d", count_even(root));
    SULineNode_t* a_node = su_create_line_node(buffer);
    su_write_to_file(a_node, "a.txt");
    su_free_file(a_node);

    SULineNode_t* filtered_node_1 = filter(root, is_snd_digit_0_or_7);
    su_write_to_file(filtered_node_1, "b.txt");
    su_free_file(filtered_node_1);

    SULineNode_t* filtered_node_2 = filter(root, is_sqrt_of_integer);
    su_write_to_file(filtered_node_2, "c.txt");
    su_free_file(filtered_node_2);

    // Measuring the time
    clock_t delta = clock() - start;
    printf("time: %f ms", (double) delta / CLOCKS_PER_SEC * 1000);

    su_free_file(root);
    return 0;
}
