#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <climits>
#include <time.h>
using namespace std;

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

class Circle
{
public:
    double x;
    double y;
    double range;
    double power;
    double distStart;
    double distToLine;
    Circle();
    Circle(double x, double y, double range, double power);
};
Circle::Circle() : x(0), y(0), range(0), power(0), distStart(0), distToLine(0){}
Circle::Circle(double x, double y, double range, double power)
        : x(x), y(y), range(range), power(power), distStart(0), distToLine(0){}

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


Point findVoronoiPoint(Circle, Circle, Circle, int);
void findTriangles(vector<Point>& nodes, vector<Circle>& circleData, int, Point&);
void twoCircleSpots(vector<Point>& nodes, const Circle& c1, const Circle& c2, int n, Point& base);
double noTurnRouteRisk(const Point& start, const Point& end, const vector<Circle>& circleData);
double calcDistToLine(double x, double y, const Point& start, const Point& end);

void findBest(int n, int m, vector<int>& turnPoint, double** nodeRisk, vector<Point>& nodes,
              Point& start, Point& end, double weight, double distLim, double& minCost
        , vector<Circle>& circleData);

int main() {
//    clock_t t;
//    t = clock();
    int n = 0, m = 0, weight = 0, distLim = 0;
    cin >> n >> m >> weight >> distLim;

    vector<int> xData;
    vector<int> yData;
    vector<int> rangeData;
    vector<int> powerData;
    vector<Circle> circleDataTemp;
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
    double x1, y1, x2, y2;
    cin >> x1 >> y1 >> x2 >> y2;
    Point start = Point(x1, y1);
    Point end = Point(x2, y2);

    for (int i = 0; i < m; ++i) {
        Circle tempCircle = Circle(xData[i], yData[i], rangeData[i], powerData[i]);
        tempCircle.distToLine = calcDistToLine(tempCircle.x, tempCircle.y, start, end);
        circleDataTemp.emplace_back(tempCircle);
    }
    stable_sort(circleDataTemp.begin(), circleDataTemp.end(),[]
            (const Circle& a, const Circle& b) -> bool{return a.distToLine < b.distToLine;});
    if(circleDataTemp.size() > 10){
        for(int i = 0; i < 10; i++){
            circleDataTemp[i].distStart =
                    calPointDist(circleDataTemp[i].x, circleDataTemp[i].y, start.x, start.y);
            circleData.push_back(circleDataTemp[i]);
        }
    }else{
        for(int i = 0; i < circleDataTemp.size(); i++){
            circleDataTemp[i].distStart =
                    calPointDist(circleDataTemp[i].x, circleDataTemp[i].y, start.x, start.y);
            circleData.push_back(circleDataTemp[i]);
        }
    }
    stable_sort(circleData.begin(), circleData.end(),[]
            (const Circle& a, const Circle& b) -> bool{return a.distStart < b.distStart;});

    vector<Point> nodes;
//    for(int i = 0; i < nodes.size(); i++){
//        cout << nodes[i].x << " " << nodes[i].y << endl;
//    }
//    cout << endl;
    findTriangles(nodes, circleData, n, start);
    stable_sort(nodes.begin(), nodes.end(),[]
            (const Point& a, const Point& b) -> bool{return a.distStart < b.distStart;});
//    for(int i = 0; i < nodes.size(); i++){
//        cout << nodes[i].x << " " << nodes[i].y << endl;
//    }
    /*
     * nodes founded
     */

    double** riskNodes = new double* [nodes.size()];
    for(int i = 0; i < nodes.size(); i++){
        double risk;
        riskNodes[i] = new double [nodes.size()];
        for(int j = 0; j <= i; j++){
            risk = noTurnRouteRisk(nodes[i], nodes[j], circleData);
            riskNodes[j][i] = risk;
            riskNodes[i][j] = risk;
            if(j == i){
                riskNodes[i][j] = 0;
            }
        }
    }
//    for(int i = 0; i < nodes.size(); i++){
//        cout << nodes[i].x << " " << nodes[i].y << " "<< nodes[i].distStart << endl;
//    }
//    for(int i = 0; i < nodes.size(); i++){
//        for(int j = 0; j < nodes.size() - 1; j ++){
//            cout << riskNodes[i][j] << " ";
//        }
//        cout << riskNodes[i][nodes.size() - 1] << endl;
//    }

    vector<int> turnPoint;
    double minCost = INT_MAX;

    for(int i = 0; i < 3; i++){
        findBest(i + 1, nodes.size(), turnPoint, riskNodes, nodes, start, end,
                 weight, distLim, minCost, circleData);
    }

    cout << turnPoint.size() << " ";
    for (int i = 0; i < turnPoint.size() - 1; ++i) {
        cout << nodes[turnPoint[i]].x <<" "<< nodes[turnPoint[i]].y << " ";
    }
    cout << nodes[turnPoint.back()].x <<" "<< nodes[turnPoint.back()].y;
//    t = clock() - t;
//    cout << (float)t/CLOCKS_PER_SEC << endl;

}

