#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

//declaring a global variable that is used in the lock_pairs() function
int start_index; 

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool check_cycle(int index, int count);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    for (int k = 0; k < candidate_count; k++)
    {
        if (strcmp(name, candidates[k]) == 0)
        {
            ranks[rank] = k;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    int temp1;
    int temp2;
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 1; j < candidate_count; j++)
        {
            if (i < j)
            {
                temp1 = ranks[i];
                temp2 = ranks[j];
                preferences[temp1][temp2] += 1;
            }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    int index = 0;
    pair_count = 0;
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 1; j < candidate_count; j++)
        {
            if ((preferences[i][j] != 0) && (preferences[i][j] > preferences[j][i]))
            {
                pairs[index].winner = i;
                pairs[index].loser = j;
                index += 1;
                pair_count += 1;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int max_index = 0;
    pair temp;
    for (int i = 0; i < pair_count; i++)
    {
        for (int j = i; j < pair_count; j++)
        {
            if ((preferences[pairs[j].winner][pairs[j].loser]) >
                (preferences[pairs[max_index].winner][pairs[max_index].loser]))
            {
                max_index = j;
            }
        }
        temp = pairs[i];
        pairs[i] = pairs[max_index];
        pairs[max_index] = temp;
        max_index = i + 1;
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    for (int i = 0; i < pair_count; i++)
    {
        locked[pairs[i].winner][pairs[i].loser] = true;
        start_index = pairs[i].winner;
        bool cycle = check_cycle(i, 1);
        if (cycle == true)
        {
            locked[pairs[i].winner][pairs[i].loser] = false;
        }
    }

    return;
}

// Print the winner of the election
void print_winner(void)
{
    string winners[candidate_count];
    for (int i = 0; i < candidate_count; i++)
    {
        winners[i] = candidates[i];
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == true)
            {
                winners[i] = "0";
            }
        }
    }

    for (int k = 0; k < candidate_count; k++)
    {
        if (strcmp("0", winners[k]) != 0)
        {
            printf("%s\n", winners[k]);
        }
    }

    return;
}

//checks if locking a vote would cause a cycle using recursion
bool check_cycle(int index, int count)
{
    bool cycle = false;
    int indexes[pair_count];
    int track = 0;
    for (int i = 0; i <= pair_count; i++)
    {
        if (pairs[index].loser == pairs[i].winner)
        {
            indexes[track] = i;
            track += 1;
        }
    }
    for (int j = 0; j <= track; j++)
    {
        if ((indexes[j] == -1) ||
            (locked[pairs[indexes[j]].winner][pairs[indexes[j]].loser] == false) ||
            (count >= pair_count))
        {
            return false;
        }

        if (pairs[indexes[j]].loser == start_index)
        {
            return true;
        }
        else
        {
            count += 1;
            cycle = check_cycle(indexes[j], count);
            if (cycle == true)
            {
                return cycle;
            }
        }
    }
    return cycle;
}
