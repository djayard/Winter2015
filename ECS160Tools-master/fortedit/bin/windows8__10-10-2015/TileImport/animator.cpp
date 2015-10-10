#include "animator.h"

#include <QString>
#include <QDebug>

/*
 * class to render animations
 */
Animator::Animator() {

}

/*
 * sets animations for file
 * @param name of file with animations
 */
void Animator::setAnimNames(QString fileName) {
    qDebug() << fileName;
    QList<QString> tmpAnimNames;
    if ("3DBurn.png" == fileName) {
        tmpAnimNames.append("Rubbleburn_1");
        tmpAnimNames.append("Rubbleburn_2");
        tmpAnimNames.append("Rubbleburn-alt_1");
        tmpAnimNames.append("Rubbleburn-alt_2");
        tmpAnimNames.append("Holeburn_1");
        tmpAnimNames.append("Holeburn_2");
        tmpAnimNames.append("Holeburn-alt_1");
        tmpAnimNames.append("Holeburn-alt_2");
    }
    else if ("3DCannon.png" == fileName) {
        tmpAnimNames.append("Cannon Spin Clockwise");
        tmpAnimNames.append("Cannon Spin Counter Clockwise");
    }
    else if ("3DCannonball.png" == fileName)
        tmpAnimNames.append("Cannon Ball Flying");
    else if ("3DCannonPlume.png" == fileName) {
        tmpAnimNames.append("North Plume");
        tmpAnimNames.append("Northeast Plume");
        tmpAnimNames.append("East Plume");
        tmpAnimNames.append("Southeast Plume");
        tmpAnimNames.append("South Plume");
        tmpAnimNames.append("Southwest Plume");
        tmpAnimNames.append("West Plume");
        tmpAnimNames.append("Northwest Plume");
    }
    else if ("3DCastles.png" == fileName) {
        tmpAnimNames.append("Blue Castle No Wind");
        tmpAnimNames.append("Blue Castle N Wind");
        tmpAnimNames.append("Blue Castle NE Wind");
        tmpAnimNames.append("Blue Castle E Wind");
        tmpAnimNames.append("Blue Castle SE Wind");
        tmpAnimNames.append("Blue Castle S Wind");
        tmpAnimNames.append("Blue Castle SW Wind");
        tmpAnimNames.append("Blue Castle W Wind");
        tmpAnimNames.append("Blue Castle NW Wind");
        tmpAnimNames.append("Red Castle No Wind");
        tmpAnimNames.append("Red Castle N Wind");
        tmpAnimNames.append("Red Castle NE Wind");
        tmpAnimNames.append("Red Castle E Wind");
        tmpAnimNames.append("Red Castle SE Wind");
        tmpAnimNames.append("Red Castle S Wind");
        tmpAnimNames.append("Red Castle SW Wind");
        tmpAnimNames.append("Red Castle W Wind");
        tmpAnimNames.append("Red Castle NW Wind");
        tmpAnimNames.append("Yellow Castle No Wind");
        tmpAnimNames.append("Yellow Castle N Wind");
        tmpAnimNames.append("Yellow Castle NE Wind");
        tmpAnimNames.append("Yellow Castle E Wind");
        tmpAnimNames.append("Yellow Castle SE Wind");
        tmpAnimNames.append("Yellow Castle S Wind");
        tmpAnimNames.append("Yellow Castle SW Wind");
        tmpAnimNames.append("Yellow Castle W Wind");
        tmpAnimNames.append("Yellow Castle NW Wind");
    }
    else if ("3DExplosions.png" == fileName) {
        tmpAnimNames.append("Explosion");
        tmpAnimNames.append("Explosion-alt");
        tmpAnimNames.append("Water Explosion");
        tmpAnimNames.append("Water Explosion-alt");
        tmpAnimNames.append("Ground Explosion");
        tmpAnimNames.append("Ground Explosion-alt");
    }
    else
        tmpAnimNames.append("[None]");
    this->animNames.clear();
    this->animNames = tmpAnimNames;
    tmpAnimNames.clear();
}

/*
 * sets the animation
 * @param tile to animate
 */