double calcDistToLine(double x, double y, const Point& start, const Point& end){
    if((x - end.x) / (y - end.y) == (x - start.x) / (y - start.y)){
        return 0;
    }else{
        double temp[2][3] = {0};
        temp[0][0] = end.y - start.y;
        temp[0][1] = start.x - end.x;
        temp[0][2] = temp[0][0] * end.x + temp[0][1] * end.y;
        temp[1][0] = end.x - start.x;
        temp[1][1] = end.y - start.y;
        temp[1][2] = temp[1][0] * x + temp[1][1] * y;

        double weight, ansX, ansY;
        if(temp[0][0] != 0){
            weight = 0 - temp[1][0] / temp[0][0];
            ansY = (weight * temp[0][2] + temp[1][2]) / (weight * temp[0][1] + temp[1][1]);
            ansX = (temp[0][2] - temp[0][1] * y) / temp[0][0];
        }else{
            weight = 0 - temp[1][1] / temp[0][1];
            ansX = (weight * temp[0][2] + temp[1][2]) / (weight * temp[0][0] + temp[1][0]);
            ansY = (temp[0][2] - temp[0][0] * x) / temp[0][1];
        }
        return calPointDist(x, y, ansX, ansY);
    }
}

double noTurnRouteRisk(const Point& start, const Point& end, const vector<Circle>& circleData){
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
            temp.risk += calPointRisk(temp.x, temp.y, circleData[i].x, circleData[i].y,
                                      circleData[i].power, circleData[i].range);
        }
        totalRisk += temp.risk;
        progress += 1;
        splits -= 1;
    }
    return totalRisk;
}

void findTriangles(vector<Point>& nodes, vector<Circle>& circleData, int n, Point& start){
    for(int i = 2; i < circleData.size(); i++){
        double minDist1 = -1, minDist2 = -1;
        int minDistCircle1 = 0, minDistCircle2 = 0;
        double dist;
        for(int j = 0; j < i; j++){
            dist = calPointDist(circleData[i].x, circleData[i].y, circleData[j].x, circleData[j].y);
            if(minDist1 == -1){
                minDist1 = dist;
                minDistCircle1 = j;
            }else if(dist < minDist1) {
                minDistCircle2 = minDistCircle1;
                minDistCircle1 = j;
                minDist2 = minDist1;
                minDist1 = dist;
            }else{
                if(dist < minDist2 || minDist2 == -1) {
                    if ((circleData[minDistCircle1].x - circleData[i].x) /
                        (circleData[minDistCircle1].y - circleData[i].y)
                        != (circleData[j].x - circleData[i].x) / (circleData[j].y - circleData[i].y)) {
                        minDistCircle2 = j;
                        minDist2 = dist;
                    }
                }
            }
        }
        Point temp;
        if(minDist1 != -1 && minDist2 != -1){
            temp = findVoronoiPoint(circleData[i],
                                    circleData[minDistCircle1], circleData[minDistCircle2], n);
            temp.distStart = temp.distToPoint(start);
            nodes.push_back(temp);
        }
//        cout << circleData[i].x << " " << circleData[i].y << endl;
//        cout << circleData[minDistCircle1].x << " " << circleData[minDistCircle1].y << endl;
//        cout << circleData[minDistCircle2].x << " " << circleData[minDistCircle2].y << endl;
//        cout << temp.x << " " << temp.y << endl;
//        cout << endl;
    }
}

