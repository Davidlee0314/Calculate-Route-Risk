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
public:
    double x;
    double y;
    double risk;
    double distStart;
    Point();
    Point(double x, double y);
    double distToPoint(const Point& aPoint);
};
Point::Point() : x(0), y(0), risk(0), distStart(0){}
Point::Point(double x, double y) : x(x), y(y), risk(0), distStart(0){}
double Point::distToPoint(const Point& aPoint)
{
    return calPointDist(aPoint.x, aPoint.y, x, y);
}

double noTurnRouteRisk(vector<Point>& splitsData, const Point& start,
                       const Point& end, const vector<Circle>& circleData);
double turnRouteRisk(const vector<Point>& turnData, const Point& start,
                     const Point& end, const vector<Circle> &circleData);
void twoCircleSpots(vector<Point>& nodes, const Circle& c1, const Circle& c2, int n);

int main() {
    int n = 0, m = 0, weight = 0, distLim = 0;
    cin >> n >> m >> weight >> distLim;

    vector<int> xData;
    vector<int> yData;
    vector<int> rangeData;
    vector<int> powerData;
    vector<Circle> circleData;
    int temp = 0;
    for (int i = 0; i < m; ++i) {
        cin >> temp;
        xData.push_back(temp);
    }
    for (int i = 0; i < m; ++i) {
        cin >> temp;
        yData.push_back(temp);
    }
    for (int i = 0; i < m; ++i) {
        cin >> temp;
        rangeData.push_back(temp);
    }
    for (int i = 0; i < m; ++i) {
        cin >> temp;
        powerData.push_back(temp);
    }
    for (int i = 0; i < m; ++i) {
        circleData.emplace_back(Circle(xData[i], yData[i], rangeData[i], powerData[i]));
    }

    double x1, y1, x2, y2;
    cin >> x1 >> y1 >> x2 >> y2;
    Point start = Point(x1, y1);
    Point end = Point(x2, y2);

    vector<Point> nodes;
    nodes.push_back(start);
    nodes.push_back(end);
    vector<Point> routes;
    for(int i = 0; i < circleData.size(); i++){
        for(int j = i + 1; j < circleData.size(); j++){
            twoCircleSpots(nodes, circleData[i], circleData[j], n);
        }
    }
    for(int i = 0; i < nodes.size(); i++){
        nodes[i].distStart = nodes[i].distToPoint(start);
        cout << nodes[i].x << "　" << nodes[i].y << " " << nodes[i].distStart << endl;
    }
    stable_sort(nodes.begin(), nodes.end(),[]
            (const Point& a, const Point& b) -> bool{return a.distStart < b.distStart;});
};

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

void twoCircleSpots(vector<Point>& nodes, const Circle& c1, const Circle& c2, int n){
    Point temp1, temp2;
    double centerDist = calPointDist(c1.x, c1.y, c2.x, c2.y);
    if(centerDist == c1.range + c2.range){
        temp1.x = round(c1.x + (c2.x - c1.x) * c1.range / centerDist);
        temp1.y = round(c1.y + (c2.y - c1.y) * c1.range / centerDist);
        nodes.push_back(temp1);
    }else if(centerDist < c1.range + c2.range && c1.range + centerDist > c2.range
    && c2.range + centerDist > c1.range && centerDist != 0){
        double onePart = 0, high = 0;
        onePart = (pow(centerDist, 2) - pow(c2.range, 2) + pow(c1.range, 2)) / (2 * centerDist);
        high = sqrt(pow(c1.range, 2) - pow(onePart, 2));

        Point midPoint = Point(c1.x + (c2.x - c1.x) * onePart / centerDist,
                               c1.y + (c2.y - c1.y) * onePart / centerDist);
        double tempx = c1.y - c2.y;
        double tempy = c2.x - c1.x;
        double weight = high / sqrt(pow(tempx, 2) + pow(tempy, 2));
        temp1.x = round(midPoint.x + tempx * weight);
        temp1.y = round(midPoint.y + tempy * weight);
        temp2.x = round(midPoint.x - tempx * weight);
        temp2.y = round(midPoint.y - tempy * weight);
        if(temp1.x >= 0 && temp1.x <= n && temp1.y >= 0 && temp1.y <= n){
            nodes.push_back(temp1);
        }
        if(temp2.x >= 0 && temp2.x <= n && temp2.y >= 0 && temp2.y <= n){
            nodes.push_back(temp2);
        }
    }
}
double noTurnRouteRisk(vector<Point>& splitsData, const Point& start,
                       const Point& end, const vector<Circle>& circleData){
    double dist = calPointDist(start.x, start.y, end.x, end.y);
    double splits = 0, totalRisk = 0;
    int progress = 1;

    if(dist - floor(dist) == 0){
        splits = floor(dist) - 1;
    }else{
        splits = floor(dist);
    }

    while(splits > 0){
        Point temp = Point(start.x + progress * (end.x - start.x) / dist,
                           start.y + progress * (end.y - start.y) / dist);
        for(int i = 0; i < circleData.size(); i++){
            double r = calPointRisk(temp.x, temp.y,
                    circleData[i].x, circleData[i].y, circleData[i].power, circleData[i].range);
            temp.risk += r;
        }
        splitsData.emplace_back(temp);
        totalRisk += temp.risk;
        progress += 1;
        splits -= 1;
        stable_sort(splitsData.begin(), splitsData.end(),[]
                (const Point& a, const Point& b) -> bool{return a.risk < b.risk;});
    }
    return totalRisk;
}