void Animator::setAnimNum(QString animName){
    QList<int> tmpAnimNum;

    //simulate random by mixing the numbers
    if ("Rubbleburn_1" == animName) { tmpAnimNum <<  0 << 3 << 2 << 1 << 2 << 1 << 0 << 3; }
    else if ("Rubbleburn_2" == animName) { tmpAnimNum <<  4 << 7 << 5 << 6 << 5 << 7 << 4 << 6; }
    else if ("Rubbleburn-alt_1" == animName) { tmpAnimNum << 10 << 9 << 8 << 11 << 8 << 11 << 10 << 9; }
    else if ("Rubbleburn-alt_2" == animName) { tmpAnimNum << 13 << 14 << 12 << 15 << 12 << 13 << 15 << 14; }
    else if ("Holeburn_1" == animName) { tmpAnimNum << 18 << 16 << 17 << 19 << 18 << 16 << 19 << 17; }
    else if ("Holeburn_2" == animName) { tmpAnimNum << 21 << 23 << 20 << 22 << 20 << 21 << 22 << 23; }
    else if ("Holeburn-alt_1" == animName) { tmpAnimNum << 25 << 27 << 24 << 26 << 24 << 27 << 25 << 26; }
    else if ("Holeburn-alt_2" == animName) { tmpAnimNum << 28 << 29 << 30 << 31 << 29 << 31 << 28 << 30; }
    else if ("Cannon Spin Clockwise" == animName) { tmpAnimNum << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7; }
    else if ("Cannon Spin Counter Clockwise" == animName) { tmpAnimNum << 7 << 6 << 5 << 4 << 3 << 2 << 1; }
    else if ("Cannon Ball Flying" == animName) { tmpAnimNum << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 6 << 5 << 4 << 3 << 2 << 1 << 0; }
    else if ("North Plume" == animName) { tmpAnimNum << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7; }
    else if ("Northeast Plume" == animName) { tmpAnimNum << 8 << 9 << 10 << 11 << 12 << 13 << 14 << 15; }
    else if ("East Plume" == animName) { tmpAnimNum << 16 << 17 << 18 << 19 << 20 << 21 << 22 << 23; }
    else if ("Southeast Plume" == animName) { tmpAnimNum << 24 << 25 << 26 << 27 << 28 << 29 << 30 << 31; }
    else if ("South Plume" == animName) { tmpAnimNum << 32 << 33 << 34 << 35 << 36 << 37 << 38 << 39; }
    else if ("Southwest Plume" == animName) { tmpAnimNum << 40 << 41 << 42 << 43 << 44 << 45 << 46 << 47; }
    else if ("West Plume" == animName) { tmpAnimNum << 48 << 49 << 50 << 51 << 52 << 53 << 54 << 55; }
    else if ("Northwest Plume" == animName) { tmpAnimNum << 56 << 57 << 58 << 59 << 60 << 61 << 62 << 63; }
    else if ("Blue Castle No Wind" == animName) { tmpAnimNum << 1 << 2 << 3 << 4; }
    else if ("Blue Castle N Wind" == animName) { tmpAnimNum << 5 << 6 << 7 << 8; }
    else if ("Blue Castle NE Wind" == animName) { tmpAnimNum << 9 << 10 << 11 << 12; }
    else if ("Blue Castle E Wind" == animName) { tmpAnimNum << 13 << 14 << 15 << 16; }
    else if ("Blue Castle SE Wind" == animName) { tmpAnimNum << 17 << 18 << 19 << 20; }
    else if ("Blue Castle S Wind" == animName) { tmpAnimNum << 21 << 22 << 23 << 24; }
    else if ("Blue Castle SW Wind" == animName) { tmpAnimNum << 25 << 26 << 27 << 28; }
    else if ("Blue Castle W Wind" == animName) { tmpAnimNum << 29 << 30 << 31 << 32; }
    else if ("Blue Castle NW Wind" == animName) { tmpAnimNum << 33 << 34 << 35 << 36; }
    else if ("Red Castle No Wind" == animName) { tmpAnimNum << 37 << 38 << 39 << 40; }
    else if ("Red Castle N Wind" == animName) { tmpAnimNum << 41 << 42 << 43 << 44; }
    else if ("Red Castle NE Wind" == animName) { tmpAnimNum << 45 << 46 << 47 << 48; }
    else if ("Red Castle E Wind" == animName) { tmpAnimNum << 49 << 50 << 51 << 52; }
    else if ("Red Castle SE Wind" == animName) { tmpAnimNum << 53 << 54 << 55 << 56; }
    else if ("Red Castle S Wind" == animName) { tmpAnimNum << 57 << 58 << 59 << 60; }
    else if ("Red Castle SW Wind" == animName) { tmpAnimNum << 61 << 62 << 63 << 64; }
    else if ("Red Castle W Wind" == animName) { tmpAnimNum << 65 << 66 << 67 << 68; }
    else if ("Red Castle NW Wind" == animName) { tmpAnimNum << 69 << 70 << 71 << 72; }
    else if ("Yellow Castle No Wind" == animName) { tmpAnimNum << 73 << 74 << 75 << 76; }
    else if ("Yellow Castle N Wind" == animName) { tmpAnimNum << 77 << 78 << 79 << 80; }
    else if ("Yellow Castle NE Wind" == animName) { tmpAnimNum << 81 << 82 << 83 << 84; }
    else if ("Yellow Castle E Wind" == animName) { tmpAnimNum << 85 << 86 << 87 << 88; }
    else if ("Yellow Castle SE Wind" == animName) { tmpAnimNum << 89 << 90 << 91 << 92; }
    else if ("Yellow Castle S Wind" == animName) { tmpAnimNum << 93 << 94 << 95 << 96; }
    else if ("Yellow Castle SW Wind" == animName) { tmpAnimNum << 97 << 98 << 99 << 100; }
    else if ("Yellow Castle W Wind" == animName) { tmpAnimNum << 101 << 102 << 103 << 104; }
    else if ("Yellow Castle NW Wind" == animName) { tmpAnimNum << 105 << 106 << 107 << 108; }
    else if ("Explosion" == animName) { tmpAnimNum << 0 << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8; }
    else if ("Explosion-alt" == animName) { tmpAnimNum << 9 << 10 << 11 << 12 << 13 << 14 << 15 << 16 << 17; }
    else if ("Water Explosion" == animName) { tmpAnimNum << 18 << 19 << 20 << 21 << 22 << 23 << 24 << 25 << 26; }
    else if ("Water Explosion-alt" == animName) { tmpAnimNum << 27 << 28 << 29 << 30 << 31 << 32 << 33 << 34 << 35; }
    else if ("Ground Explosion" == animName) { tmpAnimNum << 36 << 37 << 38 << 39 << 40 << 41 << 42 << 43 << 44; }
    else if ("Ground Explosion-alt" == animName) { tmpAnimNum << 45 << 46 << 47 << 48 << 49 << 50 << 51 << 52 << 53; }

    this->animNum.clear();
    this->animNum = tmpAnimNum;
    tmpAnimNum.clear();
}