Point findVoronoiPoint(Circle c1, Circle c2, Circle c3, int n){
    double temp[3][3] = {0};
    temp[0][0] = c2.x - c1.x;
    temp[0][1] = c2.y - c1.y;
    temp[1][0] = c3.x - c2.x;
    temp[1][1] = c3.y - c2.y;
    temp[2][0] = c1.x - c3.x;
    temp[2][1] = c1.y - c3.y;

    Point mid1 = Point(c1.x + (c2.x - c1.x) * c1.range / (c1.range + c2.range),
                       c1.y + (c2.y - c1.y) * c1.range / (c1.range + c2.range));
    Point mid2 = Point(c2.x + (c3.x - c2.x) * c2.range / (c3.range + c2.range),
                       c2.y + (c3.y - c2.y) * c2.range / (c3.range + c2.range));
    Point mid3 = Point(c3.x + (c1.x - c3.x) * c3.range / (c3.range + c1.range),
                       c3.y + (c1.y - c3.y) * c3.range / (c3.range + c1.range));
    temp[0][2] = temp[0][0] * mid1.x + temp[0][1] * mid1.y;
    temp[1][2] = temp[1][0] * mid2.x + temp[1][1] * mid2.y;
    temp[2][2] = temp[2][0] * mid3.x + temp[2][1] * mid3.y;

    double weight, y ,x;
    if(temp[0][0] != 0){
        weight = 0 - temp[1][0] / temp[0][0];
        y = round((weight * temp[0][2] + temp[1][2]) / (weight * temp[0][1] + temp[1][1]));
        x = round((temp[0][2] - temp[0][1] * y) / temp[0][0]);
    }else{
        weight = 0 - temp[1][1] / temp[0][1];
        x = round((weight * temp[0][2] + temp[1][2]) / (weight * temp[0][0] + temp[1][0]));
        y = round((temp[0][2] - temp[0][0] * x) / temp[0][1]);
    }
//    cout << mid1.x << " " << mid1.y << endl;
//    cout << mid2.x << " " << mid2.y << endl;
//    cout << mid3.x << " " << mid3.y << endl;
//    for(int i = 0; i < 3; i++){
//        for(int j = 0; j < 2; j++){
//            cout << temp[i][j] << " ";
//        }
//        cout << temp[i][2] << endl;
//    }
    if(y < 0){
        y = 0;
    }else if(y > n){
        y = n;
    }

    if(x < 0){
        x = 0;
    }else if(x > n){
        x = n;
    }
    Point tempP = Point(x, y);
    return tempP;
}

void findBest(int n,int m, vector<int>& turnPoint, double** nodeRisk,  vector<Point>& nodes,
              Point& start, Point& end, double weight, double distLim, double& minCost, vector<Circle>& circleData)
{
    vector<int> list;
    for(int i=0;i<n;++i) {
        list.push_back(i);
    }
    --list[n-1];


    vector<Point> nowTurnPoint;
    do{
        for(int i=n-1;i>=0;--i)
        {
            if(list[i]<m+i-n)
            {
                ++list[i];
                for(int j=i+1;j<n;++j)
                {
                    list[j]=list[i]+j-i;
                }
                break;
            }
        }
//         cout << "now route : ";
//        for (int k = 0; k < list.size(); ++k) {
//            cout <<list[k] << " ";
//        }
//         cout << endl;
//         index 0, 1, 2,......n - 1是我們要的點的index
        double dist = calPointDist(start.x, start.y, nodes[list[0]].x, nodes[list[0]].y)
                      + calPointDist(start.x, start.y, nodes[list.back()].x, nodes[list.back()].y);
        nowTurnPoint.clear();
        for (int index : list){
            if(!nowTurnPoint.empty()){
                dist += calPointDist(nowTurnPoint.back().x, nowTurnPoint.back().y,
                                     nodes[index].x, nodes[index].y);
            }
            nowTurnPoint.push_back(nodes[index]);
        }
        if(dist < distLim){
            double nowCost = 0;
            if(nowTurnPoint.size() > 1){
                for(int i = 1; i < nowTurnPoint.size(); i++){
                    nowCost += nodeRisk[list[i - 1]][list[i]];
                }
            }
            nowCost += noTurnRouteRisk(start, nowTurnPoint[0], circleData);
            nowCost += noTurnRouteRisk(end, nowTurnPoint.back(), circleData);
            nowCost += weight * turnPoint.size();
//            cout << "Compare NowCost " << nowCost << " with minCost " << minCost << endl;
            if (nowCost < minCost){
                minCost = nowCost;
                turnPoint.clear();
                for (int j = 0; j < list.size(); ++j) {
                    turnPoint.push_back(list[j]);
                }
//                cout << "Now minCost = " << minCost << endl;
            }
//            cout << endl;
        }
    }while(list[0]<(m-n));
}
void twoCircleSpots(vector<Point>& nodes, const Circle& c1, const Circle& c2, int n, Point& base){
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

        double dist1 = temp1.distToPoint(base);
        double dist2 = temp2.distToPoint(base);
        Point temp;
        if(dist1 >= dist2){
            temp = Point(temp2);
            temp.distStart = dist2;
        }else{
            temp = Point(temp1);
            temp.distStart = dist1;
        }

        if(temp.x >= 0 && temp.x <= n && temp.y >= 0 && temp.y <= n){
            nodes.push_back(temp);
        }
    }
}