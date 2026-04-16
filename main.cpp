#include<bits/stdc++.h>
using namespace std;
struct Point { int x, y; };
struct Node { Point pos; int data; };
struct BoundingBox { Point topLeft, botRight; }; 
struct Quad {
    Point topLeft, botRight;
    Node bucket[4];
    int count;
    Quad *tl, *tr, *bl, *br;
    bool divided;

    ~Quad() {
        delete tl;
        delete tr;
        delete bl;
        delete br;
    }
};


Quad* createQuad(Point tl, Point br) {
    Quad* q = new Quad;
    q->topLeft = tl;
    q->botRight = br;
    q->count = 0;
    q->tl = q->tr = q->bl = q->br = nullptr;
    q->divided = false;
    return q;
}

bool inBoundary(Quad* q, Point p) {
    bool x_in = p.x >= q->topLeft.x && p.x <= q->botRight.x;
    bool y_in = p.y >= q->botRight.y && p.y <= q->topLeft.y;
    return x_in && y_in;
}

void subdivide(Quad* q) {
    int midX = (q->topLeft.x + q->botRight.x) / 2;
    int midY = (q->topLeft.y + q->botRight.y) / 2;

    q->tl = createQuad(q->topLeft, Point{midX, midY});
    q->tr = createQuad(Point{midX, q->topLeft.y}, Point{q->botRight.x, midY});
    q->bl = createQuad(Point{q->topLeft.x, midY}, Point{midX, q->botRight.y});
    q->br = createQuad(Point{midX, midY}, q->botRight);

    q->divided = true;
}

bool insertNode(Quad* q, Node node) {
    if (!inBoundary(q, node.pos)) return false;

    for (int i = 0; i < q->count; i++) {
        if (node.pos.x == q->bucket[i].pos.x && node.pos.y == q->bucket[i].pos.y) {
            return true;
        }
    }

    if (q->count < 4 && !q->divided) {
        q->bucket[q->count++] = node;
        return true;
    }

    if (q->botRight.x - q->topLeft.x <= 1 && q->topLeft.y - q->botRight.y <= 1) {
        return false;
    }

    if (!q->divided) {
        subdivide(q);
        
        for (int i = 0; i < 4; ++i) {
            insertNode(q->tl, q->bucket[i]) || insertNode(q->tr, q->bucket[i]) ||
            insertNode(q->bl, q->bucket[i]) || insertNode(q->br, q->bucket[i]);
        }
    }

    return insertNode(q->tl, node) || insertNode(q->tr, node) ||
           insertNode(q->bl, node) || insertNode(q->br, node);
}

Node* searchNode(Quad* q, Point p) {
    if (!inBoundary(q, p)) return nullptr;

    for (int i = 0; i < q->count; i++) {
        if (q->bucket[i].pos.x == p.x && q->bucket[i].pos.y == p.y) {
            return &q->bucket[i];
        }
    }

    if (q->divided) {
        if (Node* found = searchNode(q->tl, p)) return found;
        if (Node* found = searchNode(q->tr, p)) return found;
        if (Node* found = searchNode(q->bl, p)) return found;
        if (Node* found = searchNode(q->br, p)) return found;
    }

    return nullptr;
}

bool boxOverlap(const Quad* q, const BoundingBox& range) {
    if (q->botRight.x < range.topLeft.x) return false; 
    if (q->topLeft.x > range.botRight.x) return false; 
    if (q->topLeft.y < range.botRight.y) return false; 
    if (q->botRight.y > range.topLeft.y) return false; 
    return true;
}

void queryRange(Quad* q, const BoundingBox& range, vector<Node>& results) {
    if (!boxOverlap(q, range)) return;

    for (int i = 0; i < q->count; i++) {
        Point p = q->bucket[i].pos;
        if (p.x >= range.topLeft.x && p.x <= range.botRight.x &&
            p.y >= range.botRight.y && p.y <= range.topLeft.y) {
            results.push_back(q->bucket[i]);
        }
    }

    if (q->divided) {
        queryRange(q->tl, range, results);
        queryRange(q->tr, range, results);
        queryRange(q->bl, range, results);
        queryRange(q->br, range, results);
    }
}


vector<Node> randomData(int n, int maxC = 1000) {
    vector<Node> pts;
    pts.reserve(n);
    
    for (int i = 0; i < n; i++) {
        int x = rand() % (maxC + 1);
        int y = rand() % (maxC + 1);
        pts.push_back({{x, y}, i});
    }
    return pts;
}


int main() {
    srand(time(0)); 
    
    const int ITERATIONS = 10000; 

    const Point SEARCH_POINT = {500, 500};
    const BoundingBox SEARCH_RANGE = {
    };

    vector<int> sizes;
    for(int i = 25; i <= 20000; i += 25) sizes.push_back(i);

    ofstream file("quadtree_benchmark.csv");
    file << "size,insert_ns,range_ns,search_ns\n";

    for (int n : sizes) {
        auto rnd = randomData(n);
        Quad* root = createQuad({0,1000},{1000,0}); 

        auto t1 = chrono::high_resolution_clock::now();
        for (auto &x : rnd) insertNode(root, x);
        auto t2 = chrono::high_resolution_clock::now();
        long long insert_ns = chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count();

        auto t3 = chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            vector<Node> results;
            queryRange(root, SEARCH_RANGE, results);
        }
        auto t4 = chrono::high_resolution_clock::now();
        long long range_ns = chrono::duration_cast<chrono::nanoseconds>(t4 - t3).count() / ITERATIONS;

        auto t5 = chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            searchNode(root, SEARCH_POINT);
        }
        auto t6 = chrono::high_resolution_clock::now();
        long long search_ns = chrono::duration_cast<chrono::nanoseconds>(t6 - t5).count() / ITERATIONS;

        cout << "N=" << n
             << " | Insert(ns): " << insert_ns 
             << " | Range(ns): " << range_ns 
             << " | Search(ns): " << search_ns << endl;

        file << n << "," << insert_ns << "," << range_ns << "," << search_ns << "\n";

        delete root;
    }

    file.close();
    cout << "\nCSV generated: quadtree_benchmark.csv\n";
    return 0;
}