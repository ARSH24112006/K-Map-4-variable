#include <bits/stdc++.h>
using namespace std;

struct Term {
    string bits;
    vector<int> cover;
};

string toBin(int x) {
    string s = "0000";
    for (int i = 3; i >= 0; i--) {
        s[i] = '0' + (x % 2);
        x /= 2;
    }
    return s;
}

bool inCover(Term t, int m) {
    for (int i = 0; i < (int)t.cover.size(); i++)
        if (t.cover[i] == m) return true;
    return false;
}

bool merge(Term a, Term b, Term &result) {
    int diff = 0, pos = -1;
    for (int i = 0; i < 4; i++) {
        if (a.bits[i] != b.bits[i]) {
            if (a.bits[i] == '-' || b.bits[i] == '-') return false;
            diff++;
            pos = i;
        }
    }
    if (diff != 1) return false;
    result.bits = a.bits;
    result.bits[pos] = '-';
    result.cover = a.cover;
    for (int i = 0; i < (int)b.cover.size(); i++) result.cover.push_back(b.cover[i]);
    return true;
}

vector<Term> getPrimes(vector<int> mins) {
    vector<Term> cur;
    for (int i = 0; i < (int)mins.size(); i++) {
        Term t;
        t.bits = toBin(mins[i]);
        t.cover.push_back(mins[i]);
        cur.push_back(t);
    }

    vector<Term> primes;
    while (cur.size() > 0) {
        vector<bool> used(cur.size(), false);
        vector<Term> next;

        for (int i = 0; i < (int)cur.size(); i++) {
            for (int j = i + 1; j < (int)cur.size(); j++) {
                Term t;
                if (merge(cur[i], cur[j], t)) {
                    used[i] = true;
                    used[j] = true;

                    bool dup = false;
                    for (int k = 0; k < (int)next.size(); k++) {
                        if (next[k].bits == t.bits) {
                            for (int m = 0; m < (int)t.cover.size(); m++)
                                if (!inCover(next[k], t.cover[m]))
                                    next[k].cover.push_back(t.cover[m]);
                            dup = true;
                        }
                    }
                    if (!dup) next.push_back(t);
                }
            }
        }

        for (int i = 0; i < (int)cur.size(); i++)
            if (!used[i]) primes.push_back(cur[i]);

        cur = next;
    }

    vector<Term> result;
    for (int i = 0; i < (int)primes.size(); i++) {
        bool dup = false;
        for (int j = 0; j < (int)result.size(); j++)
            if (result[j].bits == primes[i].bits) dup = true;
        if (!dup) result.push_back(primes[i]);
    }
    return result;
}

string toExpr(string bits) {
    string vars = "abcd";
    string s = "";
    for (int i = 0; i < 4; i++) {
        if (bits[i] == '1') s += vars[i];
        else if (bits[i] == '0') { s += vars[i]; s += '\''; }
    }
    if (s == "") s = "1";
    return s;
}

vector<Term> pool;
vector<vector<int> > results;

void search(vector<int> remain, vector<int> chosen) {
    if (remain.size() == 0) {
        results.push_back(chosen);
        return;
    }
    int m = remain[0];
    for (int i = 0; i < (int)pool.size(); i++) {
        if (!inCover(pool[i], m)) continue;

        vector<int> newRemain;
        for (int j = 0; j < (int)remain.size(); j++)
            if (!inCover(pool[i], remain[j])) newRemain.push_back(remain[j]);

        chosen.push_back(i);
        search(newRemain, chosen);
        chosen.pop_back();
    }
}

int main() {
    int order[4] = {0, 1, 3, 2};
    vector<int> mins;

    cout << "Enter the 4x4 k-map (16 values, 0 or 1):" << endl;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            int v;
            cin >> v;
            if (v == 1) mins.push_back(order[c] * 4 + order[r]);
        }
    }

    vector<Term> primes = getPrimes(mins);

    vector<int> coverCount(mins.size(), 0);
    vector<int> coveringPrime(mins.size(), -1);
    for (int i = 0; i < (int)mins.size(); i++) {
        for (int j = 0; j < (int)primes.size(); j++) {
            if (inCover(primes[j], mins[i])) {
                coverCount[i]++;
                coveringPrime[i] = j;
            }
        }
    }

    vector<int> essential;
    for (int i = 0; i < (int)mins.size(); i++) {
        if (coverCount[i] == 1) {
            int p = coveringPrime[i];
            bool already = false;
            for (int j = 0; j < (int)essential.size(); j++)
                if (essential[j] == p) already = true;
            if (!already) essential.push_back(p);
        }
    }

    vector<int> remain;
    for (int i = 0; i < (int)mins.size(); i++) {
        bool done = false;
        for (int j = 0; j < (int)essential.size(); j++)
            if (inCover(primes[essential[j]], mins[i])) done = true;
        if (!done) remain.push_back(mins[i]);
    }

    vector<int> poolIndex;
    for (int i = 0; i < (int)primes.size(); i++) {
        bool isEssential = false;
        for (int j = 0; j < (int)essential.size(); j++)
            if (essential[j] == i) isEssential = true;
        if (!isEssential) {
            pool.push_back(primes[i]);
            poolIndex.push_back(i);
        }
    }

    if (remain.size() > 0) {
        vector<int> chosen;
        search(remain, chosen);
    } else {
        vector<int> empty;
        results.push_back(empty);
    }

    int minSize = 1000;
    for (int i = 0; i < (int)results.size(); i++)
        if ((int)results[i].size() < minSize) minSize = results[i].size();

    vector<string> answers;
    for (int i = 0; i < (int)results.size(); i++) {
        if ((int)results[i].size() != minSize) continue;

        vector<int> all = essential;
        for (int j = 0; j < (int)results[i].size(); j++) {
            int p = poolIndex[results[i][j]];
            bool already = false;
            for (int k = 0; k < (int)all.size(); k++)
                if (all[k] == p) already = true;
            if (!already) all.push_back(p);
        }

        vector<string> terms;
        for (int j = 0; j < (int)all.size(); j++)
            terms.push_back(toExpr(primes[all[j]].bits));
        sort(terms.begin(), terms.end());

        string expr = "";
        for (int j = 0; j < (int)terms.size(); j++) {
            if (j > 0) expr += " + ";
            expr += terms[j];
        }

        bool already = false;
        for (int j = 0; j < (int)answers.size(); j++)
            if (answers[j] == expr) already = true;
        if (!already) answers.push_back(expr);
    }

    for (int i = 0; i < (int)answers.size(); i++)
        cout << answers[i] << endl;
}