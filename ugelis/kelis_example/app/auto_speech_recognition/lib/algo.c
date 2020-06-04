static void swap(float *a, float *b)
{
    float temp;

    temp = *a;
    *a = *b;
    *b = temp;

    return ;
}
static void swap_index(int index[], int a, int b)
{
    float temp;

    temp = index[a];
    index[a] = index[b];
    index[b] = temp;

    return ;
}

void quicksort(float array[], int index[], int maxlen, int begin, int end)
{
    int i, j;

    if (begin < end)
    {
        i = begin + 1;
        j = end;

        while (i < j)
        {
            if (array[i] > array[begin])
            {
                swap(&array[i], &array[j]);
                swap_index(index, i, j);
                j--;
            }
            else
            {
                i++;
            }
        }

        if (array[i] >= array[begin])
        {
            i--;
        }

        swap(&array[begin], &array[i]);
        swap_index(index, begin, i);
        quicksort(array, index, maxlen, begin, i);
        quicksort(array, index, maxlen, j, end);
    }
}

