This project is a simple implementation of a QuadTree, a data structure that helps organize points in a 2D space in a smarter and more efficient way. Instead of checking every point one by one (which takes O(n) time), the space is recursively divided into four smaller regions, allowing operations to be performed much faster.

Whenever a region gets too crowded, it splits into four equal quadrants. This keeps the data well-structured and reduces unnecessary checks during queries.

From a mathematical perspective, this hierarchical division reduces the height of the structure to roughly O(log n) in average cases. As a result:

Insertion: O(log n) (average case)
Range Query: O(log n + k), where k is the number of points found

This makes the QuadTree especially useful for large datasets where brute-force approaches become too slow.

Overall, this project demonstrates how combining simple recursion with spatial partitioning can significantly improve performance in problems involving 2D data, such as games, maps, and geometric queries.
