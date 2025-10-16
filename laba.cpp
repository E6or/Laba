#include <iostream>
#include <random>
#include <chrono>
using namespace std;

struct subvector {
    int *mas;
    unsigned int top;
    unsigned int capacity;
};

bool init(subvector *qv) {
    if (qv == nullptr) return false;
    qv->mas = nullptr;
    qv->top = 0;
    qv->capacity = 0;
    return true;
}

bool resize(subvector *qv, unsigned int new_capacity) {
    if (qv == nullptr) return false;

    int *tmp = nullptr;

    if (new_capacity > 0)
        tmp = new int[new_capacity];
    else {
        delete[] qv->mas;
        qv->mas = nullptr;
        qv->capacity = 0;
        qv->top = 0;
        return true;
    }

    if (qv->mas != nullptr) {
        unsigned int lim = qv->top;
        if (lim > new_capacity)
            lim = new_capacity;

        for (unsigned int i = 0; i < lim; i++)
            tmp[i] = qv->mas[i];

        delete[] qv->mas;
        qv->top = lim;
    } else {
        qv->top = 0;
    }

    qv->mas = tmp;
    qv->capacity = new_capacity;
    return true;
}

bool push_back(subvector *qv, int val) {
    if (qv == nullptr) return false;
    if (qv->capacity == 0)
        resize(qv, 1);
    else if (qv->top >= qv->capacity)
        resize(qv, qv->capacity * 2);

    qv->mas[qv->top] = val;
    qv->top++;

    return true;
}

int pop_back(subvector *qv) {
    if (qv == nullptr) return 0;
    if (qv->top == 0) return 0;

    qv->top--;
    return qv->mas[qv->top];
}

void shrink_to_fit(subvector *qv) {
    if (qv == nullptr) return;
    resize(qv, qv->top);
}

void clear(subvector *qv) {
    if (qv == nullptr) return;
    qv->top = 0;
}

void destructor(subvector *qv) {
    if (qv == nullptr) return;
    if (qv->mas != nullptr)
        delete[] qv->mas;

    qv->mas = nullptr;
    qv->top = 0;
    qv->capacity = 0;
}


using std::cout;
using std::endl;
double get_time()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count()/1e6;
}
int rand_uns(int min, int max)
{
        unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
        static std::default_random_engine e(seed);
        std::uniform_int_distribution<int> d(min, max);
        return d(e);
}
int main()
{
    int n = 100000;
    int *test_sequence = new int[n], sum_for_O3 = 0, sum_check = 0,
        *pop_push_sequence_eq = new int[n],
        *pop_push_sequence_push = new int[n],
        *pop_push_sequence_pushpush = new int[n];
    double start = 0, finish = 0, total = 0;
    cout << std::fixed;
    cout.precision(4);
//----------- Initialization
    start = get_time();
    for (int i = 0; i < n; i++)
    {
        test_sequence[i] = rand_uns(0, n - 1);
        pop_push_sequence_eq[i] = rand_uns(0, 1);
        pop_push_sequence_push[i] = rand_uns(0, 5);
        pop_push_sequence_pushpush[i] = rand_uns(0, 10);
    }
    finish = get_time();
    cout << "Test sequence initialization: \t\t" << finish - start << endl;
    subvector sv;
    init(&sv);
//----------- Test 000 Straight push_back
    start = get_time();
    for (int i = 0; i < n; i++)
    {
        push_back(&sv, test_sequence[i]);
    }
    finish = get_time();
    for (int i = 0; i < n; i++)   //!!! This is a hack to bamboozle the O3 optimization.
        sum_for_O3 += sv.mas[i];  // I might as well use it to test push/pop.
    cout << "000 Straight push_back: \t\t" << finish - start << endl;
    total += finish - start;
//----------- Test 001 Straight pop_back
    start = get_time();
    for (int i = 0; i < n; i++)
    {
        sum_check += pop_back(&sv);
    }
    finish = get_time();
    if (sum_check != sum_for_O3)
    {
        cout <<endl <<"--- !!! Failed push/pop consistency !!! ---" << endl;
        return 0;
    }
    cout << "001 Straight pop_back: \t\t\t" << finish - start << endl;
    total += finish - start;
//----------- Test 002 Straight resize up
    start = get_time();
    for (int i = 0; i < n; i++)
    {
        resize(&sv, i);
    }
    finish = get_time();
    shrink_to_fit(&sv);
    if (sv.capacity)
    {
        cout <<endl <<"--- !!! Failed resize/shrink consistency !!! ---" << endl;
        return 0;
    }
    cout << "002 Straight resize up: \t\t" << finish - start << endl;
    total += finish - start;
//----------- Test 003 Random pop/push equal amount
    sum_for_O3 = 0; // This variable will be printed so O3 won't cut the whole thing.
    start = get_time();
    for (int i = 0; i < n; i++)
    {
        if (pop_push_sequence_eq[i])
            push_back(&sv, test_sequence[i]);
        else
            sum_for_O3 += pop_back(&sv);
    }
    finish = get_time();
    clear(&sv);
    shrink_to_fit(&sv);
    if (sv.top)
    {
        cout <<endl <<"--- !!! Falied clear !!! ---" << endl;
        return 0;
    }
    if (sv.capacity)
    {
        cout <<endl <<"--- !!! Falied shrink_to_fit !!! ---" << endl;
        return 0;
    }
    cout << "003 Random pop/push equal amount: \t" << finish - start << "\t\t" << sum_for_O3 << endl;
    total += finish - start;
//----------- Test 004 Random pop/push more push
    sum_for_O3 = 0; // This variable will be printed so O3 won't cut the whole thing.
    start = get_time();
    for (int i = 0; i < n; i++)
    {
        if (pop_push_sequence_push[i])
            push_back(&sv, test_sequence[i]);
        else
            sum_for_O3 += pop_back(&sv);
    }
    finish = get_time();
    clear(&sv);
    shrink_to_fit(&sv);
    cout << "004 Random pop/push more push: \t\t" << finish - start << "\t\t" << sum_for_O3 << endl;
    total += finish - start;
//----------- Test 005 Random pop/push much more push
    sum_for_O3 = 0; // This variable will be printed so O3 won't cut the whole thing.
    start = get_time();
    for (int i = 0; i < n; i++)
    {
        if (pop_push_sequence_pushpush[i])
            push_back(&sv, test_sequence[i]);
        else
            sum_for_O3 += pop_back(&sv);
    }
    finish = get_time();
    cout << "005 Random pop/push much more push: \t" << finish - start << "\t\t" << sum_for_O3 << endl;
    total += finish - start;
//----------- End of tests
    destructor(&sv);
    cout << "-----------" << endl <<"Alltests finished, total time: \t" << total << endl;
    delete[] test_sequence;
    delete[] pop_push_sequence_eq;
    delete[] pop_push_sequence_push;
    delete[] pop_push_sequence_pushpush;
    return 0;
}