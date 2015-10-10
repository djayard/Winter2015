#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <QList>
#include <QString>

/*
 * calss for animations
 */
class Animator {
    public:
        Animator();

        QList<QString> getAnimNames() const { return this->animNames; }
        QList<int> getAnimNum() const { return this->animNum; }
        void setAnimNames(QString fileName);
        void setAnimNum(QString animName);

    protected:
        QList<QString> animNames;
        QList<int> animNum;
};

#endif // ANIMATOR_H