double turnRouteRisk(const vector<Point>& turnData, const Point& start,
                     const Point& end, const vector<Circle>& circleData){
    vector<double> distData;
    double distTemp = 0;
    for(int i = 0; i <= turnData.size(); i++){
        if(i == 0){
            distTemp = calPointDist(start.x, start.y, turnData[i].x, turnData[i].y);
        }else if(i == turnData.size()){
            distTemp = calPointDist(end.x, end.y, turnData[i - 1].x, turnData[i - 1].y);
        }else{
            distTemp = calPointDist(turnData[i - 1].x, turnData[i - 1].y, turnData[i].x, turnData[i].y);
        }
        distData.push_back(distTemp);
    }

    double dist = 0, splits = 0, totalRisk = 0, remainDist = 0;
    double tempx = 0, tempy = 0;
    int progress = 1;
    bool justTurn = false;

    for(int i = 0; i < distData.size(); i++){
        dist += distData[i];
    }
    if(dist - floor(dist) == 0){
        splits = floor(dist) - 1;
    }else{
        splits = floor(dist);
    }

    while(splits > 0){
        for(int i = 0; i <= turnData.size(); i++){
            while(remainDist + progress < distData[i]){
                Point temp;
                if(justTurn){
                    if(i == turnData.size()){
                        temp = Point(turnData[i - 1].x + (end.x - turnData[i - 1].x) * remainDist / distData[i],
                                     turnData[i - 1].y + (end.y - turnData[i - 1].y) * remainDist / distData[i]);
                    }else{
                        temp = Point(turnData[i - 1].x + (turnData[i].x - turnData[i - 1].x) * remainDist / distData[i],
                                     turnData[i - 1].y + (turnData[i].y - turnData[i - 1].y) * remainDist / distData[i]);
                    }
                    tempx = temp.x;
                    tempy = temp.y;
                    justTurn = false;
                }else{
                    if(i == 0){
                        temp = Point(start.x + progress * (turnData[i].x - start.x) / distData[i],
                                     start.y + progress * (turnData[i].y - start.y) / distData[i]);
                    }else if(i == turnData.size()){
                        temp = Point(tempx + progress * (end.x - turnData[i - 1].x) / distData[i],
                                     tempy + progress * (end.y - turnData[i - 1].y) / distData[i]);
                    }else{
                        temp = Point(tempx + progress * (turnData[i].x - turnData[i - 1].x) / distData[i],
                                     tempy + progress * (turnData[i].y - turnData[i - 1].y) / distData[i]);
                    }
                    progress += 1;
                }
                for(int j = 0; j < circleData.size(); j++){
                    temp.risk += calPointRisk(temp.x, temp.y, circleData[i].x, circleData[i].y,
                            circleData[i].power, circleData[i].range);
                }
                totalRisk += temp.risk;
                splits -= 1;
            }
            remainDist = progress + remainDist - distData[i];
            progress = 1;
            justTurn = true;
        }
    }
    return totalRisk;
}