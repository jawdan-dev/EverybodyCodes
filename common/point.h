typedef struct {
	int x, y;
} Point;

const bool Point_equal(const Point* p1, const Point* p2) {
	return p1->x == p2->x && p1->y == p2->y;
}
const Point Point_add(const Point* p1, const Point* p2) {
	return (Point) {
		.x = p1->x + p2->x,
			.y = p1->y + p2->y,
	};
}
const int Point_distance(const Point* p1, const Point* p2) {
	int cx = p1->x - p2->x;
	int cy = p1->y - p2->y;
	return abs(cx) + abs(cy);
}