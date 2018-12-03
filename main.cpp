#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <climits>
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
    Circle();
    Circle(double x, double y, double range, double power);
};
Circle::Circle() : x(0), y(0), range(0), power(0), distStart(0){}
Circle::Circle(double x, double y, double range, double power)
        : x(x), y(y), range(range), power(power), distStart(0){}

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
double turnRouteRisk(const vector<Point>& turnData, const Point& start,
                     const Point& end, const vector<Circle> &circleData);
void findBest(int n,int m, vector<int>& turnPoint, vector<Point>& nodes,
              Point& start, Point& end, double weight,
              vector<Circle>& circleData, double distLim, double& minCost);
double calcDistToLine(double x, double y, const Point& start, const Point& end);


int main()
{

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
    double x1, y1, x2, y2;
    cin >> x1 >> y1 >> x2 >> y2;
    Point start = Point(x1, y1);
    Point end = Point(x2, y2);

    for (int i = 0; i < m; ++i) {
        Circle tempCircle = Circle(xData[i], yData[i], rangeData[i], powerData[i]);
        tempCircle.distStart = calPointDist(tempCircle.x, tempCircle.y, start.x, start.y);
        circleData.emplace_back(tempCircle);
    }
    stable_sort(circleData.begin(), circleData.end(),[]
            (const Circle& a, const Circle& b) -> bool{return a.distStart < b.distStart;});

    vector<Point> nodes;
    findTriangles(nodes, circleData, n, start);
    for(int i = 0; i < nodes.size(); i++){
        nodes[i].distStart = calcDistToLine(nodes[i].x, nodes[i].y, start, end);
    }
    stable_sort(nodes.begin(), nodes.end(),[]
            (const Point& a, const Point& b) -> bool{return a.distStart < b.distStart;});

    vector<int> turnPoint;
    double minCost = INT_MAX;
    int index = 0;

    if(nodes.size() >= 50){
        index = 1;
    }else if(nodes.size() < 50 && nodes.size() >= 20){
        index = 2;
    }else if(nodes.size() < 20 && nodes.size() >= 10){
        index = 3;
    }else if(nodes.size() < 10){
        index = 4;
    }

    for(int i = 0; i < nodes.size(); i++){
        nodes[i].distStart = calPointDist(nodes[i].x, nodes[i].y, start.x, start.y);
    }
    for (int i = 0; i < index; ++i) {
        findBest(i + 1, (int)nodes.size(), turnPoint, nodes, start, end,
                 weight, circleData, distLim, minCost);
    }

    cout << turnPoint.size() << " ";
    for (int i = 0; i < turnPoint.size() - 1; ++i) {
        cout << nodes[turnPoint[i]].x <<" "<< nodes[turnPoint[i]].y << " ";
    }
    cout << nodes[turnPoint.back()].x <<" "<< nodes[turnPoint.back()].y;
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
            nodes.push_back(temp);
        }
        //cout << circleData[i].x << " " << circleData[i].y << endl;
        //cout << circleData[minDistCircle1].x << " " << circleData[minDistCircle1].y << endl;
        //cout << circleData[minDistCircle2].x << " " << circleData[minDistCircle2].y << endl;
        //cout << temp.x << " " << temp.y << endl;
        //cout << endl;
    }
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
                    temp.risk +=  calPointRisk(temp.x, temp.y, circleData[j].x, circleData[j].y,
                                               circleData[j].power, circleData[j].range);
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

void findBest(int n,int m, vector<int>& turnPoint,  // C m 取 n
              vector<Point>& nodes, Point& start, Point& end, double weight,
              vector<Circle>& circleData, double distLim, double& minCost)
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
            double nowCost = turnRouteRisk(nowTurnPoint, start, end, circleData)
                             + weight * nowTurnPoint.size();
//            cout << "Compare NowCost " << nowCost << " with minCost " << minCost << endl;
            if (nowCost < minCost){
                minCost = nowCost;
                turnPoint.clear();
                for (int j = 0; j < list.size(); ++j) {
                    turnPoint.push_back(list[j]);
//                    cout << list[j] << " ";
                }
//                cout << "Now minCost = " << minCost << endl;
            }
//            cout<<nowCost << endl;
        }
    }while(list[0]<(m-n));

}