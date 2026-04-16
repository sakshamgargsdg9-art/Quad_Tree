#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
using namespace std;

struct Point
{
    int x, y;
};

struct Node
{
    Point pos;
    int data;
};

struct Quad
{
    Point topLeft, botRight;
    Node bucket[4];
    int count;
    Quad *tl, *tr, *br, *bl;
    bool divided;
};

Quad *createQuad(Point tl, Point br)
{
    Quad *q = new Quad;
    q->topLeft = tl;
    q->botRight = br;
    q->count = 0;
    q->tl = q->tr = q->bl = q->br = NULL;
    q->divided = false;
    return q;
}

bool inBoundary(Quad *q, Point p)
{
    bool x_in = p.x >= q->topLeft.x && p.x <= q->botRight.x;
    bool y_in = p.y >= q->botRight.y && p.y <= q->topLeft.y;

    return x_in && y_in;
}

void subdivide(Quad *q)
{
    int midX = (q->topLeft.x + q->botRight.x) / 2;
    int midY = (q->topLeft.y + q->botRight.y) / 2;

    cout << 3 << "\n0";
    q->tl = createQuad(q->topLeft, Point{midX, midY});
    q->tr = createQuad(Point{midX, q->topLeft.y}, Point{q->botRight.x, midY});

    q->bl = createQuad(Point{q->topLeft.x, midY}, Point{midX, q->botRight.y});

    q->br = createQuad(Point{midX, midY}, q->botRight);

    q->divided = true;
}

bool insertNode(Quad *q, Node node)
{
    if (!inBoundary(q, node.pos))
        return false;
    for (int i = 0; i < q->count; i++)
    {
        if (node.pos.x == q->bucket[i].pos.x && node.pos.y == q->bucket[i].pos.y)
        {
            q->bucket[i] = node;
            return true;
        }
    }
    if (q->count < 4)
    {
        q->bucket[q->count++] = node;
        return true;
    }

    if (q->botRight.x - q->topLeft.x <= 1 || q->topLeft.y - q->botRight.y <= 1)
    {
        return false;
    }
    if (!q->divided)
        subdivide(q);
    int midX = (q->topLeft.x + q->botRight.x) / 2;
    int midY = (q->topLeft.y + q->botRight.y) / 2;

    return insertNode(q->tl, node) ||
           insertNode(q->bl, node) ||
           insertNode(q->tr, node) ||
           insertNode(q->br, node);
}

Node *searchNode(Quad *q, Point p)
{
    if (!inBoundary(q, p))
        return NULL;

    for (int i = 0; i < q->count; i++)
    {
        if (q->bucket[i].pos.x == p.x && q->bucket[i].pos.y == p.y)
            return &q->bucket[i];
    }

    if (!q->divided)
        return NULL;

    int midX = (q->topLeft.x + q->botRight.x) / 2;
    int midY = (q->topLeft.y + q->botRight.y) / 2;

    if (p.x <= midX)
    {
        if (p.y <= midY)
            return searchNode(q->tl, p);
        else
            return searchNode(q->bl, p);
    }
    else
    {
        if (p.y <= midY)
            return searchNode(q->tr, p);
        else
            return searchNode(q->br, p);
    }
}
bool intersects(Quad *q, Point rangeTL, Point rangeBR)
{
    if (q->botRight.x < rangeTL.x || q->topLeft.x > rangeBR.x)
        return false;

    if (q->botRight.y > rangeTL.y || q->topLeft.y < rangeBR.y)
        return false;
    return true;
}

void rangeQuery(Quad *q, Point rangeTL, Point rangeBR, vector<Node> &result)
{
    if (q == NULL || !intersects(q, rangeTL, rangeBR))
        return;

    for (int i = 0; i < q->count; i++)
    {
        Node &node = q->bucket[i];

        if (node.pos.x >= rangeTL.x && node.pos.x <= rangeBR.x &&
            node.pos.y >= rangeBR.y && node.pos.y <= rangeTL.y)
        {
            result.push_back(node);
        }
    }

    if (q->divided)
    {
        rangeQuery(q->tl, rangeTL, rangeBR, result);
        rangeQuery(q->tr, rangeTL, rangeBR, result);
        rangeQuery(q->bl, rangeTL, rangeBR, result);
        rangeQuery(q->br, rangeTL, rangeBR, result);
    }
}
void pri(Quad *root)
{
    if (root == NULL)
        return;
    for (int i = 0; i < root->count; i++)
    {
        cout << root->bucket[i].pos.x << " " << root->bucket[i].pos.y << " " << root->bucket[i].data << endl;
    }
    if (root->divided)
    {
        pri(root->tl);
        pri(root->tr);
        pri(root->br);
        pri(root->bl);
    }
}
int main()
{
    int n, h, w;
    cout << "height: ";
    cin >> h;
    cout << "\nWidth: ";
    cin >> w;
    Point t1, t2;
    t1.x = 0;
    t1.y = h;
    t2.x = w;
    t2.y = 0;
    Quad *root = createQuad(t1, t2);
    cout << "\nOutput the number of points to insert: ";
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        int data, x, y;
        cout << "\nx co-ordinate: ";
        cin >> x;
        cout << "\ny co-ordinate: ";
        cin >> y;
        cout << "\nValue: ";
        cin >> data;
        cout << endl;
        Node temp;
        temp.pos.x = x;
        temp.pos.y = y;
        temp.data = data;
        insertNode(root, temp);
    }

    // traversal
    // pri(root);

    // cout << "Range query: ";
    // Point br, tl;
    // cin >> tl.x >> tl.y;
    // cin >> br.x >> br.y;

    // range query

    // vector<Node> found_nodes;
    // rangeQuery(root, tl, br, found_nodes);

    // cout << "\nNodes found in rectangle (" << tl.x << ", " << br.y << ") to (" << tl.x << ", " << tl.y << ") to (" << br.x << ", " << tl.y << ") to (" << br.x << ", " << br.y << "):" << endl;
    // for (auto &i : found_nodes)
    // {
    //     cout << i.pos.x << " " << i.pos.y << " " << i.data << endl;
    // }

    // searching
    // cout << "Node a: " << searchNode(root, Point{1, 1})->data << "\n";
    return 0;
}






This implementation features a Point Quadtree, a spatial data structure designed for efficient two-dimensional range searching and point location. By recursively partitioning a 2D space into four quadrants (Top-Left, Top-Right, Bottom-Left, and Bottom-Right), the structure optimizes spatial queries from $O(N)$ to an average of $O(\log N)$.