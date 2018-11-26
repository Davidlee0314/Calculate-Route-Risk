#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

double calPointDist(double x1, double y1, double x2, double y2);
double calPointRisk(double x, double y, double riskPointX, double riskPointY, double power, double range);

class Circle
{
public:
    double x;
    double y;
    double range;
    double power;
    Circle();
    Circle(double x, double y, double range, double power);
};
Circle::Circle() : x(0), y(0), range(0), power(0) {}
Circle::Circle(double x, double y, double range, double power)
        : x(x), y(y), range(range), power(power) {}

class Point
{
    friend ostream& operator<<(ostream& out, const Point& pt);
public:
    double x;
    double y;
    double risk;
    Point();
    Point(double x, double y);
    double distToPoint(const Point& aPoint);
};
Point::Point() : x(0), y(0), risk(0){}
Point::Point(double x, double y) : x(x), y(y), risk(0){}
double Point::distToPoint(const Point& aPoint)
{
    return calPointDist(aPoint.x, aPoint.y, x, y);
}

Point findVoronoiPoint(Circle, Circle, Circle);
ostream& operator<<(ostream& out, const Point& pt)
{
    out << pt.risk;
    return out;
}

double calPointDist(double x1, double y1, double x2, double y2)
{
    double ans = pow(pow(x1 - x2, 2) + pow(y1 - y2, 2), 0.5);
    return ans;
}
double calPointRisk(double x, double y, double riskPointX, double riskPointY, double power, double range)
{
    double risk = power * ((((double)range) - calPointDist(x, y, riskPointX, riskPointY)) / (double) range);
    if (risk < 0){
        risk = 0;
    }
    return risk;
}

int main() {

//    int n = 0, m = 0, weight = 0, distLim = 0;
//    cin >> n >> m >> weight >> distLim;
//
//    vector<int> xData;
//    vector<int> yData;
//    vector<int> rangeData;
//    vector<int> powerData;
//    vector<Circle> circleData;
//    int temp = 0;
//    for (int i = 0; i < m; ++i) {
//        cin >> temp;
//        xData.push_back(temp);
//    }
//    for (int i = 0; i < m; ++i) {
//        cin >> temp;
//        yData.push_back(temp);
//    }
//    for (int i = 0; i < m; ++i) {
//        cin >> temp;
//        rangeData.push_back(temp);
//    }
//    for (int i = 0; i < m; ++i) {
//        cin >> temp;
//        powerData.push_back(temp);
//    }
//    for (int i = 0; i < m; ++i) {
//        circleData.emplace_back(Circle(xData[i], yData[i], rangeData[i], powerData[i]));
//    }
    Circle c1 = Circle(0, 0, 2, 3);
    Circle c2 = Circle(2, 2, 1, 3);
    Circle c3 = Circle(4, 0, 2, 3);
    Point ans = findVoronoiPoint(c1, c2, c3);
    cout << ans.x << " " << ans.y;
}

Point findVoronoiPoint(Circle c1, Circle c2, Circle c3){
    double temp[3][3] = {0};
    temp[0][0] = c2.x - c1.x;
    temp[0][1] = c2.y - c1.y;
    temp[1][0] = c3.x - c2.x;
    temp[1][1] = c3.y - c2.y;
    temp[2][0] = c1.x - c3.x;
    temp[2][1] = c1.y - c3.y;

    Point mid1 = Point((c2.x - c1.x) * c1.range / (c1.range + c2.range),
                       (c2.y - c1.y) * c1.range / (c1.range + c2.range));
    Point mid2 = Point((c3.x - c2.x) * c2.range / (c3.range + c2.range),
                       (c3.y - c2.y) * c2.range / (c3.range + c2.range));
    Point mid3 = Point((c1.x - c3.x) * c3.range / (c3.range + c1.range),
                       (c1.y - c3.y) * c3.range / (c3.range + c1.range));

    temp[0][2] = temp[0][0] * mid1.x + temp[0][1] * mid1.y;
    temp[1][2] = temp[1][0] * mid2.x + temp[1][1] * mid2.y;
    temp[2][2] = temp[2][0] * mid3.x + temp[2][1] * mid3.y;

    double weight = 0 - temp[1][0] / temp[0][0];
    double y = (weight * temp[0][2] + temp[1][2]) / (weight * temp[0][1] + temp[1][1]);
    double x = (temp[0][2] - temp[0][1] * y) / temp[0][0];
    Point tempP = Point(x, y);
    return tempP;
}


