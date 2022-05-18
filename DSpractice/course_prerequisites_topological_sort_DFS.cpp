class Solution {
public:
    vector<int> findOrder(int c, vector<vector<int>>& p) {
        vector<int> res;
        if (c == 1)
        {
            res.push_back(0);
            return res;
        }

        int s = p.size();
        if (s == 0)
        {
            for (int i = 0; i < c; ++i)
                res.push_back(i);
            return res;
        }
        if (s == 1 && c == 2)
        {
            res.push_back(p[0][1]);
            res.push_back(p[0][0]);
            cout << "\n size = " << s << endl;
            return res;
        }

        TopoSort(c, p, res);

        return res;
    }

    void TopoSort(int c, vector<vector<int>>& p, vector<int>& res)
    {
        int s = p.size();
        vector<list<int>> al(c, list<int>());
        vector<int> ed;
        vector<int> in(c, 0);
        vector<int> out(c, 0);

        for (int i = 0; i < s; ++i)
        {
            ed = p[i];
            al[ed[1]].push_back(ed[0]);
            in[ed[0]]++;
            out[ed[1]]++;
        }

        //=================
        stack<int> st;
        vector<bool> v(c, false);
        vector<bool> is(c, false);

        bool cy = false;
        for (int i = 0; i < c; ++i)
        {
            if (!v[i])
                if (!DFS(i, al, v, st, is))
                {
                    cy = true;
                    break;
                }
        }
        if (!cy)
        {

            for (int i = 0; i < c; ++i)
            {
                if ((in[i] == 0) && (out[i] == 0))
                {
                    cout << "\n found disconnected node = " << i << endl;
                    //st.push(i);
                }
            }
            cout << "\n filling res = " << st.size() << endl;
            while (!st.empty())
            {
                res.push_back(st.top());
                st.pop();
            }
        }

        return;
    }

    bool DFS(int n, vector<list<int>>& al, vector<bool>& v, stack<int>& st, vector<bool>& is)
    {

        if (!v[n])
        {
            is[n] = true;
            v[n] = true;

            auto itr = al[n].begin();
            for (; itr != al[n].end(); ++itr)
            {
                int u = *itr;
                if (!v[u] && !is[u])
                {
                    cout << "\n checking u= " << n << ", v = " << u << endl;
                    if (!DFS(u, al, v, st, is))
                        return false;
                }
                else if (is[u])
                {
                    cout << "\n found cycle u= " << n << ", v = " << u << endl;
                    return false;
                }

            }

            st.push(n);
            is[n] = false;
        }

        return true;
    }
};




















