#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
using namespace std;

double calPointDist(double x1, double y1, double x2, double y2);
double calPointRisk(double x, double y, double riskPointX, double riskPointY, double power, double range);

class Point
{
    friend ostream& operator<<(ostream& out, const Point& pt);
public:
    double x;
    double y;
    double risk;
    bool find;
    Point();
    Point(double x, double y);
    double distToPoint(const Point& aPoint);
};
Point::Point() : x(0), y(0), risk(0), find(false){}
Point::Point(double x, double y) : x(x), y(y), risk(0), find(false){}
double Point::distToPoint(const Point& aPoint)
{
    return calPointDist(aPoint.x, aPoint.y, x, y);
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
    double givePointRisk(const Point& aPoint) const;
};
Circle::Circle() : x(0), y(0), range(0), power(0) {}
Circle::Circle(double x, double y, double range, double power)
        : x(x), y(y), range(range), power(power) {}
double Circle::givePointRisk(const Point &aPoint) const
{
    return calPointRisk(aPoint.x, aPoint.y, x, y, power, range);
}

double noTurnRouteRisk(vector<Point>& splitsData, const Point& start,
                       const Point& end, const vector<Circle>& circleData);
double turnRouteRisk(const vector<Point>& turnData, const Point& start,
                     const Point& end, const vector<Circle> &circleData);
Point findNextPoint(bool map[][1000] , const Point& start, const Point& end, Point nowPt, double distLim,
                    int ratio, int n, const vector<Circle>& circleData);



int main()
{
    int numOfTurns = 1;// for answer

    int n = 0, m = 0, weight = 0, distLim = 0;
    cin >> n >> m >> weight >> distLim;

    bool map[1000][1000] = {false};
    /*
     * ==================================
     * DOWN BELOW : FOR INPUT CIRCLE DATA
     * ==================================
     */
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
/*
  * ==================================
  * TEST: CALCULATE THE ROUTE RISK
  * ==================================
  */
    double x1, y1, x2, y2;
    cin >> x1 >> y1 >> x2 >> y2;
    Point start = Point(x1, y1);
    Point end = Point(x2, y2);
    vector<Point>turnData;
    vector<Point>splitsData;
    /*
     * ==================================
     * USE FOR TURNS
     * ==================================
     */
    int ratio;
    if(n / 50 < 1){
        ratio = 1;
    }else{
        ratio = n / 50;
    }
    noTurnRouteRisk(splitsData, start, end, circleData);
    /*
     * =====================
     * END INPUT CIRCLE DATA
     * =====================
     */
    /*
     * ============================
     * * DRAW CIRCLE RISK DATA ON MAP
     * ============================
     */
    Point tempPoint = Point(floor(splitsData.back().x), floor(splitsData.back().y));
    Point ans = findNextPoint(map, start, end, tempPoint, distLim, ratio, n, circleData);
    cout <<"1 " << ans.x << " " << ans.y;

    /*
     * =================
     * BELOW FOR TESTING
     * drawing a n*n map
     *    with risks
     * =================
     */
    return 0;
}

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

/*
 * SplitsData: contain all the midpoints without a turn, and is ordered with risk
 * TurnData: a vector that contain the turn points
 * circleData: contain all the circles
 */
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
            temp.risk += circleData[i].givePointRisk(temp);
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
                    temp.risk += circleData[j].givePointRisk(temp);
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

Point findNextPoint(bool map[][1000] , const Point& start, const Point& end, Point nowPt,
                    double distLim,int ratio, int n, const vector<Circle> &circleData)
{
    Point bestPoint(nowPt);
    vector<Point>targetData;
    targetData.emplace_back(bestPoint);
    vector<Point>turnData;

    double xFrom, xTo, yFrom, yTo;
    if(nowPt.x - floor(nowPt.x) == 0 && nowPt.y - floor(nowPt.y) == 0){
        xFrom = static_cast<int>(nowPt.x / ratio) * ratio;
        yFrom = static_cast<int>(nowPt.x / ratio) * ratio;
        xTo = xFrom + ratio;
        yTo = yFrom + ratio;
    }else{
        xFrom = static_cast<int>(nowPt.x / ratio) * ratio - ratio;
        yFrom = static_cast<int>(nowPt.x / ratio) * ratio - ratio;
        xTo = xFrom + 2 * ratio;
        yTo = yFrom + 2 * ratio;
    }
    for(int i = (int)xFrom; i <= (int)xTo; i += ratio){
        int tempx = 0, tempy = 0;
        if (i < 0){
            tempx = 0;
        } else if (i > n){
            tempx = n;
        }else{
            tempx = i;
        }
        for(int j = (int)yFrom; j <= (int)yTo; j += ratio){
            if (j < 0){
                tempy = 0;
            } else if (j > n){
                tempy = n;
            }else{
                tempy = j;
            }
            if(!map[tempx][tempy]){
                map[tempx][tempy] = true;
                turnData.clear();
                turnData.emplace_back(Point(tempx, tempy));
                double risk = turnRouteRisk(turnData, start, end, circleData);
                // judge
                if (risk < turnRouteRisk(targetData, start, end, circleData) &&
                    calPointDist(start.x, start.y, tempx, tempy) + calPointDist(tempx, tempy, end.x, end.y) <= distLim &&
                    tempx - start.x / tempy - start.y != end.x - tempx / end.y - tempy){
                    bestPoint = Point(tempx, tempy);
                }
            }
        }
    }
    if(bestPoint.x == nowPt.x && bestPoint.y == nowPt.y){
        return bestPoint;
    }else{
        return findNextPoint(map, start, end, bestPoint, distLim, ratio, n, circleData);
    }
}