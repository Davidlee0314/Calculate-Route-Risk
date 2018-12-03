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
    Circle close;
    Point();
    Point(double x, double y);
    double distToPoint(const Point& aPoint);
};
Point::Point() : x(0), y(0), risk(0), close(Circle(0,0,0,0)){}
Point::Point(double x, double y) : x(x), y(y), risk(0), close(Circle(0,0,0,0)){}
double Point::distToPoint(const Point& aPoint)
{
    return calPointDist(aPoint.x, aPoint.y, x, y);
}

double noTurnRouteRisk(vector<Point>& splitsData, const Point& start,
                       const Point& end, const vector<Circle>& circleData);
double turnRouteRisk(const vector<Point>& turnData, const Point& start,
                     const Point& end, const vector<Circle> &circleData);
void dodgeRisk(vector<Point>& turnsData, vector<Point>& splitsData, const Point& start,
               const Point& end, const vector<Circle>& circleData, int weight, double lineRisk);

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
    vector<Point> splitsData;
    vector<Point> turnsData;
    double lineRisk = noTurnRouteRisk(splitsData, start, end, circleData);
//    cout << lineRisk << endl;
//    for(int i = 0; i < splitsData.size(); i++){
//        cout << splitsData[i].x << " " << splitsData[i].y << " -- " << splitsData[i].close.x <<" " << splitsData[i].close.y << endl;
//    }
    dodgeRisk(turnsData, splitsData, start, end, circleData, weight, lineRisk);
    cout << turnsData.size();
    for(int i = 0; i < turnsData.size(); i++){
        cout << " " << round(turnsData[i].x) << " " << round(turnsData[i].y);
    }
}

void dodgeRisk(vector<Point>& turnsData, vector<Point>& splitsData, const Point& start,
               const Point& end, const vector<Circle>& circleData, int weight, double lineRisk){
    vector<Point> nowTurnsData;
    int index = 0;
    double nowCost = 0, prevCost = lineRisk;
    double toX = 0, toY = 0, dist = 0;
    Point high = splitsData.back();
    high.close = splitsData.back().close;

    if((high.x - high.close.x) / (high.y - high.close.y) != (high.x - end.x) / (high.y - end.y)){
        dist = sqrt(pow(high.x - high.close.x, 2) + pow(high.y - high.close.y, 2));
        toX = 10 * (high.x - high.close.x) / dist;
        toY = 10 * (high.y - high.close.y) / dist;
    }else{
        dist = sqrt(pow(end.x - high.close.x, 2) + pow(end.y - high.close.y, 2));
        toX = 10 * (high.close.y - end.y) / dist;
        toY = 10 * (end.x - high.close.x) / dist;
    }

    do{
        if(index != 0){
            prevCost = nowCost;
        }
        high.x += toX;
        high.y += toY;
        turnsData.clear();
        for(int i = 0; i < nowTurnsData.size(); i++){
            turnsData.push_back(nowTurnsData[i]);
        }
        nowTurnsData.clear();
        nowTurnsData.push_back(high);
        double risk = turnRouteRisk(nowTurnsData, start, end, circleData);
        nowCost = weight * nowTurnsData.size() + risk;
        index++;
    }while(nowCost <= prevCost);

    high.x -= toX;
    high.y -= toY;

    if(index == 1){
        index = 0;
        do{
            if(index != 0){
                prevCost = nowCost;
            }
            high.x -= toX;
            high.y -= toY;
            turnsData.clear();
            for(int i = 0; i < nowTurnsData.size(); i++){
                turnsData.push_back(nowTurnsData[i]);
            }
            nowTurnsData.clear();
            nowTurnsData.push_back(high);
            double risk = turnRouteRisk(nowTurnsData, start, end, circleData);
            nowCost = weight * nowTurnsData.size() + risk;
            index++;
        }while(nowCost <= prevCost);
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
        double maxRisk = 0, risk = 0;
        for(int i = 0; i < circleData.size(); i++){
            double r = calPointRisk(temp.x, temp.y,
                                    circleData[i].x, circleData[i].y, circleData[i].power, circleData[i].range);
            if(r > maxRisk){
                temp.close = circleData[i];
            }
            temp.risk += r;
            risk += r;
        }
        splitsData.emplace_back(temp);
        totalRisk += risk;
        progress += 1;
        splits -= 1;
    }
    stable_sort(splitsData.begin(), splitsData.end(),[]
            (const Point& a, const Point& b) -> bool{return a.risk < b.risk;});
    return totalRisk;
}

double turnRouteRisk(const vector<Point>& turnData, const Point& start,
                     const Point& end, const vector<Circle>& circleData){
    vector<double> distData;
    double distTemp = 0, dist = 0;
    for(int i = 0; i <= turnData.size(); i++){
        if(i == 0){
            distTemp = calPointDist(start.x, start.y, turnData[i].x, turnData[i].y);
        }else if(i == turnData.size()){
            distTemp = calPointDist(end.x, end.y, turnData[i - 1].x, turnData[i - 1].y);
        }else{
            distTemp = calPointDist(turnData[i - 1].x, turnData[i - 1].y, turnData[i].x, turnData[i].y);
        }
        distData.push_back(distTemp);
        dist += distTemp;

    }

    double splits = 0, totalRisk = 0, risk = 0, remainDist = 0;
    double tempx = 0, tempy = 0;
    int progress = 1;
    bool justTurn = false;

    if(dist - floor(dist) == 0){
        splits = floor(dist) - 1;
    }else{
        splits = floor(dist);
    }

    while(splits > 0){
        for(int i = 0; i <= turnData.size(); i++){
            while(remainDist + progress < distData[i]){
                Point temp;
                risk = 0;
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
                    risk += calPointRisk(temp.x, temp.y, circleData[j].x, circleData[j].y,
                                         circleData[j].power, circleData[j].range);
                }
                totalRisk += risk;
                splits -= 1;
            }
            remainDist = progress + remainDist - distData[i];
            progress = 1;
            justTurn = true;
        }
    }
    return totalRisk;
}